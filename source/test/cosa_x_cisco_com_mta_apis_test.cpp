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
using ::testing::ReturnArg;
using ::testing::SetArrayArgument;
using ::testing::DoAll;
using ::testing::StrEq;
using ::testing::SetArgPointee;
using ::testing::_;


extern "C"
{      
    #include "mta_hal.h"
    #include "cosa_x_cisco_com_mta_apis.h"  
    
    ANSC_STATUS fillCurrentPartnerId
    (

        char*                       pValue,
        PULONG                      pulSize
    );
}


extern MtaHalMock* g_mtaHALMock;
extern AnscWrapperApiMock * g_anscWrapperApiMock;
extern PtdHandlerMock * g_PtdHandlerMock;
extern SyscfgMock * g_syscfgMock;

// Test cases for CosaDmlMTAInit
TEST_F(CcspMtaTestFixture, CosaDmlMTAInit_SuccessCase) {

    EXPECT_CALL(*g_mtaHALMock, mta_hal_InitDB())
        .WillOnce(Return(RETURN_OK));

    ANSC_STATUS result = CosaDmlMTAInit(NULL, NULL);

    EXPECT_EQ(result, ANSC_STATUS_SUCCESS);
}

TEST_F(CcspMtaTestFixture, CosaDmlMTAInit_FailureCase) {

    EXPECT_CALL(*g_mtaHALMock, mta_hal_InitDB())
        .WillOnce(Return(RETURN_ERR));

    ANSC_STATUS result = CosaDmlMTAInit(NULL, NULL);

    EXPECT_EQ(result, ANSC_STATUS_FAILURE);
}


// Test cases for CosaDmlMTAGetDHCPInfo
TEST_F(CcspMtaTestFixture, CosaDmlMTAGetDHCPInfo_SuccessCase) {

    PCOSA_MTA_DHCP_INFO pInfo = (PCOSA_MTA_DHCP_INFO)malloc(sizeof(COSA_MTA_DHCP_INFO));

    ASSERT_NE(pInfo, nullptr);
    memset(pInfo, 0, sizeof(COSA_MTA_DHCP_INFO));

    EXPECT_CALL(*g_mtaHALMock, mta_hal_GetDHCPInfo(_))
        .WillOnce(Return(RETURN_OK));

    ANSC_STATUS result = CosaDmlMTAGetDHCPInfo(NULL, pInfo);
    EXPECT_EQ(result, ANSC_STATUS_SUCCESS);
    free(pInfo);
}

TEST_F(CcspMtaTestFixture, CosaDmlMTAGetDHCPInfo_FailureCase) {

    PCOSA_MTA_DHCP_INFO pInfo = (PCOSA_MTA_DHCP_INFO)malloc(sizeof(COSA_MTA_DHCP_INFO));

    ASSERT_NE(pInfo, nullptr);
    memset(pInfo, 0, sizeof(COSA_MTA_DHCP_INFO));

    EXPECT_CALL(*g_mtaHALMock, mta_hal_GetDHCPInfo(_))
        .WillOnce(Return(RETURN_ERR));

    ANSC_STATUS result = CosaDmlMTAGetDHCPInfo(NULL, pInfo);
    EXPECT_EQ(result, ANSC_STATUS_FAILURE);
    free(pInfo);
}

// Test cases for CosaDmlMTAGetDHCPV6Info
TEST_F(CcspMtaTestFixture, CosaDmlMTAGetDHCPV6Info_SuccessCase) {

    PCOSA_MTA_DHCPv6_INFO pInfo = (PCOSA_MTA_DHCPv6_INFO)malloc(sizeof(COSA_MTA_DHCPv6_INFO));
    ASSERT_NE(pInfo, nullptr);  
    memset(pInfo, 0, sizeof(COSA_MTA_DHCPv6_INFO)); 

    EXPECT_CALL(*g_mtaHALMock, mta_hal_GetDHCPV6Info(_))
        .WillOnce(Return(RETURN_OK));

    ANSC_STATUS result = CosaDmlMTAGetDHCPV6Info(NULL, pInfo);

    EXPECT_EQ(result, ANSC_STATUS_SUCCESS);

    free(pInfo); 
}

TEST_F(CcspMtaTestFixture, CosaDmlMTAGetDHCPV6Info_FailureCase) {
    
    PCOSA_MTA_DHCPv6_INFO pInfo = (PCOSA_MTA_DHCPv6_INFO)malloc(sizeof(COSA_MTA_DHCPv6_INFO));
    ASSERT_NE(pInfo, nullptr);  
    memset(pInfo, 0, sizeof(COSA_MTA_DHCPv6_INFO)); 

    EXPECT_CALL(*g_mtaHALMock, mta_hal_GetDHCPV6Info(_))
        .WillOnce(Return(RETURN_ERR));

    ANSC_STATUS result = CosaDmlMTAGetDHCPV6Info(NULL, pInfo);

    EXPECT_EQ(result, ANSC_STATUS_FAILURE);

    free(pInfo);  
}

// Test case for CosaDmlMTAGetPktc
TEST_F(CcspMtaTestFixture, CosaDmlMTAGetPktc_SuccessCase) {

    PCOSA_MTA_PKTC pPktc = (PCOSA_MTA_PKTC)malloc(sizeof(COSA_MTA_PKTC));
    ASSERT_NE(pPktc, nullptr);  
    memset(pPktc, 0, sizeof(COSA_MTA_PKTC)); 

    ANSC_STATUS result = CosaDmlMTAGetPktc(NULL, pPktc);

    EXPECT_EQ(result, ANSC_STATUS_SUCCESS);

    free(pPktc);
}

// Test case for CosaDmlMTASetPktc
TEST_F(CcspMtaTestFixture, CosaDmlMTASetPktc_SuccessCase) {

    PCOSA_MTA_PKTC pPktc = (PCOSA_MTA_PKTC)malloc(sizeof(COSA_MTA_PKTC));
    ASSERT_NE(pPktc, nullptr);  
    memset(pPktc, 0, sizeof(COSA_MTA_PKTC));  

    ANSC_STATUS result = CosaDmlMTASetPktc(NULL, pPktc);

    EXPECT_EQ(result, ANSC_STATUS_SUCCESS);

    free(pPktc);
}

// Test cases for CosaDmlMTALineTableGetNumberOfEntries
TEST_F(CcspMtaTestFixture, CosaDmlMTALineTableGetNumberOfEntries_Success) {

    EXPECT_CALL(*g_mtaHALMock, mta_hal_LineTableGetNumberOfEntries())
        .WillOnce(Return(5)); //successful return of the number of entries

    ULONG result = CosaDmlMTALineTableGetNumberOfEntries(NULL);

    EXPECT_EQ(result, 5);
}

TEST_F(CcspMtaTestFixture, CosaDmlMTALineTableGetNumberOfEntries_Failure) {

    EXPECT_CALL(*g_mtaHALMock, mta_hal_LineTableGetNumberOfEntries())
        .WillOnce(Return(0));

    ULONG result = CosaDmlMTALineTableGetNumberOfEntries(NULL);

    EXPECT_EQ(result, 0);
}

// Test cases for CosaDmlMTALineTableGetEntry
TEST_F(CcspMtaTestFixture, CosaDmlMTALineTableGetEntry_Success) {

    PCOSA_MTA_LINETABLE_INFO pEntry = (PCOSA_MTA_LINETABLE_INFO)malloc(sizeof(COSA_MTA_LINETABLE_INFO));
    ASSERT_NE(pEntry, nullptr);
    memset(pEntry, 0, sizeof(COSA_MTA_LINETABLE_INFO));

    EXPECT_CALL(*g_mtaHALMock, mta_hal_LineTableGetEntry(_,_))
        .WillOnce(Return(RETURN_OK));

    ANSC_STATUS result = CosaDmlMTALineTableGetEntry(NULL, 0, pEntry);

    EXPECT_EQ(result, ANSC_STATUS_SUCCESS);

    free(pEntry);
}

TEST_F(CcspMtaTestFixture, CosaDmlMTALineTableGetEntry_Failure) {

    PCOSA_MTA_LINETABLE_INFO pEntry = (PCOSA_MTA_LINETABLE_INFO)malloc(sizeof(COSA_MTA_LINETABLE_INFO));
    ASSERT_NE(pEntry, nullptr);
    memset(pEntry, 0, sizeof(COSA_MTA_LINETABLE_INFO));

    
    EXPECT_CALL(*g_mtaHALMock, mta_hal_LineTableGetEntry(_,_))
        .WillOnce(Return(RETURN_ERR));

    ANSC_STATUS result = CosaDmlMTALineTableGetEntry(NULL, 0, pEntry);

    EXPECT_EQ(result, ANSC_STATUS_FAILURE);

    free(pEntry);
}

