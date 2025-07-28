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

#include <gmock/gmock.h>
#include "mta_mock.h"

SafecLibMock * g_safecLibMock = NULL;
UserTimeMock * g_usertimeMock = NULL;
SyscfgMock * g_syscfgMock = NULL;
AnscMemoryMock * g_anscMemoryMock = NULL;
cjsonMock *g_cjsonMock = NULL;
TraceMock * g_traceMock = NULL;
AnscFileIOMock* g_anscFileIOMock = NULL;
FileIOMock * g_fileIOMock = NULL;
SyseventMock *g_syseventMock = NULL;
MtaHalMock *g_mtaHALMock = NULL;
AnscWrapperApiMock * g_anscWrapperApiMock = NULL;
BaseAPIMock * g_baseapiMock = NULL;
PtdHandlerMock * g_PtdHandlerMock = NULL;
webconfigFwMock *g_webconfigFwMock = NULL;
msgpackMock *g_msgpackMock = NULL;
rbusMock *g_rbusMock = NULL;


ANSC_HANDLE g_MessageBusHandle_Irep = NULL;
char  g_SubSysPrefix_Irep[32] = {0};
ULONG g_currentWriteEntity = 0;
ULONG g_currentBsUpdate = 0; 

CcspMtaTestFixture::CcspMtaTestFixture()
{
    g_safecLibMock = new SafecLibMock;
    g_usertimeMock = new UserTimeMock;
    g_syscfgMock = new SyscfgMock;
    g_anscMemoryMock = new AnscMemoryMock;
    g_cjsonMock = new cjsonMock;
    g_traceMock = new TraceMock;
    g_anscFileIOMock = new AnscFileIOMock;
    g_fileIOMock = new FileIOMock;
    g_syseventMock = new SyseventMock;
    g_mtaHALMock = new MtaHalMock;
    g_anscWrapperApiMock = new AnscWrapperApiMock;
    g_baseapiMock  = new BaseAPIMock;
    g_PtdHandlerMock = new PtdHandlerMock;
    g_webconfigFwMock = new webconfigFwMock;
    g_msgpackMock = new msgpackMock;
    g_rbusMock = new rbusMock;
}

CcspMtaTestFixture::~CcspMtaTestFixture()
{
    delete g_safecLibMock;
    delete g_usertimeMock;
    delete g_syscfgMock;
    delete g_anscMemoryMock;
    delete g_cjsonMock;
    delete g_fileIOMock;
    delete g_syseventMock;
    delete g_mtaHALMock;
    delete g_anscWrapperApiMock;
    delete g_baseapiMock;
    delete g_traceMock;
    delete g_PtdHandlerMock;
    delete g_webconfigFwMock;
    delete g_msgpackMock;
    delete g_rbusMock;

    g_safecLibMock = nullptr;
    g_usertimeMock = nullptr;
    g_syscfgMock = nullptr;
    g_anscMemoryMock = nullptr;
    g_cjsonMock = nullptr;
    g_fileIOMock = nullptr;
    g_mtaHALMock =nullptr;
    g_syseventMock =nullptr;
    g_anscWrapperApiMock = nullptr;
    g_baseapiMock = nullptr;
    g_PtdHandlerMock = nullptr;
    g_webconfigFwMock = nullptr;
    g_msgpackMock = nullptr;
    g_rbusMock = nullptr;

}

void CcspMtaTestFixture::SetUp() {}
void CcspMtaTestFixture::TearDown() {}
void CcspMtaTestFixture::TestBody() {}

// end of file
