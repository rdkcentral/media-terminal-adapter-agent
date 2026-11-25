/*
 * Copyright 2026 RDK Management
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "mta_mock.h"
#include <atomic>
#include <cerrno>
#include <condition_variable>
#include <cstring>
#include <mutex>
#include <thread>
#include <vector>

extern "C" {
#include "TR104.h"
#include "cosa_x_cisco_com_mta_internal.h"
void MtaTR104InitResetForTest(void);
int __real_pthread_create(pthread_t *, const pthread_attr_t *,
                          void *(*)(void *), void *);
unsigned int __real_sleep(unsigned int);
FILE *__real_fopen(const char *, const char *);
int __real_mta_hal_start_provisioning(PMTAMGMT_MTA_PROVISIONING_PARAMS params);
}

extern SyscfgMock *g_syscfgMock;
extern MtaHalMock *g_mtaHALMock;
extern rbusMock *g_rbusMock;
extern webconfigFwMock *g_webconfigFwMock;
extern SecureWrapperMock *g_securewrapperMock;
extern SafecLibMock *g_safecLibMock;

using ::testing::_;
using ::testing::DoAll;
using ::testing::Invoke;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::StrEq;

namespace {
struct WorkerControl {
    std::atomic<int> creates{0};
    int createResult = 0;
    void *(*entry)(void *) = nullptr;
    void *arg = nullptr;
    std::vector<unsigned int> sleeps;
    std::vector<int> provisioningResults;
    size_t provisioningCalls = 0;
};

WorkerControl *control = nullptr;
}

extern "C" int __wrap_pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                                     void *(*entry)(void *), void *arg)
{
    if (!control)
        return __real_pthread_create(thread, attr, entry, arg);
    ++control->creates;
    int detachState = 0;
    EXPECT_EQ(0, pthread_attr_getdetachstate(attr, &detachState));
    EXPECT_EQ(PTHREAD_CREATE_DETACHED, detachState);
    if (control->createResult)
        return control->createResult;
    EXPECT_EQ(nullptr, control->entry);
    control->entry = entry;
    control->arg = arg;
    return 0;
}

extern "C" unsigned int __wrap_sleep(unsigned int seconds)
{
    if (!control)
        return __real_sleep(seconds);
    control->sleeps.push_back(seconds);
    return 0;
}

extern "C" FILE *__wrap_fopen(const char *path, const char *mode)
{
    if (control && std::strcmp(path, "/nvram/.vsb64.txt") == 0) {
        errno = ENOENT;
        return nullptr;
    }
    return __real_fopen(path, mode);
}

extern "C" int __wrap_mta_hal_start_provisioning(PMTAMGMT_MTA_PROVISIONING_PARAMS params)
{
    /* If no test control is installed, forward to real HAL mock */
    if (!control) {
        return __real_mta_hal_start_provisioning(params);
    }

    /* Test control is active: validate call and return queued result */
    EXPECT_NE(nullptr, params);
    if (control->provisioningCalls >= control->provisioningResults.size()) {
        ADD_FAILURE() << "Unexpected HAL provisioning call (result list exhausted)";
        return -1;
    }
    return control->provisioningResults[control->provisioningCalls++];
}

class TR104InitTest : public CcspMtaTestFixture {
protected:
    WorkerControl worker;
    char parameter[32] = "Device.Services.VoiceService";
    char *parameters[1] = {parameter};

    void SetUp() override
    {
        CcspMtaTestFixture::SetUp();
        MtaTR104InitResetForTest();
        control = &worker;
        EXPECT_CALL(*g_securewrapperMock, v_secure_system(_, _)).Times(0);
    }

    void TearDown() override
    {
        EXPECT_EQ(nullptr, worker.entry) << "Captured worker must be run before teardown";
        control = nullptr;
        MtaTR104InitResetForTest();
        CcspMtaTestFixture::TearDown();
    }

    void EnableRfc()
    {
        EXPECT_CALL(*g_syscfgMock, syscfg_get(nullptr, StrEq("TR104enable"), _, _))
            .WillRepeatedly(DoAll(::testing::SetArrayArgument<2>("true", "true" + 5),
                                  Return(0)));
    }

    void RunWorker()
    {
        ASSERT_NE(nullptr, worker.entry);
        auto entry = worker.entry;
        worker.entry = nullptr;
        EXPECT_EQ(nullptr, entry(worker.arg));
    }

    void ExpectSuccess()
    {
        EXPECT_CALL(*g_mtaHALMock, mta_hal_getTR104parameterNames(_, _))
            .WillOnce(DoAll(SetArgPointee<0>(parameters), SetArgPointee<1>(1), Return(0)));
        EXPECT_CALL(*g_rbusMock, rbus_open(_, _)).WillOnce(Return(RBUS_ERROR_SUCCESS));
        EXPECT_CALL(*g_rbusMock, rbus_regDataElements(_, _, _))
            .WillOnce(Return(RBUS_ERROR_SUCCESS));
        EXPECT_CALL(*g_webconfigFwMock, register_sub_docs(_, _, _, _)).Times(1);
        EXPECT_CALL(*g_securewrapperMock,
                    v_secure_system(StrEq("systemctl restart CcspTr069PaSsp &"), _))
            .WillOnce(Return(0));
    }
};

TEST_F(TR104InitTest, RepeatedTriggersWhilePendingAndAfterSuccessCreateOnlyOneWorker)
{
    EnableRfc();
    ExpectSuccess();
    MtaTR104Init(FALSE);
    MtaTR104Init(FALSE);
    MtaTR104Init(TRUE);
    EXPECT_EQ(1, worker.creates.load());
    RunWorker();
    MtaTR104Init(FALSE);
    MtaTR104Init(TRUE);
    EXPECT_EQ(1, worker.creates.load());
    EXPECT_TRUE(worker.sleeps.empty());
}