//Test cases for CosaDmlMTATriggerDiagnostics
TEST_F(CcspMtaTestFixture, CosaDmlMTATriggerDiagnostics_Success) {

    EXPECT_CALL(*g_mtaHALMock, mta_hal_TriggerDiagnostics(::testing::_))
        .WillOnce(Return(RETURN_OK));

    ANSC_STATUS result = CosaDmlMTATriggerDiagnostics(0);

    EXPECT_EQ(result, ANSC_STATUS_SUCCESS);
}

TEST_F(CcspMtaTestFixture, CosaDmlMTATriggerDiagnostics_Failure) {

    EXPECT_CALL(*g_mtaHALMock, mta_hal_TriggerDiagnostics(_))
        .WillOnce(Return(RETURN_ERR));

    ANSC_STATUS result = CosaDmlMTATriggerDiagnostics(0);

    EXPECT_EQ(result, ANSC_STATUS_FAILURE);
}

// Test case for CosaDmlMTAGetServiceClass
TEST_F(CcspMtaTestFixture, CosaDmlMTAGetServiceClass_Success) {

    ULONG count = 5;
    PCOSA_MTA_SERVICE_CLASS pCfg = (PCOSA_MTA_SERVICE_CLASS)malloc(sizeof(COSA_MTA_SERVICE_CLASS));
    ASSERT_NE(pCfg, nullptr);
    memset(pCfg, 0, sizeof(COSA_MTA_SERVICE_CLASS));


    EXPECT_EQ(count, 5); //checking the value of count variable before the fucntion call.
    ANSC_STATUS result = CosaDmlMTAGetServiceClass(nullptr, &count, &pCfg);

    EXPECT_EQ(result, ANSC_STATUS_SUCCESS);
    EXPECT_EQ(count, 0); // checking the value of count varibale after the updation of its value in the source code. 

    free(pCfg);
}

//Test cases for CosaDmlMTADectGetEnable
TEST_F(CcspMtaTestFixture, CosaDmlMTADectGetEnable_Success) {

    EXPECT_CALL(*g_mtaHALMock, mta_hal_DectGetEnable(_))
        .WillOnce(Return(RETURN_OK)); 

    BOOLEAN bBool = false; 
    ANSC_STATUS result = CosaDmlMTADectGetEnable(NULL, &bBool);

    EXPECT_EQ(result, ANSC_STATUS_SUCCESS); 

}

TEST_F(CcspMtaTestFixture, CosaDmlMTADectGetEnable_Failure) {
    BOOLEAN bBool = false;
    EXPECT_CALL(*g_mtaHALMock, mta_hal_DectGetEnable(_))
        .WillOnce(Return(RETURN_ERR)); 

    ANSC_STATUS result = CosaDmlMTADectGetEnable(NULL, &bBool);

    EXPECT_EQ(result, ANSC_STATUS_FAILURE);
}

// Test cases for CosaDmlMTADectSetEnable
TEST_F(CcspMtaTestFixture, CosaDmlMTADectSetEnable_Success) {

    EXPECT_CALL(*g_mtaHALMock, mta_hal_DectSetEnable(true))
        .WillOnce(Return(RETURN_OK));

    ANSC_STATUS result = CosaDmlMTADectSetEnable(NULL, true);

    EXPECT_EQ(result, ANSC_STATUS_SUCCESS);
}

TEST_F(CcspMtaTestFixture, CosaDmlMTADectSetEnable_Failure) {
    EXPECT_CALL(*g_mtaHALMock, mta_hal_DectSetEnable(true))
        .WillOnce(Return(RETURN_ERR));

    ANSC_STATUS result = CosaDmlMTADectSetEnable(NULL, true);

    EXPECT_EQ(result, ANSC_STATUS_FAILURE);
}

// Test cases for CosaDmlMTADectGetRegistrationMode
TEST_F(CcspMtaTestFixture, CosaDmlMTADectGetRegistrationMode_Success) {
    
    EXPECT_CALL(*g_mtaHALMock, mta_hal_DectGetRegistrationMode(_))
        .WillOnce(Return(RETURN_OK));
    
    BOOLEAN bBool = false;
    ANSC_STATUS result = CosaDmlMTADectGetRegistrationMode(NULL, &bBool);

    EXPECT_EQ(result, ANSC_STATUS_SUCCESS);
}

TEST_F(CcspMtaTestFixture, CosaDmlMTADectGetRegistrationMode_Failure) {
    BOOLEAN bBool = false;
    EXPECT_CALL(*g_mtaHALMock, mta_hal_DectGetRegistrationMode(_))
        .WillOnce(Return(RETURN_ERR));

    ANSC_STATUS result = CosaDmlMTADectGetRegistrationMode(NULL, &bBool);

    EXPECT_EQ(result, ANSC_STATUS_FAILURE);
}

// Test cases for CosaDmlMTADectSetRegistrationMode
TEST_F(CcspMtaTestFixture, CosaDmlMTADectSetRegistrationMode_Success) {

    EXPECT_CALL(*g_mtaHALMock, mta_hal_DectSetRegistrationMode(true))
        .WillOnce(Return(RETURN_OK));

    ANSC_STATUS result = CosaDmlMTADectSetRegistrationMode(NULL, true);

    EXPECT_EQ(result, ANSC_STATUS_SUCCESS);
}

TEST_F(CcspMtaTestFixture, CosaDmlMTADectSetRegistrationMode_Failure) {

    EXPECT_CALL(*g_mtaHALMock, mta_hal_DectSetRegistrationMode(true))
        .WillOnce(Return(RETURN_ERR));

    ANSC_STATUS result = CosaDmlMTADectSetRegistrationMode(NULL, true);

    EXPECT_EQ(result, ANSC_STATUS_FAILURE);
}

// Test cases for CosaDmlMTADectRegisterDectHandset
TEST_F(CcspMtaTestFixture, CosaDmlMTADectRegisterDectHandset_Success) {
    
    ANSC_HANDLE hContext = nullptr; 
    ULONG uValue = 123; 

    ANSC_STATUS result = CosaDmlMTADectRegisterDectHandset(hContext, uValue);

    EXPECT_EQ(result, ANSC_STATUS_SUCCESS);
}

// Test cases for CosaDmlMTADectDeregisterDectHandset
TEST_F(CcspMtaTestFixture, CosaDmlMTADectDeregisterDectHandset_Success) {

    EXPECT_CALL(*g_mtaHALMock, mta_hal_DectDeregisterDectHandset(1234))
        .WillOnce(Return(RETURN_OK));

    ANSC_STATUS result = CosaDmlMTADectDeregisterDectHandset(NULL, 1234);

    EXPECT_EQ(result, ANSC_STATUS_SUCCESS);
}

TEST_F(CcspMtaTestFixture, CosaDmlMTADectDeregisterDectHandset_Failure) {

    EXPECT_CALL(*g_mtaHALMock, mta_hal_DectDeregisterDectHandset(1234))
        .WillOnce(Return(RETURN_ERR));

    ANSC_STATUS result = CosaDmlMTADectDeregisterDectHandset(NULL, 1234);

    EXPECT_EQ(result, ANSC_STATUS_FAILURE);
}

//Test cases for CosaDmlMTAGetDect
TEST_F(CcspMtaTestFixture, CosaDmlMTAGetDect_Success) {
   
    PCOSA_MTA_DECT pDect = (PCOSA_MTA_DECT)malloc(sizeof(COSA_MTA_DECT));
    ASSERT_NE(pDect, nullptr); 
    memset(pDect, 0, sizeof(COSA_MTA_DECT));

    EXPECT_CALL(*g_mtaHALMock, mta_hal_GetDect(_))
        .WillOnce(Return(RETURN_OK));

    ANSC_STATUS result = CosaDmlMTAGetDect(NULL, pDect);

    EXPECT_EQ(result, ANSC_STATUS_SUCCESS);
    free(pDect);
}

TEST_F(CcspMtaTestFixture, CosaDmlMTAGetDect_Failure) {
   
    PCOSA_MTA_DECT pDect = (PCOSA_MTA_DECT)malloc(sizeof(COSA_MTA_DECT));
    ASSERT_NE(pDect, nullptr); 
    memset(pDect, 0, sizeof(COSA_MTA_DECT));

    EXPECT_CALL(*g_mtaHALMock, mta_hal_GetDect(_))
        .WillOnce(Return(RETURN_ERR));

    ANSC_STATUS result = CosaDmlMTAGetDect(NULL, pDect);

    EXPECT_EQ(result, ANSC_STATUS_FAILURE);
    free(pDect);
}

