/*
* If not stated otherwise in this file or this component's LICENSE file the
* following copyright and licenses apply:
*
* Copyright 2024 RDK Management
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

#ifndef MTA_MOCK_H
#define MTA_MOCK_H

#include "gtest/gtest.h"

#include <mocks/mock_safec_lib.h>
#include <mocks/mock_usertime.h>
#include <mocks/mock_syscfg.h>
#include <mocks/mock_ansc_memory.h>
#include <mocks/mock_cJSON.h>
#include <mocks/mock_trace.h>
#include <mocks/mock_ansc_file_io.h>
#include <mocks/mock_file_io.h>
#include <mocks/mock_mta_hal.h>
#include <mocks/mock_sysevent.h>
#include <mocks/mock_ansc_wrapper_api.h>
#include <mocks/mock_base_api.h>
#include <mocks/mock_pthread.h>
#include <mocks/mock_webconfigframework.h>
#include <mocks/mock_msgpack.h>
#include <mocks/mock_rbus.h>


extern char g_SubSysPrefix_Irep[32];
extern ULONG g_currentWriteEntity;
extern ULONG g_currentBsUpdate;
extern ANSC_HANDLE g_MessageBusHandle_Irep;

class CcspMtaTestFixture : public ::testing::Test {
  protected:
        SafecLibMock mockedSafecLibMock;
        UserTimeMock mockedUserTime;
        SyscfgMock mockedSyscfg;
        AnscMemoryMock mockedAnscMemory;
        cjsonMock mockedCjson;
        TraceMock mockedTrace;
        AnscFileIOMock mockedAnscFileIO;
        FileIOMock mockedFileIO;
        SyseventMock mockedSysevent;
        MtaHalMock mockedMtaHal;
        AnscWrapperApiMock mockedAnscWrapperApi;
        BaseAPIMock mockedBaseAPI;
        PtdHandlerMock mockedPtdHandler;
        webconfigFwMock mockedwebconfigFw;
        msgpackMock mockedmsgpack;
        rbusMock mockedrbus;


        CcspMtaTestFixture();
        virtual ~CcspMtaTestFixture();
        virtual void SetUp() override;
        virtual void TearDown() override;

        void TestBody() override;
};

#endif // MTA_MOCK_H