TEST_F(TR104InitTest, ConcurrentTriggersCreateOnlyOneWorker)
{
    EnableRfc();
    ExpectSuccess();
    std::mutex mutex;
    std::condition_variable ready;
    int waiting = 0;
    bool start = false;
    std::vector<std::thread> callers;
    for (int i = 0; i < 8; ++i) {
        callers.emplace_back([&] {
            {
                std::unique_lock<std::mutex> lock(mutex);
                ++waiting;
                ready.notify_all();
                ready.wait(lock, [&] { return start; });
            }
            MtaTR104Init(FALSE);
        });
    }
    {
        std::unique_lock<std::mutex> lock(mutex);
        ready.wait(lock, [&] { return waiting == 8; });
        start = true;
    }
    ready.notify_all();
    for (auto &caller : callers)
        caller.join();
    EXPECT_EQ(1, worker.creates.load());
    RunWorker();
}

TEST_F(TR104InitTest, ThreadCreationFailureAllowsAnotherTrigger)
{
    EnableRfc();
    ExpectSuccess();
    worker.createResult = EAGAIN;
    MtaTR104Init(TRUE);
    EXPECT_EQ(nullptr, worker.entry);
    worker.createResult = 0;
    MtaTR104Init(TRUE);
    EXPECT_EQ(2, worker.creates.load());
    RunWorker();
    EXPECT_EQ(std::vector<unsigned int>({4}), worker.sleeps);
}

TEST_F(TR104InitTest, DisabledRfcDoesNotRegisterAndAllowsLaterEnabledTrigger)
{
    EXPECT_CALL(*g_syscfgMock, syscfg_get(nullptr, StrEq("TR104enable"), _, _))
        .WillOnce(DoAll(::testing::SetArrayArgument<2>("false", "false" + 6),
                        Return(0)));
    EXPECT_CALL(*g_mtaHALMock, mta_hal_getTR104parameterNames(_, _)).Times(0);
    MtaTR104Init(FALSE);
    RunWorker();
    EXPECT_TRUE(worker.sleeps.empty());
    ::testing::Mock::VerifyAndClearExpectations(g_syscfgMock);
    ::testing::Mock::VerifyAndClearExpectations(g_mtaHALMock);
    EnableRfc();
    ExpectSuccess();
    MtaTR104Init(FALSE);
    RunWorker();
    EXPECT_EQ(2, worker.creates.load());
}

TEST_F(TR104InitTest, EthernetExhaustsEightAttemptsThenAcceptsNewTrigger)
{
    EnableRfc();
    EXPECT_CALL(*g_mtaHALMock, mta_hal_getTR104parameterNames(_, _))
        .Times(8).WillRepeatedly(Return(-1));
    MtaTR104Init(TRUE);
    RunWorker();
    EXPECT_EQ(std::vector<unsigned int>(8, 4), worker.sleeps);
    ::testing::Mock::VerifyAndClearExpectations(g_mtaHALMock);
    ExpectSuccess();
    MtaTR104Init(TRUE);
    RunWorker();
    EXPECT_EQ(2, worker.creates.load());
    EXPECT_EQ(std::vector<unsigned int>(9, 4), worker.sleeps);
}

TEST_F(TR104InitTest, DocsisExhaustsTwentyFourAttemptsThenAcceptsNewTrigger)
{
    EnableRfc();
    EXPECT_CALL(*g_mtaHALMock, mta_hal_getTR104parameterNames(_, _))
        .Times(24).WillRepeatedly(Return(-1));
    MtaTR104Init(FALSE);
    RunWorker();
    EXPECT_EQ(std::vector<unsigned int>(24, 10), worker.sleeps);
    ::testing::Mock::VerifyAndClearExpectations(g_mtaHALMock);
    ExpectSuccess();
    MtaTR104Init(FALSE);
    RunWorker();
    EXPECT_EQ(2, worker.creates.load());
    EXPECT_EQ(std::vector<unsigned int>(24, 10), worker.sleeps);
}

TEST_F(TR104InitTest, LaterSuccessfulStartupIPModeProvisioningStartsEthernetWorker)
{
    COSA_MTA_ETHWAN_PROV_INFO provisioning = {};
    provisioning.StartupIPMode.ActiveValue = 1;
    worker.provisioningResults = {-1, RETURN_OK, RETURN_OK};
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_, _, _, _, _))
        .WillRepeatedly(Return(0));
    EXPECT_CALL(*g_syscfgMock, syscfg_set(nullptr, StrEq("MTA_PROVISION"), StrEq("true")))
        .Times(2).WillRepeatedly(Return(0));
    EXPECT_EQ(ANSC_STATUS_FAILURE, CosaSetMTAHal(&provisioning));
    EXPECT_EQ(0, worker.creates.load());

    provisioning.StartupIPMode.ActiveValue = 2;
    EnableRfc();
    ExpectSuccess();
    EXPECT_EQ(ANSC_STATUS_SUCCESS, CosaSetMTAHal(&provisioning));
    EXPECT_EQ(1, worker.creates.load());
    RunWorker();
    EXPECT_EQ(std::vector<unsigned int>({4}), worker.sleeps);
    EXPECT_EQ(ANSC_STATUS_SUCCESS, CosaSetMTAHal(&provisioning));
    EXPECT_EQ(1, worker.creates.load());
    EXPECT_EQ(3u, worker.provisioningCalls);
}