//Test cases of CosaDmlMTAGetDectPIN
TEST_F(CcspMtaTestFixture, CosaDmlMTAGetDectPIN_Success) {

    char pinString[128] = {}; 

    EXPECT_CALL(*g_mtaHALMock, mta_hal_GetDectPIN(_))
        .WillOnce(Return(RETURN_OK)); 

    ANSC_STATUS result = CosaDmlMTAGetDectPIN(NULL, pinString);

    EXPECT_EQ(result, ANSC_STATUS_SUCCESS); 

}

TEST_F(CcspMtaTestFixture, CosaDmlMTAGetDectPIN_Failure) {

    char pinString[128] = {}; 

    EXPECT_CALL(*g_mtaHALMock, mta_hal_GetDectPIN(::testing::_))
        .WillOnce(Return(RETURN_ERR)); 

    ANSC_STATUS result = CosaDmlMTAGetDectPIN(NULL, pinString);

    EXPECT_EQ(result, ANSC_STATUS_FAILURE); 
}

//Test cases for CosaDmlMTASetDectPIN
TEST_F(CcspMtaTestFixture, CosaDmlMTASetDectPIN_Success) {

    const char pinString[] = "5678"; 

    EXPECT_CALL(*g_mtaHALMock, mta_hal_SetDectPIN(StrEq(pinString)))
        .WillOnce(Return(RETURN_OK));

    ANSC_STATUS result = CosaDmlMTASetDectPIN(NULL, (char*)pinString);

    EXPECT_EQ(result, ANSC_STATUS_SUCCESS);
}

TEST_F(CcspMtaTestFixture, CosaDmlMTASetDectPIN_Failure) {

    const char pinString[] = "5678"; 

    EXPECT_CALL(*g_mtaHALMock, mta_hal_SetDectPIN(StrEq(pinString)))
        .WillOnce(Return(RETURN_ERR));

    ANSC_STATUS result = CosaDmlMTASetDectPIN(NULL, (char*)pinString);

    EXPECT_EQ(result, ANSC_STATUS_FAILURE); 
}

//Test case for CosaDmlMTASetHandsets
TEST_F(CcspMtaTestFixture, CosaDmlMTASetHandsets_Success) {
    
    PCOSA_MTA_HANDSETS_INFO pHandsets = (PCOSA_MTA_HANDSETS_INFO)malloc(sizeof(COSA_MTA_HANDSETS_INFO));
    ASSERT_NE(pHandsets, nullptr); 
    memset(pHandsets, 0, sizeof(COSA_MTA_HANDSETS_INFO));

    ANSC_STATUS result = CosaDmlMTASetHandsets(nullptr, pHandsets);

    EXPECT_EQ(result, ANSC_STATUS_SUCCESS);
    free(pHandsets);
}

//Test case for CosaDmlMTAVQMResetStats
TEST_F(CcspMtaTestFixture, CosaDmlMTAVQMResetStats_Success) {

    ANSC_HANDLE hContext = reinterpret_cast<ANSC_HANDLE>(0x1234);

    ANSC_STATUS result = CosaDmlMTAVQMResetStats(hContext);

    EXPECT_EQ(result, ANSC_STATUS_SUCCESS);
}

//Test cases for CosaDmlMTAGetCALLP
TEST_F(CcspMtaTestFixture, CosaDmlMTAGetCALLP_Success) {

    // Allocate and initialize the PCOSA_MTA_CAPPL object
    PCOSA_MTA_CAPPL pCallp = (PCOSA_MTA_CAPPL)malloc(sizeof(COSA_MTA_CAPPL));
    ASSERT_NE(pCallp, nullptr);
    memset(pCallp, 0, sizeof(COSA_MTA_CAPPL));

    // Allocate and initialize the PCOSA_MTA_LINETABLE_INFO object
    PCOSA_MTA_LINETABLE_INFO pLineTable = (PCOSA_MTA_LINETABLE_INFO)malloc(sizeof(COSA_MTA_LINETABLE_INFO));
    ASSERT_NE(pLineTable, nullptr);
    memset(pLineTable, 0, sizeof(COSA_MTA_LINETABLE_INFO));
    pLineTable->LineNumber = 1; 

    EXPECT_CALL(*g_mtaHALMock, mta_hal_GetCALLP(pLineTable->LineNumber,_))
        .WillOnce(Return(RETURN_OK));

    ANSC_STATUS result = CosaDmlMTAGetCALLP(reinterpret_cast<ANSC_HANDLE>(pLineTable), pCallp);

    EXPECT_EQ(result, ANSC_STATUS_SUCCESS);
    free(pCallp);
    free(pLineTable);
}

TEST_F(CcspMtaTestFixture, CosaDmlMTAGetCALLP_Failure) {

    // Allocate and initialize the PCOSA_MTA_CAPPL object
    PCOSA_MTA_CAPPL pCallp = (PCOSA_MTA_CAPPL)malloc(sizeof(COSA_MTA_CAPPL));
    ASSERT_NE(pCallp, nullptr);
    memset(pCallp, 0, sizeof(COSA_MTA_CAPPL));

    // Allocate and initialize the PCOSA_MTA_LINETABLE_INFO object
    PCOSA_MTA_LINETABLE_INFO pLineTable = (PCOSA_MTA_LINETABLE_INFO)malloc(sizeof(COSA_MTA_LINETABLE_INFO));
    ASSERT_NE(pLineTable, nullptr);
    memset(pLineTable, 0, sizeof(COSA_MTA_LINETABLE_INFO));
    pLineTable->LineNumber = 1; 

    EXPECT_CALL(*g_mtaHALMock, mta_hal_GetCALLP(pLineTable->LineNumber,_))
        .WillOnce(Return(RETURN_ERR));

    ANSC_STATUS result = CosaDmlMTAGetCALLP(reinterpret_cast<ANSC_HANDLE>(pLineTable), pCallp);

    EXPECT_EQ(result, ANSC_STATUS_FAILURE);
    free(pCallp);
    free(pLineTable);
}

//Test cases for CosaDmlMTAGetDSXLogEnable
TEST_F(CcspMtaTestFixture, CosaDmlMTAGetDSXLogEnable_Success) {

    BOOLEAN dsxLogEnable = FALSE;

    EXPECT_CALL(*g_mtaHALMock, mta_hal_GetDSXLogEnable(_))
        .WillOnce(Return(RETURN_OK));

    ANSC_STATUS result = CosaDmlMTAGetDSXLogEnable(nullptr, &dsxLogEnable);

    EXPECT_EQ(result, ANSC_STATUS_SUCCESS);
}


TEST_F(CcspMtaTestFixture, CosaDmlMTAGetDSXLogEnable_Failure) {

    BOOLEAN dsxLogEnable = FALSE;

    EXPECT_CALL(*g_mtaHALMock, mta_hal_GetDSXLogEnable(_))
        .WillOnce(Return(RETURN_ERR));

    
    ANSC_STATUS result = CosaDmlMTAGetDSXLogEnable(nullptr, &dsxLogEnable);

    EXPECT_EQ(result, ANSC_STATUS_FAILURE);
}

//Test cases for CosaDmlMTASetDSXLogEnable
TEST_F(CcspMtaTestFixture, CosaDmlMTASetDSXLogEnable_Success) {

    BOOLEAN dsxLogEnable = TRUE;

    EXPECT_CALL(*g_mtaHALMock, mta_hal_SetDSXLogEnable(dsxLogEnable))
        .WillOnce(Return(RETURN_OK));

    ANSC_STATUS result = CosaDmlMTASetDSXLogEnable(nullptr, dsxLogEnable);

    EXPECT_EQ(result, ANSC_STATUS_SUCCESS);
}

TEST_F(CcspMtaTestFixture, CosaDmlMTASetDSXLogEnable_Failure) {

    BOOLEAN dsxLogEnable = TRUE;

    EXPECT_CALL(*g_mtaHALMock, mta_hal_SetDSXLogEnable(dsxLogEnable))
        .WillOnce(Return(RETURN_ERR));

    ANSC_STATUS result = CosaDmlMTASetDSXLogEnable(nullptr, dsxLogEnable);

    EXPECT_EQ(result, ANSC_STATUS_FAILURE);
}

//Test cases for CosaDmlMTAClearDSXLog
TEST_F(CcspMtaTestFixture, CosaDmlMTAClearDSXLog_Success) {

    BOOLEAN clearLog = TRUE;

    EXPECT_CALL(*g_mtaHALMock, mta_hal_ClearDSXLog(clearLog))
        .WillOnce(Return(RETURN_OK));

    ANSC_STATUS result = CosaDmlMTAClearDSXLog(nullptr, clearLog);
    EXPECT_EQ(result, ANSC_STATUS_SUCCESS);
}


