/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
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

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "mta_mock.h"

using ::testing::Return;
using ::testing::DoAll;
using ::testing::StrEq;
using ::testing::_;

extern "C"
{      
    #include "plugin_main_apis.h"  
}

extern MtaHalMock* g_mtaHALMock;
extern AnscMemoryMock* g_anscMemoryMock;

TEST_F(CcspMtaTestFixture, CosaBackEndManagerCreate) {

    ANSC_HANDLE result = CosaBackEndManagerCreate();
    ASSERT_NE(result, nullptr);

    PCOSA_BACKEND_MANAGER_OBJECT pMyObject = (PCOSA_BACKEND_MANAGER_OBJECT)result;

    EXPECT_EQ(pMyObject->Oid, COSA_DATAMODEL_BASE_OID);
    EXPECT_EQ(pMyObject->Create, CosaBackEndManagerCreate);
    EXPECT_EQ(pMyObject->Remove, CosaBackEndManagerRemove);
    EXPECT_EQ(pMyObject->Initialize, CosaBackEndManagerInitialize);
}

TEST_F(CcspMtaTestFixture, CosaBackEndManagerInitialize) {

    PCOSA_BACKEND_MANAGER_OBJECT pMyObject = (PCOSA_BACKEND_MANAGER_OBJECT)malloc(sizeof(COSA_BACKEND_MANAGER_OBJECT));
    ASSERT_NE(pMyObject, nullptr);
    memset(pMyObject, 0, sizeof(COSA_BACKEND_MANAGER_OBJECT));
    
    EXPECT_CALL(*g_mtaHALMock, mta_hal_InitDB())
        .WillOnce(Return(RETURN_OK));         

    EXPECT_CALL(*g_mtaHALMock, mta_hal_LineTableGetNumberOfEntries())
        .WillOnce(Return(5)); //successful return of the number of entries
        
    ANSC_STATUS status = CosaBackEndManagerInitialize((ANSC_HANDLE)pMyObject);


    EXPECT_EQ(status, ANSC_STATUS_SUCCESS);
    EXPECT_NE(pMyObject->hMTA, nullptr); // Ensure hMTA is initialized

    free(pMyObject);
}

TEST_F(CcspMtaTestFixture, CosaBackEndManagerRemove) {

    PCOSA_BACKEND_MANAGER_OBJECT pMyObject = (PCOSA_BACKEND_MANAGER_OBJECT)malloc(sizeof(COSA_BACKEND_MANAGER_OBJECT));
    ASSERT_NE(pMyObject, nullptr);
    memset(pMyObject, 0, sizeof(COSA_BACKEND_MANAGER_OBJECT));
    
    pMyObject->hMTA = (void*)malloc(1);
    ASSERT_NE(pMyObject->hMTA, nullptr);

    EXPECT_CALL(*g_anscMemoryMock, AnscFreeMemoryOrig((void*)pMyObject->hMTA))
        .Times(1);
    EXPECT_CALL(*g_anscMemoryMock, AnscFreeMemoryOrig((void*)pMyObject))
        .Times(1);

    ANSC_STATUS status = CosaBackEndManagerRemove((ANSC_HANDLE)pMyObject);

    EXPECT_EQ(status, ANSC_STATUS_SUCCESS);
    
    free(pMyObject);
}