TEST_F(CcspMtaTestFixture, CosaDmlMTAClearDSXLog_Failure) {

    BOOLEAN clearLog = TRUE;

    EXPECT_CALL(*g_mtaHALMock, mta_hal_ClearDSXLog(clearLog))
        .WillOnce(Return(RETURN_ERR));

    ANSC_STATUS result = CosaDmlMTAClearDSXLog(nullptr, clearLog);
    EXPECT_EQ(result, ANSC_STATUS_FAILURE);
}

//Test cases for CosaDmlMTAGetCallSignallingLogEnable
TEST_F(CcspMtaTestFixture, CosaDmlMTAGetCallSignallingLogEnable_Success) {

    BOOLEAN callSignallingLogEnable = FALSE;

    EXPECT_CALL(*g_mtaHALMock, mta_hal_GetCallSignallingLogEnable(_))
        .WillOnce(Return(RETURN_OK));

    ANSC_STATUS result = CosaDmlMTAGetCallSignallingLogEnable(nullptr, &callSignallingLogEnable);

    EXPECT_EQ(result, ANSC_STATUS_SUCCESS);
}

TEST_F(CcspMtaTestFixture, CosaDmlMTAGetCallSignallingLogEnable_Failure) {

    BOOLEAN callSignallingLogEnable = FALSE;

    EXPECT_CALL(*g_mtaHALMock, mta_hal_GetCallSignallingLogEnable(::testing::_))
        .WillOnce(Return(RETURN_ERR));

    ANSC_STATUS result = CosaDmlMTAGetCallSignallingLogEnable(nullptr, &callSignallingLogEnable);

    EXPECT_EQ(result, ANSC_STATUS_FAILURE);
}

//Test cases for CosaDmlMTASetCallSignallingLogEnable
TEST_F(CcspMtaTestFixture, CosaDmlMTASetCallSignallingLogEnable_Success) {

    BOOLEAN callSignallingLogEnable = TRUE;

    EXPECT_CALL(*g_mtaHALMock, mta_hal_SetCallSignallingLogEnable(callSignallingLogEnable))
        .WillOnce(Return(RETURN_OK));

    ANSC_STATUS result = CosaDmlMTASetCallSignallingLogEnable(nullptr, callSignallingLogEnable);

    EXPECT_EQ(result, ANSC_STATUS_SUCCESS);
}

TEST_F(CcspMtaTestFixture, CosaDmlMTASetCallSignallingLogEnable_Failure) {

    BOOLEAN callSignallingLogEnable = TRUE;

    EXPECT_CALL(*g_mtaHALMock, mta_hal_SetCallSignallingLogEnable(callSignallingLogEnable))
        .WillOnce(Return(RETURN_ERR));

    ANSC_STATUS result = CosaDmlMTASetCallSignallingLogEnable(nullptr, callSignallingLogEnable);

    EXPECT_EQ(result, ANSC_STATUS_FAILURE);
}

// Test cased for CosaDmlMtaBatteryGetInstalled
TEST_F(CcspMtaTestFixture, CosaDmlMtaBatteryGetInstalled_Success) {

    BOOL isInstalled = FALSE;

    EXPECT_CALL(*g_mtaHALMock, mta_hal_BatteryGetInstalled(::testing::_))
        .WillOnce(Return(RETURN_OK));

    ANSC_STATUS result = CosaDmlMtaBatteryGetInstalled(nullptr, &isInstalled);

    EXPECT_EQ(result, ANSC_STATUS_SUCCESS);
}

TEST_F(CcspMtaTestFixture, CosaDmlMtaBatteryGetInstalled_Failure) {

    BOOL isInstalled = FALSE;

    EXPECT_CALL(*g_mtaHALMock, mta_hal_BatteryGetInstalled(_))
        .WillOnce(Return(RETURN_ERR));

    ANSC_STATUS result = CosaDmlMtaBatteryGetInstalled(nullptr, &isInstalled);

    EXPECT_EQ(result, ANSC_STATUS_FAILURE);
}

//Test cases for CosaDmlMtaBatteryGetTotalCapacity
TEST_F(CcspMtaTestFixture, CosaDmlMtaBatteryGetTotalCapacity_Success) {

    ULONG totalCapacity = 0;

    EXPECT_CALL(*g_mtaHALMock, mta_hal_BatteryGetTotalCapacity(::testing::_))
        .WillOnce(Return(RETURN_OK));

    ANSC_STATUS result = CosaDmlMtaBatteryGetTotalCapacity(nullptr, &totalCapacity);

    EXPECT_EQ(result, ANSC_STATUS_SUCCESS);
}

TEST_F(CcspMtaTestFixture, CosaDmlMtaBatteryGetTotalCapacity_Failure) {

    ULONG totalCapacity = 0;

    EXPECT_CALL(*g_mtaHALMock, mta_hal_BatteryGetTotalCapacity(::testing::_))
        .WillOnce(Return(RETURN_ERR));

    ANSC_STATUS result = CosaDmlMtaBatteryGetTotalCapacity(nullptr, &totalCapacity);

    EXPECT_EQ(result, ANSC_STATUS_FAILURE);
}

//Test cases for CosaDmlMtaBatteryGetActualCapacity
TEST_F(CcspMtaTestFixture, CosaDmlMtaBatteryGetActualCapacity_Success) {

    ULONG actualCapacity = 0;

    EXPECT_CALL(*g_mtaHALMock, mta_hal_BatteryGetActualCapacity(_))
        .WillOnce(Return(RETURN_OK));

    ANSC_STATUS result = CosaDmlMtaBatteryGetActualCapacity(nullptr, &actualCapacity);

    EXPECT_EQ(result, ANSC_STATUS_SUCCESS);
}

TEST_F(CcspMtaTestFixture, CosaDmlMtaBatteryGetActualCapacity_Failure) {

    ULONG actualCapacity = 0;

    EXPECT_CALL(*g_mtaHALMock, mta_hal_BatteryGetActualCapacity(_))
        .WillOnce(Return(RETURN_ERR));

    ANSC_STATUS result = CosaDmlMtaBatteryGetActualCapacity(nullptr, &actualCapacity);

    EXPECT_EQ(result, ANSC_STATUS_FAILURE);
}

//Test cases for CosaDmlMtaBatteryGetRemainingCharge
TEST_F(CcspMtaTestFixture, CosaDmlMtaBatteryGetRemainingCharge_Success) {

    ULONG remainingCharge = 0;

    EXPECT_CALL(*g_mtaHALMock, mta_hal_BatteryGetRemainingCharge(::testing::_))
        .WillOnce(Return(RETURN_OK));

    ANSC_STATUS result = CosaDmlMtaBatteryGetRemainingCharge(nullptr, &remainingCharge);

    EXPECT_EQ(result, ANSC_STATUS_SUCCESS);
}

TEST_F(CcspMtaTestFixture, CosaDmlMtaBatteryGetRemainingCharge_Failure) {

    ULONG remainingCharge = 0;

    EXPECT_CALL(*g_mtaHALMock, mta_hal_BatteryGetRemainingCharge(_))
        .WillOnce(Return(RETURN_ERR));

    ANSC_STATUS result = CosaDmlMtaBatteryGetRemainingCharge(nullptr, &remainingCharge);

    EXPECT_EQ(result, ANSC_STATUS_FAILURE);
}

//Test cases for CosaDmlMtaBatteryGetRemainingTime
TEST_F(CcspMtaTestFixture, CosaDmlMtaBatteryGetRemainingTime_Success) {

    ULONG remainingTime = 0;

    EXPECT_CALL(*g_mtaHALMock, mta_hal_BatteryGetRemainingTime(_))
        .WillOnce(Return(RETURN_OK));

    ANSC_STATUS result = CosaDmlMtaBatteryGetRemainingTime(nullptr, &remainingTime);

    EXPECT_EQ(result, ANSC_STATUS_SUCCESS);
}

TEST_F(CcspMtaTestFixture, CosaDmlMtaBatteryGetRemainingTime_Failure) {

    ULONG remainingTime = 0;

    EXPECT_CALL(*g_mtaHALMock, mta_hal_BatteryGetRemainingTime(_))
        .WillOnce(Return(RETURN_ERR));

    ANSC_STATUS result = CosaDmlMtaBatteryGetRemainingTime(nullptr, &remainingTime);

    EXPECT_EQ(result, ANSC_STATUS_FAILURE);
}

//Test cases for CosaDmlMtaBatteryGetNumberofCycles
TEST_F(CcspMtaTestFixture, CosaDmlMtaBatteryGetNumberofCycles_Success) {

    ULONG numberOfCycles = 0;

    EXPECT_CALL(*g_mtaHALMock, mta_hal_BatteryGetNumberofCycles(_))
        .WillOnce(Return(RETURN_OK));

    ANSC_STATUS result = CosaDmlMtaBatteryGetNumberofCycles(nullptr, &numberOfCycles);

    EXPECT_EQ(result, ANSC_STATUS_SUCCESS);
}

TEST_F(CcspMtaTestFixture, CosaDmlMtaBatteryGetNumberofCycles_Failure) {

    ULONG numberOfCycles = 0;

    EXPECT_CALL(*g_mtaHALMock, mta_hal_BatteryGetNumberofCycles(_))
        .WillOnce(Return(RETURN_ERR));

    ANSC_STATUS result = CosaDmlMtaBatteryGetNumberofCycles(nullptr, &numberOfCycles);
    EXPECT_EQ(result, ANSC_STATUS_FAILURE);
}

//Test cases for CosaDmlMtaBatteryGetPowerStatus
TEST_F(CcspMtaTestFixture, CosaDmlMtaBatteryGetPowerStatus_Success) {

    char powerStatus[64] = {0};
    ULONG size = sizeof(powerStatus);

    EXPECT_CALL(*g_mtaHALMock, mta_hal_BatteryGetPowerStatus(_,_))
        .WillOnce(Return(RETURN_OK));

    ANSC_STATUS result = CosaDmlMtaBatteryGetPowerStatus(nullptr, powerStatus, &size);

    EXPECT_EQ(result, ANSC_STATUS_SUCCESS);
}

TEST_F(CcspMtaTestFixture, CosaDmlMtaBatteryGetPowerStatus_Failure) {

    char powerStatus[64] = {0};
    ULONG size = sizeof(powerStatus);

    EXPECT_CALL(*g_mtaHALMock, mta_hal_BatteryGetPowerStatus(_,_))
        .WillOnce(Return(RETURN_ERR));

    ANSC_STATUS result = CosaDmlMtaBatteryGetPowerStatus(nullptr, powerStatus, &size);
    EXPECT_EQ(result, ANSC_STATUS_FAILURE);
}

//Test cases for CosaDmlMtaBatteryGetCondition
TEST_F(CcspMtaTestFixture, CosaDmlMtaBatteryGetCondition_Success) {

    char condition[64] = {0};
    ULONG size = sizeof(condition);

    
    EXPECT_CALL(*g_mtaHALMock, mta_hal_BatteryGetCondition(_,_))
        .WillOnce(Return(RETURN_OK));

    ANSC_STATUS result = CosaDmlMtaBatteryGetCondition(nullptr, condition, &size);

    EXPECT_EQ(result, ANSC_STATUS_SUCCESS);
}

TEST_F(CcspMtaTestFixture, CosaDmlMtaBatteryGetCondition_Failure) {

    char condition[64] = {0};
    ULONG size = sizeof(condition);

    EXPECT_CALL(*g_mtaHALMock, mta_hal_BatteryGetCondition(_,_))
        .WillOnce(Return(RETURN_ERR));

    ANSC_STATUS result = CosaDmlMtaBatteryGetCondition(nullptr, condition, &size);

    EXPECT_EQ(result, ANSC_STATUS_FAILURE);
}

//Test cases for CosaDmlMtaBatteryGetStatus
TEST_F(CcspMtaTestFixture, CosaDmlMtaBatteryGetStatus_Success) {

    char status[64] = {0};
    ULONG size = sizeof(status);

    EXPECT_CALL(*g_mtaHALMock, mta_hal_BatteryGetStatus(_,_))
        .WillOnce(Return(RETURN_OK));

    ANSC_STATUS result = CosaDmlMtaBatteryGetStatus(nullptr, status, &size);

    EXPECT_EQ(result, ANSC_STATUS_SUCCESS);
}

TEST_F(CcspMtaTestFixture, CosaDmlMtaBatteryGetStatus_Failure) {

    char status[64] = {0};
    ULONG size = sizeof(status);

    EXPECT_CALL(*g_mtaHALMock, mta_hal_BatteryGetStatus(_,_))
        .WillOnce(Return(RETURN_ERR));

    ANSC_STATUS result = CosaDmlMtaBatteryGetStatus(nullptr, status, &size);

    EXPECT_EQ(result, ANSC_STATUS_FAILURE);
}

//Test cases for CosaDmlMtaBatteryGetLife
TEST_F(CcspMtaTestFixture, CosaDmlMtaBatteryGetLife_Success) {

    char batteryLife[64];
    ULONG size = sizeof(batteryLife);

    EXPECT_CALL(*g_mtaHALMock, mta_hal_BatteryGetLife(_,_))
        .WillOnce(Return(RETURN_OK));

  
    ANSC_STATUS result = CosaDmlMtaBatteryGetLife(nullptr, batteryLife, &size);

    EXPECT_EQ(result, ANSC_STATUS_SUCCESS);
}

TEST_F(CcspMtaTestFixture, CosaDmlMtaBatteryGetLife_Failure) {
    
    char batteryLife[64];
    ULONG size = sizeof(batteryLife);

    EXPECT_CALL(*g_mtaHALMock, mta_hal_BatteryGetLife(_,_))
        .WillOnce(Return(RETURN_ERR));

    ANSC_STATUS result = CosaDmlMtaBatteryGetLife(nullptr, batteryLife, &size);
    EXPECT_EQ(result, ANSC_STATUS_FAILURE);
}

//Test cases for CosaDmlMtaBatteryGetInfo
TEST_F(CcspMtaTestFixture, CosaDmlMtaBatteryGetInfo_Success) {
    
    PCOSA_DML_BATTERY_INFO pBatteryInfo = (PCOSA_DML_BATTERY_INFO)malloc(sizeof(COSA_DML_BATTERY_INFO));
    ASSERT_NE(pBatteryInfo, nullptr); 
    memset(pBatteryInfo, 0, sizeof(COSA_DML_BATTERY_INFO)); 

    EXPECT_CALL(*g_mtaHALMock, mta_hal_BatteryGetInfo(_))
        .WillOnce(Return(RETURN_OK));

    ANSC_STATUS result = CosaDmlMtaBatteryGetInfo(nullptr, pBatteryInfo);

    EXPECT_EQ(result, ANSC_STATUS_SUCCESS);
    free(pBatteryInfo);
}

TEST_F(CcspMtaTestFixture, CosaDmlMtaBatteryGetInfo_Failure) {
   
    PCOSA_DML_BATTERY_INFO pBatteryInfo = (PCOSA_DML_BATTERY_INFO)malloc(sizeof(COSA_DML_BATTERY_INFO));
    ASSERT_NE(pBatteryInfo, nullptr);  
    memset(pBatteryInfo, 0, sizeof(COSA_DML_BATTERY_INFO)); 

    EXPECT_CALL(*g_mtaHALMock, mta_hal_BatteryGetInfo(_))
        .WillOnce(Return(RETURN_ERR));

    ANSC_STATUS result = CosaDmlMtaBatteryGetInfo(nullptr, pBatteryInfo);

    EXPECT_EQ(result, ANSC_STATUS_FAILURE);
    free(pBatteryInfo);
}

//Test case for CosaDmlMtaGetDectLog
TEST_F(CcspMtaTestFixture, CosaDmlMtaGetDectLog_Success) {
    
    ULONG ulCount = 0;
    PCOSA_DML_DECTLOG_FULL ppConf = (PCOSA_DML_DECTLOG_FULL)malloc(sizeof(COSA_DML_DECTLOG_FULL));
    ASSERT_NE(ppConf, nullptr);  
    memset(ppConf, 0, sizeof(COSA_DML_DECTLOG_FULL));  

    ANSC_STATUS result = CosaDmlMtaGetDectLog(nullptr, &ulCount, &ppConf);

    EXPECT_EQ(result, ANSC_STATUS_SUCCESS);
    free(ppConf);
}

//Test cases for CosaDmlMtaGetResetCount
TEST_F(CcspMtaTestFixture, CosaDmlMtaGetResetCount_MTAReset) {
    
    ULONG resetCount = 0;
    EXPECT_CALL(*g_mtaHALMock, mta_hal_Get_MTAResetCount(_))
        .WillOnce(Return(RETURN_OK)); 

    ANSC_STATUS result = CosaDmlMtaGetResetCount(nullptr, MTA_RESET, &resetCount);

    EXPECT_EQ(result, ANSC_STATUS_SUCCESS);
}

TEST_F(CcspMtaTestFixture, CosaDmlMtaGetResetCount_LineReset) {

    ULONG resetCount = 0;
    EXPECT_CALL(*g_mtaHALMock, mta_hal_Get_LineResetCount(_))
        .WillOnce(Return(RETURN_OK));

    ANSC_STATUS result = CosaDmlMtaGetResetCount(nullptr, LINE_RESET, &resetCount);

    EXPECT_EQ(result, ANSC_STATUS_SUCCESS);
}

TEST_F(CcspMtaTestFixture, CosaDmlMtaGetResetCount_InvalidType) {

    ULONG resetCount = 0;

    ANSC_STATUS result = CosaDmlMtaGetResetCount(nullptr, static_cast<MTA_RESET_TYPE>(999), &resetCount);

    EXPECT_EQ(result, ANSC_STATUS_SUCCESS);
}

//Test case for CosaDmlMtaClearCalls
TEST_F(CcspMtaTestFixture, CosaDmlMtaClearCalls_Success) {
    
    ULONG instanceNumber = 1; // Set a test instance number

    EXPECT_CALL(*g_mtaHALMock, mta_hal_ClearCalls(instanceNumber))
        .WillOnce(Return(RETURN_OK));

    ANSC_STATUS result = CosaDmlMtaClearCalls(instanceNumber);
    EXPECT_EQ(result, ANSC_STATUS_SUCCESS);
}

//Test case for CosaDmlMTASetStartUpIpMode
TEST_F(CcspMtaTestFixture, CosaDmlMTASetStartUpIpMode_Success) {

    
    PCOSA_MTA_ETHWAN_PROV_INFO pmtaethpro = (PCOSA_MTA_ETHWAN_PROV_INFO)malloc(sizeof(COSA_MTA_ETHWAN_PROV_INFO));
    ASSERT_NE(pmtaethpro, nullptr);
    memset(pmtaethpro, 0, sizeof(COSA_MTA_ETHWAN_PROV_INFO)); 

    INT bInt = 1; // Set a test IP mode value

    ANSC_STATUS result = CosaDmlMTASetStartUpIpMode(pmtaethpro, bInt);

    EXPECT_EQ(result, ANSC_STATUS_SUCCESS); 
    EXPECT_EQ(pmtaethpro->StartupIPMode.ActiveValue, bInt);

    free(pmtaethpro);
}

//Test case for CosaDmlMTASetPrimaryDhcpServerOptions
TEST_F(CcspMtaTestFixture, CosaDmlMTASetPrimaryDhcpServerOptions_Success) {
    
    PCOSA_MTA_ETHWAN_PROV_INFO pmtaethpro = (PCOSA_MTA_ETHWAN_PROV_INFO)malloc(sizeof(COSA_MTA_ETHWAN_PROV_INFO));
    ASSERT_NE(pmtaethpro, nullptr);
    memset(pmtaethpro, 0, sizeof(COSA_MTA_ETHWAN_PROV_INFO));

    // Test with IPv4 type
    char testBuf[] = "192.168.1.1"; // Test buffer value for IPv4
    MTA_IP_TYPE_TR testType = MTA_IPV4_TR; // Test type for IPv4

    ANSC_STATUS result = CosaDmlMTASetPrimaryDhcpServerOptions(pmtaethpro, testBuf, testType);

    EXPECT_EQ(result, ANSC_STATUS_SUCCESS); 
    EXPECT_STREQ(pmtaethpro->IPv4PrimaryDhcpServerOptions.ActiveValue, testBuf); // Verify the IPv4 option is set correctly

    // Test with IPv6 type
    char testBufIPv6[] = "2001:0db8:0000:0042:0000:8a2e:0370:7334"; // Test buffer value for IPv6
    testType = MTA_IPV6_TR; // Test type for IPv6

    result = CosaDmlMTASetPrimaryDhcpServerOptions(pmtaethpro, testBufIPv6, testType);

    EXPECT_EQ(result, ANSC_STATUS_SUCCESS);
    EXPECT_STREQ(pmtaethpro->IPv6PrimaryDhcpServerOptions.ActiveValue, testBufIPv6); // Verify the IPv6 option is set correctly

    free(pmtaethpro);
}

//Test case for CosaDmlMTASetSecondaryDhcpServerOptions
TEST_F(CcspMtaTestFixture, CosaDmlMTASetSecondaryDhcpServerOptions_Success) {
    
    PCOSA_MTA_ETHWAN_PROV_INFO pmtaethpro = (PCOSA_MTA_ETHWAN_PROV_INFO)malloc(sizeof(COSA_MTA_ETHWAN_PROV_INFO));
    ASSERT_NE(pmtaethpro, nullptr);
    memset(pmtaethpro, 0, sizeof(COSA_MTA_ETHWAN_PROV_INFO));

    // Test with IPv4 type
    char testBuf[] = "192.168.1.1"; // Test buffer value for IPv4
    MTA_IP_TYPE_TR testType = MTA_IPV4_TR; // Test type for IPv4

    ANSC_STATUS result = CosaDmlMTASetSecondaryDhcpServerOptions(pmtaethpro, testBuf, testType);

    EXPECT_EQ(result, ANSC_STATUS_SUCCESS); 
    EXPECT_STREQ(pmtaethpro->IPv4SecondaryDhcpServerOptions.ActiveValue, testBuf); // Verify the IPv4 option is set correctly

    // Test with IPv6 type
    char testBufIPv6[] = "2001:0db8:0000:0042:0000:8a2e:0370:7334"; // Test buffer value for IPv6
    testType = MTA_IPV6_TR; // Test type for IPv6

    result = CosaDmlMTASetSecondaryDhcpServerOptions(pmtaethpro, testBufIPv6, testType);

    EXPECT_EQ(result, ANSC_STATUS_SUCCESS);
    EXPECT_STREQ(pmtaethpro->IPv6SecondaryDhcpServerOptions.ActiveValue, testBufIPv6); // Verify the IPv6 option is set correctly

    free(pmtaethpro);
}

//Test case for CosaDmlMTAGetServiceFlow
TEST_F(CcspMtaTestFixture, Success_MtaHalGetServiceFlowSuccess) {

    ULONG count = 1;
    PCOSA_MTA_SERVICE_FLOW pCfg = (PCOSA_MTA_SERVICE_FLOW)malloc(sizeof(COSA_MTA_SERVICE_FLOW));

    ASSERT_NE(pCfg, nullptr);
    memset(pCfg, 0, sizeof(COSA_MTA_SERVICE_FLOW));

    EXPECT_CALL(*g_mtaHALMock, mta_hal_GetServiceFlow(_, _))
        .WillOnce(Return(RETURN_OK));

    EXPECT_EQ(CosaDmlMTAGetServiceFlow(nullptr, &count, &pCfg), ANSC_STATUS_SUCCESS);
    EXPECT_NE(pCfg, nullptr);

    free(pCfg);
}

TEST_F(CcspMtaTestFixture, Success_MtaHalGetHandsetsReturnsOk_PulCountZeroOrLess) {
    ULONG count = 0;
    PCOSA_MTA_HANDSETS_INFO pHandsets = nullptr;

    // Simulate that mta_hal_GetHandsets returns OK and sets pulCount to 0
    EXPECT_CALL(*g_mtaHALMock, mta_hal_GetHandsets(_, _))
        .WillOnce(DoAll(
            SetArgPointee<0>(count),
            Return(RETURN_OK)
        ));

    EXPECT_EQ(CosaDmlMTAGetHandsets(nullptr, &count, &pHandsets), ANSC_STATUS_SUCCESS);
    EXPECT_EQ(count, 0);
    EXPECT_EQ(pHandsets, nullptr);
}


TEST_F(CcspMtaTestFixture, Failure_MtaHalGetHandsetsReturnsError) {
    ULONG count = 0;
    PCOSA_MTA_HANDSETS_INFO pHandsets = nullptr;

    EXPECT_CALL(*g_mtaHALMock, mta_hal_GetHandsets(_, _))
        .WillOnce(Return(RETURN_ERR)); 

    EXPECT_EQ(CosaDmlMTAGetHandsets(nullptr, &count, &pHandsets), ANSC_STATUS_FAILURE);
    EXPECT_EQ(count, 0);
    EXPECT_EQ(pHandsets, nullptr);
}

//Test cases for CosaDmlMTAGetCalls
TEST_F(CcspMtaTestFixture, Success_MtaHalGetCallsReturnsOk_PulCountZero) {
    ULONG count = 0;
    PCOSA_MTA_CALLS pCalls = nullptr;

    EXPECT_CALL(*g_mtaHALMock, mta_hal_GetCalls(_, _, _))
        .WillOnce(DoAll(
            SetArgPointee<1>(count),
            Return(RETURN_OK)
        ));

    EXPECT_EQ(CosaDmlMTAGetCalls(nullptr, 1, &count, &pCalls), ANSC_STATUS_SUCCESS);
    EXPECT_EQ(count, 0);
    EXPECT_EQ(pCalls, nullptr);
}

TEST_F(CcspMtaTestFixture, Success_MtaHalGetCallsReturnsOk_PulCountGreaterThanZero) {
    ULONG count = 2;
    PCOSA_MTA_CALLS pCalls = nullptr;
    PMTAMGMT_MTA_CALLS pInfo = (PMTAMGMT_MTA_CALLS)malloc(sizeof(MTAMGMT_MTA_CALLS) * count);

    ASSERT_NE(pInfo, nullptr); 

    // Initialize pInfo with some data (optional)
    memset(pInfo, 0, sizeof(MTAMGMT_MTA_CALLS) * count);

    EXPECT_CALL(*g_mtaHALMock, mta_hal_GetCalls(_, _, _))
        .WillOnce(DoAll(
            SetArgPointee<1>(count), // Set pulCount to 2
            SetArgPointee<2>(pInfo), // Set pInfo to the mock data
            Return(RETURN_OK)
        ));

    EXPECT_EQ(CosaDmlMTAGetCalls(nullptr, 1, &count, &pCalls), ANSC_STATUS_SUCCESS);
    EXPECT_EQ(count, 2);
    ASSERT_NE(pCalls, nullptr);
}

TEST_F(CcspMtaTestFixture, Failure_MtaHalGetCallsReturnsError) {

    ULONG count = 0;
    PCOSA_MTA_CALLS pCalls = nullptr;
    EXPECT_CALL(*g_mtaHALMock, mta_hal_GetCalls(_, _, _))
        .WillOnce(Return(RETURN_ERR)); 

    EXPECT_EQ(CosaDmlMTAGetCalls(nullptr, 1, &count, &pCalls), ANSC_STATUS_FAILURE);
    EXPECT_EQ(count, 0);
    EXPECT_EQ(pCalls, nullptr);
}

//Test cases forCosaDmlMTAGetDSXLogs
TEST_F(CcspMtaTestFixture, Success_MtaHalGetDSXLogsReturnsOk_PulCountZero) {
    ULONG count = 0;
    PCOSA_MTA_DSXLOG pDSXLog = nullptr;

    // Simulate that mta_hal_GetDSXLogs returns OK but pulCount is set to 0
    EXPECT_CALL(*g_mtaHALMock, mta_hal_GetDSXLogs(_, _))
        .WillOnce(DoAll(
            SetArgPointee<0>(count),
            Return(RETURN_OK)
        ));

    EXPECT_EQ(CosaDmlMTAGetDSXLogs(nullptr, &count, &pDSXLog), ANSC_STATUS_SUCCESS);
    EXPECT_EQ(count, 0);
    EXPECT_EQ(pDSXLog, nullptr);
}

TEST_F(CcspMtaTestFixture, Success_MtaHalGetDSXLogsReturnsOk_PulCountGreaterThanZero) {
    ULONG count = 2;
    PCOSA_MTA_DSXLOG pDSXLog = nullptr;
    PMTAMGMT_MTA_DSXLOG pInfo = (PMTAMGMT_MTA_DSXLOG)malloc(sizeof(MTAMGMT_MTA_DSXLOG) * count);

    ASSERT_NE(pInfo, nullptr); 

    memset(pInfo, 0, sizeof(MTAMGMT_MTA_DSXLOG) * count);

    // Simulate that mta_hal_GetDSXLogs returns OK and sets pulCount to 2
    EXPECT_CALL(*g_mtaHALMock, mta_hal_GetDSXLogs(_, _))
        .WillOnce(DoAll(
            SetArgPointee<0>(count), // Set pulCount to 2
            SetArgPointee<1>(pInfo), // Set pInfo to the mock data
            Return(RETURN_OK)
        ));

    EXPECT_EQ(CosaDmlMTAGetDSXLogs(nullptr, &count, &pDSXLog), ANSC_STATUS_SUCCESS);
    EXPECT_EQ(count, 2);
    ASSERT_NE(pDSXLog, nullptr); 

}

TEST_F(CcspMtaTestFixture, Success_MtaHalGetDSXLogsReturnsError) {
    ULONG count = 0;
    PCOSA_MTA_DSXLOG pDSXLog = nullptr;

    EXPECT_CALL(*g_mtaHALMock, mta_hal_GetDSXLogs(_, _))
        .WillOnce(Return(RETURN_ERR)); 

    EXPECT_EQ(CosaDmlMTAGetDSXLogs(nullptr, &count, &pDSXLog), ANSC_STATUS_SUCCESS);
    EXPECT_EQ(count, 0);
    EXPECT_EQ(pDSXLog, nullptr);
}

TEST_F(CcspMtaTestFixture, Failure_MtaHalGetDSXLogsReturnsError) {
    ULONG count = 0;
    PCOSA_MTA_DSXLOG pDSXLog = nullptr;
    PMTAMGMT_MTA_DSXLOG pInfo = (PMTAMGMT_MTA_DSXLOG)malloc(sizeof(MTAMGMT_MTA_DSXLOG)); // Simulate non-null pInfo

    ASSERT_NE(pInfo, nullptr);

    // Simulate that mta_hal_GetDSXLogs returns an error and sets pInfo to a non-null value
    EXPECT_CALL(*g_mtaHALMock, mta_hal_GetDSXLogs(_, _))
        .WillOnce(DoAll(
            SetArgPointee<1>(pInfo), // Set pInfo to non-null
            Return(RETURN_ERR)       // Simulate error
        ));

    EXPECT_EQ(CosaDmlMTAGetDSXLogs(nullptr, &count, &pDSXLog), ANSC_STATUS_FAILURE);
    EXPECT_EQ(count, 0);
    EXPECT_EQ(pDSXLog, nullptr);
}

// Test cases for CosaDmlMTAClearCallSignallingLog
TEST_F(CcspMtaTestFixture, Success_MtaHalClearCallSignallingLog) {
    BOOLEAN clearLog = true;

    EXPECT_CALL(*g_mtaHALMock, mta_hal_ClearCallSignallingLog(clearLog))
        .WillOnce(Return(RETURN_OK));

    EXPECT_EQ(CosaDmlMTAClearCallSignallingLog(nullptr, clearLog), ANSC_STATUS_SUCCESS);
}

TEST_F(CcspMtaTestFixture, Failure_MtaHalClearCallSignallingLog) {
    BOOLEAN clearLog = true;

    EXPECT_CALL(*g_mtaHALMock, mta_hal_ClearCallSignallingLog(clearLog))
        .WillOnce(Return(RETURN_ERR));

    EXPECT_EQ(CosaDmlMTAClearCallSignallingLog(nullptr, clearLog), ANSC_STATUS_FAILURE);
}

//Test cases for CosaDmlMtaGetMtaLog
TEST_F(CcspMtaTestFixture, Success_MtaHalGetMtaLogReturnsOk_PulCountZero) {
    ULONG count = 0;
    PCOSA_DML_MTALOG_FULL pConf = nullptr;

    // Simulate that mta_hal_GetMtaLog returns OK but pulCount is set to 0
    EXPECT_CALL(*g_mtaHALMock, mta_hal_GetMtaLog(_, _))
        .WillOnce(DoAll(
            SetArgPointee<0>(count), // Set pulCount to 0
            Return(RETURN_OK)
        ));

    EXPECT_EQ(CosaDmlMtaGetMtaLog(nullptr, &count, &pConf), ANSC_STATUS_SUCCESS);
    EXPECT_EQ(count, 0);
    EXPECT_EQ(pConf, nullptr);
}


TEST_F(CcspMtaTestFixture, Failure_MtaHalGetMtaLogReturnsError_WithNonNullPInfo) {
    ULONG count = 0;
    PCOSA_DML_MTALOG_FULL pConf = nullptr;
    PMTAMGMT_MTA_MTALOG_FULL pInfo = (PMTAMGMT_MTA_MTALOG_FULL)malloc(sizeof(MTAMGMT_MTA_MTALOG_FULL));

    ASSERT_NE(pInfo, nullptr);
    memset(pInfo, 0, sizeof(MTAMGMT_MTA_MTALOG_FULL));

    // Simulate that mta_hal_GetMtaLog returns an error but pInfo is non-null
    EXPECT_CALL(*g_mtaHALMock, mta_hal_GetMtaLog(_, _))
        .WillOnce(DoAll(
            SetArgPointee<1>(pInfo), // Set pInfo to a valid non-null pointer
            Return(RETURN_ERR) 
        ));

    EXPECT_EQ(CosaDmlMtaGetMtaLog(nullptr, &count, &pConf), ANSC_STATUS_FAILURE);
    EXPECT_EQ(count, 0);
    EXPECT_EQ(pConf, nullptr);
}

TEST_F(CcspMtaTestFixture, Success_MtaHalGetMtaLogReturnsOk_PulCountGreaterThanZero) {
    ULONG count = 2; // Expecting 2 logs
    PCOSA_DML_MTALOG_FULL pConf = nullptr;

    // Simulate that mta_hal_GetMtaLog returns OK and sets pulCount to 2
    PMTAMGMT_MTA_MTALOG_FULL pInfo = (PMTAMGMT_MTA_MTALOG_FULL)malloc(sizeof(MTAMGMT_MTA_MTALOG_FULL) * count);
    ASSERT_NE(pInfo, nullptr);

      for (unsigned int i = 0; i < count; i++) {
        pInfo[i].pDescription = (char *)malloc(sizeof(char) * (strlen("Test Description") + 1)); 
        ASSERT_NE(pInfo[i].pDescription, nullptr); 
        strcpy(pInfo[i].pDescription, "Test Description");
    }


    EXPECT_CALL(*g_mtaHALMock, mta_hal_GetMtaLog(_, _))
        .WillOnce(DoAll(
            SetArgPointee<0>(count),   // Set pulCount to 2
            SetArgPointee<1>(pInfo),   // Set pInfo to the mock data
            Return(RETURN_OK)
        ));

   EXPECT_CALL(*g_anscWrapperApiMock, AnscCloneString(StrEq("Test Description")))
    .WillRepeatedly(DoAll(Return(strdup("Test Description")) ));


    EXPECT_EQ(CosaDmlMtaGetMtaLog(nullptr, &count, &pConf), ANSC_STATUS_SUCCESS);
    EXPECT_EQ(count, 2);
    ASSERT_NE(pConf, nullptr);

    // Validate the results
    for (unsigned int i = 0; i < count; i++) {
        EXPECT_NE(pConf[i].pDescription, nullptr);
        EXPECT_STREQ(pConf[i].pDescription, "Test Description");
    }
}

// Test case for CosaDmlMtaGetDhcpStatus with mock for mta_hal_getDhcpStatus
TEST_F(CcspMtaTestFixture, GetDhcpStatus_Success_Test)
{
    MTAMGMT_MTA_STATUS ipv4MockStatus = MTA_COMPLETE; 
    MTAMGMT_MTA_STATUS ipv6MockStatus = MTA_COMPLETE;

#ifdef _CBR_PRODUCT_REQ_
    EXPECT_CALL(*g_mtaHALMock, mta_hal_getDhcpStatus(_, _))
        .WillOnce(DoAll(SetArgPointee<0>(ipv4MockStatus), SetArgPointee<1>(ipv6MockStatus), Return(RETURN_OK)));
#endif

    ULONG ipv4Status, ipv6Status;
    ANSC_STATUS result = CosaDmlMtaGetDhcpStatus(&ipv4Status, &ipv6Status);

    EXPECT_EQ(result, ANSC_STATUS_SUCCESS);
#ifdef _CBR_PRODUCT_REQ_
    EXPECT_EQ(ipv4Status, ipv4MockStatus);
    EXPECT_EQ(ipv6Status, ipv6MockStatus);
#else
    EXPECT_EQ(ipv4Status, MTA_ERROR);
    EXPECT_EQ(ipv6Status, MTA_ERROR);
#endif
}

// Test case for CosaDmlMtaGetConfigFileStatus
TEST_F(CcspMtaTestFixture, GetConfigFileStatus_Success_Test)
{
    MTAMGMT_MTA_STATUS mockStatus = MTA_COMPLETE;  

#ifdef _CBR_PRODUCT_REQ_
    EXPECT_CALL(*g_mtaHALMock, mta_hal_getConfigFileStatus(_))
        .WillOnce(DoAll(SetArgPointee<0>(mockStatus), Return(RETURN_OK)));
#endif

    ULONG configFileStatus;
    ANSC_STATUS result = CosaDmlMtaGetConfigFileStatus(&configFileStatus);

    EXPECT_EQ(result, ANSC_STATUS_SUCCESS);
#ifdef _CBR_PRODUCT_REQ_
    EXPECT_EQ(configFileStatus, mockStatus);
#endif
}

// Test case for CosaDmlMtaGetLineRegisterStatus
TEST_F(CcspMtaTestFixture, GetLineRegisterStatus_Success_Test)
{
    char lineRegisterStatus[256] = {0};
    MTAMGMT_MTA_STATUS mockStatusArray[8] = {MTA_INIT, MTA_START, MTA_COMPLETE, MTA_ERROR, MTA_START, MTA_COMPLETE, MTA_INIT, MTA_INIT}; // Use enum values

#ifdef _CBR_PRODUCT_REQ_
    EXPECT_CALL(*g_mtaHALMock, mta_hal_getLineRegisterStatus(_, _))
        .WillOnce(DoAll(SetArrayArgument<0>(mockStatusArray, mockStatusArray + 8), Return(RETURN_OK)));
#endif

    ANSC_STATUS result = CosaDmlMtaGetLineRegisterStatus(lineRegisterStatus);

    EXPECT_EQ(result, ANSC_STATUS_SUCCESS);
#ifdef _CBR_PRODUCT_REQ_
    EXPECT_STREQ(lineRegisterStatus, "Init,Start,Complete,Error,Start,Complete,Init,Init");
#else
    EXPECT_STREQ(lineRegisterStatus, "");
#endif
}

// Test cases for CosaDmlMtaResetNow 
TEST_F(CcspMtaTestFixture, ResetNow_Success_Test)
{
    BOOLEAN bValue = TRUE; 

#if defined(_CBR_PRODUCT_REQ_) || defined(_XB6_PRODUCT_REQ_)
    EXPECT_CALL(*g_mtaHALMock, mta_hal_devResetNow(bValue))
        .WillOnce(Return(RETURN_OK));
#endif

    ANSC_STATUS result = CosaDmlMtaResetNow(bValue);

    EXPECT_EQ(result, ANSC_STATUS_SUCCESS);
}

TEST_F(CcspMtaTestFixture, ResetNow_Failure_Test)
{
    BOOLEAN bValue = TRUE; 

#if defined(_CBR_PRODUCT_REQ_) || defined(_XB6_PRODUCT_REQ_)
    EXPECT_CALL(*g_mtaHALMock, mta_hal_devResetNow(bValue))
        .WillOnce(Return(RETURN_ERR)); 

    ANSC_STATUS result = CosaDmlMtaResetNow(bValue);
    EXPECT_EQ(result, ANSC_STATUS_FAILURE); 
#else
   
    ANSC_STATUS result = CosaDmlMtaResetNow(bValue);
    EXPECT_EQ(result, ANSC_STATUS_SUCCESS); // Expect success when flags are not defined
#endif
}

//Test cases for fillCurrentPartnerId
TEST_F(CcspMtaTestFixture, FillCurrentPartnerId_Success_ValidPartnerID)
{
    char pValue[64] = {'\0'};
    ULONG pulSize = 0;

    EXPECT_CALL(*g_syscfgMock, syscfg_get(_, "PartnerID", _, 64))
        .WillOnce(DoAll(::testing::SetArrayArgument<2>("Partner123", "Partner123" + 10), 
                        Return(ANSC_STATUS_SUCCESS)));

    ANSC_STATUS result = fillCurrentPartnerId(pValue, &pulSize);

    EXPECT_EQ(result, ANSC_STATUS_SUCCESS);
    EXPECT_STREQ(pValue, "Partner123");  
}

TEST_F(CcspMtaTestFixture, FillCurrentPartnerId_Failure_EmptyBuffer)
{
    char pValue[64] = {'\0'};
    ULONG pulSize = 0;

    EXPECT_CALL(*g_syscfgMock, syscfg_get(_, "PartnerID", _, 64))
        .WillOnce(DoAll(::testing::SetArrayArgument<2>("", "" + 1), 
                        Return(ANSC_STATUS_SUCCESS)));

    ANSC_STATUS result = fillCurrentPartnerId(pValue, &pulSize);

    EXPECT_EQ(result, ANSC_STATUS_FAILURE);
}

TEST_F(CcspMtaTestFixture, FillCurrentPartnerId_Failure_SyscfgGetFailure)
{
    char pValue[64] = {'\0'};
    ULONG pulSize = 0;

    EXPECT_CALL(*g_syscfgMock, syscfg_get(_, "PartnerID", _, 64))
        .WillOnce(Return(ANSC_STATUS_FAILURE));

    ANSC_STATUS result = fillCurrentPartnerId(pValue, &pulSize);

    EXPECT_EQ(result, ANSC_STATUS_FAILURE);
}