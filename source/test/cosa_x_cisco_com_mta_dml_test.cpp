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
extern "C" 
{
    #include "cosa_x_cisco_com_mta_dml.h"
    #include "cosa_x_cisco_com_mta_internal.h"

}

extern MtaHalMock* g_mtaHALMock;
extern SafecLibMock* g_safecLibMock;
extern UserTimeMock* g_usertimeMock;

using ::testing::_;
using ::testing::Return;
using ::testing::DoAll;
using ::testing::SetArgPointee;
using ::testing::Return;
using ::testing::StrEq;


//Test case for X_CISCO_COM_MTA_V6_Validate
TEST_F(CcspMtaTestFixture, X_CISCO_COM_MTA_V6_Validate_ReturnsTrue)
{
    ANSC_HANDLE hInsContext = nullptr;
    char pReturnParamName[256] = {0}; 
    ULONG puLength = 0;
    BOOL result = X_CISCO_COM_MTA_V6_Validate(hInsContext, pReturnParamName, &puLength);
    EXPECT_TRUE(result); 
}

//Test case for X_CISCO_COM_MTA_V6_Commit
TEST_F(CcspMtaTestFixture, X_CISCO_COM_MTA_V6_Commit_ReturnsZero)
{

    ANSC_HANDLE hInsContext = nullptr;
    ULONG result = X_CISCO_COM_MTA_V6_Commit(hInsContext);
    EXPECT_EQ(result, 0);
}

//X_CISCO_COM_MTA_V6_Rollback
TEST_F(CcspMtaTestFixture, X_CISCO_COM_MTA_V6_Rollback_ReturnsZero)
{
    
    ANSC_HANDLE hInsContext = nullptr;
    ULONG result = X_CISCO_COM_MTA_V6_Rollback(hInsContext);
    EXPECT_EQ(result, 0);
}

//X_CISCO_COM_MTA_GetParamIntValue
TEST_F(CcspMtaTestFixture, X_CISCO_COM_MTA_GetParamIntValue_ReturnsFalse)
{
    
    ANSC_HANDLE hInsContext = nullptr;
    char ParamName[256] = "TestParam"; 
    int pInt = 0;
    BOOL result = X_CISCO_COM_MTA_GetParamIntValue(hInsContext, ParamName, &pInt);
    EXPECT_FALSE(result);
}

//X_CISCO_COM_MTA_SetParamIntValue
TEST_F(CcspMtaTestFixture, X_CISCO_COM_MTA_SetParamIntValue_ReturnsFalse)
{

    ANSC_HANDLE hInsContext = nullptr;
    char ParamName[256] = "TestParam"; 
    int iValue = 42;  
    BOOL result = X_CISCO_COM_MTA_SetParamIntValue(hInsContext, ParamName, iValue);
    EXPECT_FALSE(result);
}

//X_CISCO_COM_MTA_SetParamStringValue
TEST_F(CcspMtaTestFixture, X_CISCO_COM_MTA_SetParamStringValue_ReturnsFalse)
{

    ANSC_HANDLE hInsContext = nullptr;
    char ParamName[256] = "TestParam";
    char pString[256] = "TestValue";
    BOOL result = X_CISCO_COM_MTA_SetParamStringValue(hInsContext, ParamName, pString);
    EXPECT_FALSE(result);
}

//X_CISCO_COM_MTA_Validate
TEST_F(CcspMtaTestFixture, X_CISCO_COM_MTA_Validate_ReturnsTrue)
{
    
    ANSC_HANDLE hInsContext = nullptr;
    char pReturnParamName[256] = {0};
    ULONG puLength = 0;
    BOOL result = X_CISCO_COM_MTA_Validate(hInsContext, pReturnParamName, &puLength);
    EXPECT_TRUE(result);
}


//LineTable_GetParamIntValue
TEST_F(CcspMtaTestFixture, LineTable_GetParamIntValue_ReturnsFalse)
{

    ANSC_HANDLE hInsContext = nullptr;
    char ParamName[256] = "TestParam";
    int pInt = 0;
    BOOL result = LineTable_GetParamIntValue(hInsContext, ParamName, &pInt);
    EXPECT_FALSE(result);
}


//LineTable_SetParamIntValue
TEST_F(CcspMtaTestFixture, LineTable_SetParamIntValue_ReturnsFalse)
{
    ANSC_HANDLE hInsContext = nullptr;
    char ParamName[256] = "TestParam"; 
    int iValue = 42;
    BOOL result = LineTable_SetParamIntValue(hInsContext, ParamName, iValue);
    EXPECT_FALSE(result);
}

//LineTable_SetParamUlongValue
TEST_F(CcspMtaTestFixture, LineTable_SetParamUlongValue_ReturnsFalse)
{
    // Initialize the parameters used in the test
    ANSC_HANDLE hInsContext = nullptr;
    char ParamName[256] = "TestParam";
    ULONG uValue = 42; 
    BOOL result = LineTable_SetParamUlongValue(hInsContext, ParamName, uValue);
    EXPECT_FALSE(result);
}

//LineTable_SetParamStringValue
TEST_F(CcspMtaTestFixture, LineTable_SetParamStringValue_ReturnsFalse)
{
 
    ANSC_HANDLE hInsContext = nullptr;
    char ParamName[256] = "TestParam";
    char pString[256] = "TestString";
    BOOL result = LineTable_SetParamStringValue(hInsContext, ParamName, pString);
    EXPECT_FALSE(result);
}

//LineTable_Validate
TEST_F(CcspMtaTestFixture, LineTable_Validate_ReturnsTrue)
{
    
    ANSC_HANDLE hInsContext = nullptr;
    char pReturnParamName[256] = {0}; 
    ULONG puLength = 0;
    BOOL result = LineTable_Validate(hInsContext, pReturnParamName, &puLength);
    EXPECT_TRUE(result);
}
//LineTable_Commit
TEST_F(CcspMtaTestFixture, LineTable_CommitReturnsZero)
{ 
    ANSC_HANDLE hInsContext = nullptr;
    ULONG result = LineTable_Commit(hInsContext);
    EXPECT_EQ(result, 0);
}

//LineTable_Rollback
TEST_F(CcspMtaTestFixture, LineTable_Rollback_ReturnsZero)
{
    ANSC_HANDLE hInsContext = nullptr;
    ULONG result = LineTable_Rollback(hInsContext);
    EXPECT_EQ(result, 0);
}

//VQM_Validate
TEST_F(CcspMtaTestFixture, VQM_Validate_ReturnsTrue)
{
    ANSC_HANDLE hInsContext = nullptr;    
    char pReturnParamName[256] = {0};      
    ULONG puLength = 0;      
    BOOL result = VQM_Validate(hInsContext, pReturnParamName, &puLength);
    EXPECT_TRUE(result);
}

//VQM_Commit
TEST_F(CcspMtaTestFixture,VQM_Commit_ReturnsZero)
{
    ANSC_HANDLE hInsContext = nullptr;
    ULONG result = VQM_Commit(hInsContext);
    EXPECT_EQ(result, 0);
}

//VQM_Rollback
TEST_F(CcspMtaTestFixture, VQM_Rollback_ReturnsZero)
{
    ANSC_HANDLE hInsContext = nullptr;
    ULONG result = VQM_Rollback(hInsContext);
    EXPECT_EQ(result, 0);
}

//Dect_GetParamIntValue
TEST_F(CcspMtaTestFixture, Dect_GetParamIntValue_ReturnsFalse)
{
    ANSC_HANDLE hInsContext = nullptr;
    char ParamName[] = "TestParam";
    int value = 0;
    BOOL result = Dect_GetParamIntValue(hInsContext, ParamName, &value);
    EXPECT_FALSE(result);
}

//Dect_Validate
TEST_F(CcspMtaTestFixture, Dect_Validate_ReturnsTrue)
{
    ANSC_HANDLE hInsContext = nullptr;
    char* pReturnParamName = nullptr;
    ULONG puLength = 0;
    BOOL result = Dect_Validate(hInsContext, pReturnParamName, &puLength);
    EXPECT_TRUE(result);
}

//Dect_Commit
TEST_F(CcspMtaTestFixture, Dect_Commit_ReturnsZero)
{
    ANSC_HANDLE hInsContext = nullptr;
    ULONG result = Dect_Commit(hInsContext);
    EXPECT_EQ(result, 0);
}

//Dect_Rollback
TEST_F(CcspMtaTestFixture, Dect_Rollback_ReturnsZero)
{
    ANSC_HANDLE hInsContext = nullptr;
    ULONG result = Dect_Rollback(hInsContext);
    EXPECT_EQ(result, 0);
}

//Handsets_GetParamIntValue
TEST_F(CcspMtaTestFixture, Handsets_GetParamIntValue_ReturnsFalse) {
    ANSC_HANDLE hInsContext = nullptr;
    char ParamName[] = "TestParam";
    int value = 0;
    BOOL result = Handsets_GetParamIntValue(hInsContext, ParamName, &value);
    EXPECT_FALSE(result);
}

//Handsets_GetParamUlongValue
TEST_F(CcspMtaTestFixture, HandsetsGetParamUlongValue_ReturnsFalse) {
    ANSC_HANDLE hInsContext = nullptr;
    char ParamName[] = "TestParam";
    ULONG value = 0;
    BOOL result = Handsets_GetParamUlongValue(hInsContext, ParamName, &value);
    EXPECT_FALSE(result);
}

//Handsets_SetParamBoolValue
TEST_F(CcspMtaTestFixture, Handsets_SetParamBoolValue_ReturnsFalse) {
    ANSC_HANDLE hInsContext = nullptr;
    char ParamName[] = "TestParam";
    BOOL bValue = TRUE;
    BOOL result = Handsets_SetParamBoolValue(hInsContext, ParamName, bValue);
    EXPECT_FALSE(result);
}

//Handsets_SetParamIntValue
TEST_F(CcspMtaTestFixture, Handsets_SetParamIntValue_ReturnsFalse) {
    ANSC_HANDLE hInsContext = nullptr;
    char ParamName[] = "TestParam";
    int iValue = 42;
    BOOL result = Handsets_SetParamIntValue(hInsContext, ParamName, iValue);
    EXPECT_FALSE(result);
}

//Handsets_SetParamUlongValue
TEST_F(CcspMtaTestFixture, Handsets_SetParamUlongValue_ReturnsFalse) {
    ANSC_HANDLE hInsContext = nullptr;
    char ParamName[] = "TestParam";
    ULONG uValue = 100;
    BOOL result = Handsets_SetParamUlongValue(hInsContext, ParamName, uValue);
    EXPECT_FALSE(result);
}

//Handsets_Validate
TEST_F(CcspMtaTestFixture, Handsets_Validate_ReturnsTrue) {
    ANSC_HANDLE hInsContext = nullptr;
    char pReturnParamName[256] = {0};
    ULONG puLength = 0;
    BOOL result = Handsets_Validate(hInsContext, pReturnParamName, &puLength);
    EXPECT_TRUE(result);
}

//Handsets_Rollback
TEST_F(CcspMtaTestFixture, Handsets_Rollback_ReturnsZero) {
    ANSC_HANDLE hInsContext = nullptr;
    ULONG result = Handsets_Rollback(hInsContext);
    EXPECT_EQ(result, 0);
}

//Battery_GetParamIntValue
TEST_F(CcspMtaTestFixture, Battery_GetParamIntValue_ReturnsFalse) {
    ANSC_HANDLE hInsContext = nullptr;
    char ParamName[] = "TestParam";
    int pInt = 0;

    BOOL result = Battery_GetParamIntValue(hInsContext, ParamName, &pInt);

    EXPECT_FALSE(result);
}

// Test case for X_CISCO_COM_MTA_Commit
TEST_F(CcspMtaTestFixture, TestX_CISCO_COM_MTA_Commit) {
    
    g_pCosaBEManager = (PCOSA_BACKEND_MANAGER_OBJECT)malloc(sizeof(COSA_BACKEND_MANAGER_OBJECT));
    ASSERT_NE(g_pCosaBEManager, nullptr) << "Failed to allocate memory for g_pCosaBEManager";

    g_pCosaBEManager->hMTA = (PCOSA_DATAMODEL_MTA)malloc(sizeof(COSA_DATAMODEL_MTA));
    ASSERT_NE(g_pCosaBEManager->hMTA, nullptr) << "Failed to allocate memory for g_pCosaBEManager->hMTA";

    
    memset(g_pCosaBEManager->hMTA, 0, sizeof(COSA_DATAMODEL_MTA));

   
    ULONG result = X_CISCO_COM_MTA_Commit(NULL);

    
    EXPECT_EQ(result, 0);

   
    free(g_pCosaBEManager->hMTA);
    free(g_pCosaBEManager);
}

//Test case for TestX_CISCO_COM_MTA_Rollback
TEST_F(CcspMtaTestFixture, TestX_CISCO_COM_MTA_Rollback) {
    
    g_pCosaBEManager = (PCOSA_BACKEND_MANAGER_OBJECT)malloc(sizeof(COSA_BACKEND_MANAGER_OBJECT));
    ASSERT_NE(g_pCosaBEManager, nullptr) << "Failed to allocate memory for g_pCosaBEManager";

    g_pCosaBEManager->hMTA = (PCOSA_DATAMODEL_MTA)malloc(sizeof(COSA_DATAMODEL_MTA));
    ASSERT_NE(g_pCosaBEManager->hMTA, nullptr) << "Failed to allocate memory for g_pCosaBEManager->hMTA";

    
    memset(g_pCosaBEManager->hMTA, 0, sizeof(COSA_DATAMODEL_MTA));


    
    ULONG result = X_CISCO_COM_MTA_Rollback(NULL);

    
    EXPECT_EQ(result, 0);

    
    free(g_pCosaBEManager->hMTA);
    free(g_pCosaBEManager);
}

//LineTable_GetEntryCount
TEST_F(CcspMtaTestFixture, TestLineTable_GetEntryCount) {
   
    g_pCosaBEManager = (PCOSA_BACKEND_MANAGER_OBJECT)malloc(sizeof(COSA_BACKEND_MANAGER_OBJECT));
    ASSERT_NE(g_pCosaBEManager, nullptr) << "Failed to allocate memory for g_pCosaBEManager";

   
    g_pCosaBEManager->hMTA = (PCOSA_DATAMODEL_MTA)malloc(sizeof(COSA_DATAMODEL_MTA));
    ASSERT_NE(g_pCosaBEManager->hMTA, nullptr) << "Failed to allocate memory for g_pCosaBEManager->hMTA";

    
    PCOSA_DATAMODEL_MTA pMyObject = (PCOSA_DATAMODEL_MTA)g_pCosaBEManager->hMTA;
    memset(pMyObject, 0, sizeof(COSA_DATAMODEL_MTA));
    pMyObject->LineTableCount = 5;

   
    ULONG result = LineTable_GetEntryCount(NULL);

   
    EXPECT_EQ(result, 5);

    
    free(g_pCosaBEManager->hMTA);
    free(g_pCosaBEManager);
}

//Calls_GetEntryCount
TEST_F(CcspMtaTestFixture, TestCalls_GetEntryCount) {
   
    PCOSA_MTA_LINETABLE_INFO pMyObject = (PCOSA_MTA_LINETABLE_INFO)malloc(sizeof(COSA_MTA_LINETABLE_INFO));
    ASSERT_NE(pMyObject, nullptr) << "Failed to allocate memory for pMyObject";

    
    memset(pMyObject, 0, sizeof(COSA_MTA_LINETABLE_INFO));
    pMyObject->CallsNumber = 10;  

    
    ULONG result = Calls_GetEntryCount((ANSC_HANDLE)pMyObject);

    
    EXPECT_EQ(result, 10);

    
    free(pMyObject);
}

//ServiceClass_GetEntryCount
TEST_F(CcspMtaTestFixture, TestServiceClass_GetEntryCount) {
    
    g_pCosaBEManager = (PCOSA_BACKEND_MANAGER_OBJECT)malloc(sizeof(COSA_BACKEND_MANAGER_OBJECT));
    ASSERT_NE(g_pCosaBEManager, nullptr) << "Failed to allocate memory for g_pCosaBEManager";

    
    g_pCosaBEManager->hMTA = (PCOSA_DATAMODEL_MTA)malloc(sizeof(COSA_DATAMODEL_MTA));
    ASSERT_NE(g_pCosaBEManager->hMTA, nullptr) << "Failed to allocate memory for g_pCosaBEManager->hMTA";

   
    PCOSA_DATAMODEL_MTA pMyObject = (PCOSA_DATAMODEL_MTA)g_pCosaBEManager->hMTA;
    memset(pMyObject, 0, sizeof(COSA_DATAMODEL_MTA));
    pMyObject->ServiceClassNumber = 7;  

    
    ULONG result = ServiceClass_GetEntryCount(NULL);

   
    EXPECT_EQ(result, 7);

    
    free(g_pCosaBEManager->hMTA);
    free(g_pCosaBEManager);
}

//ServiceFlow_GetEntryCount
TEST_F(CcspMtaTestFixture, TestServiceFlow_GetEntryCount) {
    g_pCosaBEManager = (PCOSA_BACKEND_MANAGER_OBJECT)malloc(sizeof(COSA_BACKEND_MANAGER_OBJECT));
    ASSERT_NE(g_pCosaBEManager, nullptr) << "Failed to allocate memory for g_pCosaBEManager";

    g_pCosaBEManager->hMTA = (PCOSA_DATAMODEL_MTA)malloc(sizeof(COSA_DATAMODEL_MTA));
    ASSERT_NE(g_pCosaBEManager->hMTA, nullptr) << "Failed to allocate memory for g_pCosaBEManager->hMTA";

    PCOSA_DATAMODEL_MTA pMyObject = (PCOSA_DATAMODEL_MTA)g_pCosaBEManager->hMTA;
    memset(pMyObject, 0, sizeof(COSA_DATAMODEL_MTA));
    pMyObject->ServiceFlowNumber = 12;  

    ULONG result = ServiceFlow_GetEntryCount(NULL);

    EXPECT_EQ(result, 12);

    free(g_pCosaBEManager->hMTA);
    free(g_pCosaBEManager);
}

//Handsets_GetEntryCount
TEST_F(CcspMtaTestFixture, TestHandsets_GetEntryCount) {
    g_pCosaBEManager = (PCOSA_BACKEND_MANAGER_OBJECT)malloc(sizeof(COSA_BACKEND_MANAGER_OBJECT));
    ASSERT_NE(g_pCosaBEManager, nullptr) << "Failed to allocate memory for g_pCosaBEManager";

    g_pCosaBEManager->hMTA = (PCOSA_DATAMODEL_MTA)malloc(sizeof(COSA_DATAMODEL_MTA));
    ASSERT_NE(g_pCosaBEManager->hMTA, nullptr) << "Failed to allocate memory for g_pCosaBEManager->hMTA";

    PCOSA_DATAMODEL_MTA pMyObject = (PCOSA_DATAMODEL_MTA)g_pCosaBEManager->hMTA;
    memset(pMyObject, 0, sizeof(COSA_DATAMODEL_MTA));
    pMyObject->HandsetsNumber = 5; 

    ULONG result = Handsets_GetEntryCount(NULL);

    EXPECT_EQ(result, 5);
    free(g_pCosaBEManager->hMTA);
    free(g_pCosaBEManager);
}

//DSXLog_GetEntryCount
TEST_F(CcspMtaTestFixture, TestDSXLog_GetEntryCount) {
    g_pCosaBEManager = (PCOSA_BACKEND_MANAGER_OBJECT)malloc(sizeof(COSA_BACKEND_MANAGER_OBJECT));
    ASSERT_NE(g_pCosaBEManager, nullptr) << "Failed to allocate memory for g_pCosaBEManager";

    g_pCosaBEManager->hMTA = (PCOSA_DATAMODEL_MTA)malloc(sizeof(COSA_DATAMODEL_MTA));
    ASSERT_NE(g_pCosaBEManager->hMTA, nullptr) << "Failed to allocate memory for g_pCosaBEManager->hMTA";

    PCOSA_DATAMODEL_MTA pMyObject = (PCOSA_DATAMODEL_MTA)g_pCosaBEManager->hMTA;
    memset(pMyObject, 0, sizeof(COSA_DATAMODEL_MTA));
    pMyObject->DSXLogNumber = 3;  

    ULONG result = DSXLog_GetEntryCount(NULL);

    EXPECT_EQ(result, 3);

    free(g_pCosaBEManager->hMTA);
    free(g_pCosaBEManager);
}

//MTALog_GetEntryCount
TEST_F(CcspMtaTestFixture, TestMTALog_GetEntryCount) {
    g_pCosaBEManager = (PCOSA_BACKEND_MANAGER_OBJECT)malloc(sizeof(COSA_BACKEND_MANAGER_OBJECT));
    ASSERT_NE(g_pCosaBEManager, nullptr) << "Failed to allocate memory for g_pCosaBEManager";

    g_pCosaBEManager->hMTA = (PCOSA_DATAMODEL_MTA)malloc(sizeof(COSA_DATAMODEL_MTA));
    ASSERT_NE(g_pCosaBEManager->hMTA, nullptr) << "Failed to allocate memory for g_pCosaBEManager->hMTA";

    PCOSA_DATAMODEL_MTA pMyObject = (PCOSA_DATAMODEL_MTA)g_pCosaBEManager->hMTA;
    memset(pMyObject, 0, sizeof(COSA_DATAMODEL_MTA));
    pMyObject->MtaLogNumber = 4;  

    ULONG result = MTALog_GetEntryCount(NULL);

    EXPECT_EQ(result, 4);

    free(g_pCosaBEManager->hMTA);
    free(g_pCosaBEManager);
}

//DECTLog_GetEntryCount
TEST_F(CcspMtaTestFixture, TestDECTLog_GetEntryCount) {
    g_pCosaBEManager = (PCOSA_BACKEND_MANAGER_OBJECT)malloc(sizeof(COSA_BACKEND_MANAGER_OBJECT));
    ASSERT_NE(g_pCosaBEManager, nullptr) << "Failed to allocate memory for g_pCosaBEManager";

    g_pCosaBEManager->hMTA = (PCOSA_DATAMODEL_MTA)malloc(sizeof(COSA_DATAMODEL_MTA));
    ASSERT_NE(g_pCosaBEManager->hMTA, nullptr) << "Failed to allocate memory for g_pCosaBEManager->hMTA";

    PCOSA_DATAMODEL_MTA pMyObject = (PCOSA_DATAMODEL_MTA)g_pCosaBEManager->hMTA;
    memset(pMyObject, 0, sizeof(COSA_DATAMODEL_MTA));
    pMyObject->DectLogNumber = 5;  

    ULONG result = DECTLog_GetEntryCount(NULL);

    EXPECT_EQ(result, 5);

    free(g_pCosaBEManager->hMTA);
    free(g_pCosaBEManager);
}

// LineTable_GetEntry
TEST_F(CcspMtaTestFixture, TestLineTable_GetEntry) {
    g_pCosaBEManager = (PCOSA_BACKEND_MANAGER_OBJECT)malloc(sizeof(COSA_BACKEND_MANAGER_OBJECT));
    ASSERT_NE(g_pCosaBEManager, nullptr) << "Failed to allocate memory for g_pCosaBEManager";

    g_pCosaBEManager->hMTA = (PCOSA_DATAMODEL_MTA)malloc(sizeof(COSA_DATAMODEL_MTA));
    ASSERT_NE(g_pCosaBEManager->hMTA, nullptr) << "Failed to allocate memory for g_pCosaBEManager->hMTA";

    PCOSA_DATAMODEL_MTA pMyObject = (PCOSA_DATAMODEL_MTA)g_pCosaBEManager->hMTA;
    memset(pMyObject, 0, sizeof(COSA_DATAMODEL_MTA));
    
    ULONG maxEntries = 5;
    pMyObject->pLineTable = (PCOSA_MTA_LINETABLE_INFO)malloc(maxEntries * sizeof(COSA_MTA_LINETABLE_INFO));
    ASSERT_NE(pMyObject->pLineTable, nullptr) << "Failed to allocate memory for pLineTable";

    for (ULONG i = 0; i < maxEntries; ++i) {
        pMyObject->pLineTable[i].InstanceNumber = 0; 
    }

    ULONG insNumber = 0;
    ULONG indexToTest = 2; 

    ANSC_HANDLE result = LineTable_GetEntry(NULL, indexToTest, &insNumber);

    EXPECT_EQ(insNumber, indexToTest + 1);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(((PCOSA_MTA_LINETABLE_INFO)result)->InstanceNumber, indexToTest + 1);

    free(pMyObject->pLineTable);
    free(g_pCosaBEManager->hMTA);
    free(g_pCosaBEManager);
}

// Calls_GetEntry
TEST_F(CcspMtaTestFixture, TestCalls_GetEntry_ValidIndex) {
    PCOSA_MTA_LINETABLE_INFO pMyObject = new COSA_MTA_LINETABLE_INFO;
    int maxCalls = 5;

    pMyObject->pCalls = (PCOSA_MTA_CALLS)malloc(maxCalls * sizeof(COSA_MTA_CALLS));
    ASSERT_NE(pMyObject->pCalls, nullptr) << "Failed to allocate memory for pCalls"; 
    pMyObject->CallsNumber = maxCalls;

  
    for (int i = 0; i < maxCalls; i++) {
        strcpy(pMyObject->pCalls[i].Codec, "G711"); 
        pMyObject->pCalls[i].CallDuration = i * 10;
    }

    ULONG insNumber = 0;
    ANSC_HANDLE result = Calls_GetEntry((ANSC_HANDLE)pMyObject, 2, &insNumber); 

    ASSERT_NE(result, nullptr); 
    EXPECT_EQ(insNumber, 3);   
    EXPECT_STREQ(((PCOSA_MTA_CALLS)result)->Codec, "G711"); 
    EXPECT_EQ(((PCOSA_MTA_CALLS)result)->CallDuration, 20); 

    free(pMyObject->pCalls); 
    delete pMyObject;
}

TEST_F(CcspMtaTestFixture, TestCalls_GetEntry_InvalidIndex) {
    PCOSA_MTA_LINETABLE_INFO pMyObject = new COSA_MTA_LINETABLE_INFO;
    int maxCalls = 5;

    pMyObject->pCalls = (PCOSA_MTA_CALLS)malloc(maxCalls * sizeof(COSA_MTA_CALLS));
    ASSERT_NE(pMyObject->pCalls, nullptr) << "Failed to allocate memory for pCalls"; 
    pMyObject->CallsNumber = maxCalls;

    ULONG insNumber = 0;
    ANSC_HANDLE result = Calls_GetEntry((ANSC_HANDLE)pMyObject, 10, &insNumber); 

    EXPECT_EQ(result, nullptr); 

    free(pMyObject->pCalls); 
    delete pMyObject;
}

//ServiceClass_GetEntry
TEST_F(CcspMtaTestFixture, TestServiceClass_GetEntry) {
    PCOSA_DATAMODEL_MTA mockMta = (PCOSA_DATAMODEL_MTA)malloc(sizeof(COSA_DATAMODEL_MTA));
    ASSERT_NE(mockMta, nullptr) << "Failed to allocate memory for mockMta"; 

    mockMta->ServiceClassNumber = 3;
    mockMta->pServiceClass = (PCOSA_MTA_SERVICE_CLASS)malloc(3 * sizeof(COSA_MTA_SERVICE_CLASS));
    ASSERT_NE(mockMta->pServiceClass, nullptr) << "Failed to allocate memory for pServiceClass"; 

    strncpy(mockMta->pServiceClass[0].ServiceClassName, "ServiceClass1", sizeof(mockMta->pServiceClass[0].ServiceClassName));
    strncpy(mockMta->pServiceClass[1].ServiceClassName, "ServiceClass2", sizeof(mockMta->pServiceClass[1].ServiceClassName));
    strncpy(mockMta->pServiceClass[2].ServiceClassName, "ServiceClass3", sizeof(mockMta->pServiceClass[2].ServiceClassName));

    g_pCosaBEManager->hMTA = mockMta;

    ULONG insNumber = 0;
    PCOSA_MTA_SERVICE_CLASS result = (PCOSA_MTA_SERVICE_CLASS)ServiceClass_GetEntry(NULL, 1, &insNumber);

    ASSERT_NE(result, nullptr) << "Result is nullptr for valid index.";
    EXPECT_EQ(insNumber, 2); 
    EXPECT_STREQ(result->ServiceClassName, "ServiceClass2");

    ULONG invalidIndex = 5; 
    result = (PCOSA_MTA_SERVICE_CLASS)ServiceClass_GetEntry(NULL, invalidIndex, &insNumber);

    // Verify that the result is NULL
    EXPECT_EQ(result, nullptr); 

    // Clean up
    free(mockMta->pServiceClass);
    free(mockMta);
}


//ServiceFlow_GetEntry
TEST_F(CcspMtaTestFixture, TestServiceFlow_GetEntry) {
    PCOSA_DATAMODEL_MTA mockMta = (PCOSA_DATAMODEL_MTA)malloc(sizeof(COSA_DATAMODEL_MTA));
    ASSERT_NE(mockMta, nullptr) << "Failed to allocate memory for mockMta";

    mockMta->ServiceFlowNumber = 3;
    mockMta->pServiceFlow = (PCOSA_MTA_SERVICE_FLOW)malloc(mockMta->ServiceFlowNumber * sizeof(COSA_MTA_SERVICE_FLOW));
    ASSERT_NE(mockMta->pServiceFlow, nullptr) << "Failed to allocate memory for pServiceFlow";

    for (ULONG i = 0; i < mockMta->ServiceFlowNumber; ++i) {
        mockMta->pServiceFlow[i].SFID = i + 1;
        snprintf(mockMta->pServiceFlow[i].ServiceClassName, sizeof(mockMta->pServiceFlow[i].ServiceClassName), "ServiceFlow%d", i + 1);
      
    }

    g_pCosaBEManager->hMTA = (ANSC_HANDLE)mockMta; 

    ULONG insNumber = 0;
    PCOSA_MTA_SERVICE_FLOW result = (PCOSA_MTA_SERVICE_FLOW)ServiceFlow_GetEntry(NULL, 1, &insNumber);

    ASSERT_NE(result, nullptr) << "Result is nullptr. Check the ServiceFlow_GetEntry implementation.";
    EXPECT_EQ(insNumber, 2); 
    EXPECT_EQ(result->SFID, 2);
    EXPECT_STREQ(result->ServiceClassName, "ServiceFlow2");

 
    free(mockMta->pServiceFlow);
    free(mockMta);
}

//Handsets_GetEntry
TEST_F(CcspMtaTestFixture, TestHandsets_GetEntry) {
    g_pCosaBEManager = (PCOSA_BACKEND_MANAGER_OBJECT)malloc(sizeof(COSA_BACKEND_MANAGER_OBJECT));
    ASSERT_NE(g_pCosaBEManager, nullptr) << "Failed to allocate memory for g_pCosaBEManager";

    g_pCosaBEManager->hMTA = (PCOSA_DATAMODEL_MTA)malloc(sizeof(COSA_DATAMODEL_MTA));
    ASSERT_NE(g_pCosaBEManager->hMTA, nullptr) << "Failed to allocate memory for g_pCosaBEManager->hMTA";

    PCOSA_DATAMODEL_MTA pMyObject = (PCOSA_DATAMODEL_MTA)g_pCosaBEManager->hMTA;
    memset(pMyObject, 0, sizeof(COSA_DATAMODEL_MTA));

    pMyObject->HandsetsNumber = 3;
    pMyObject->pHandsets = (PCOSA_MTA_HANDSETS_INFO)malloc(pMyObject->HandsetsNumber * sizeof(COSA_MTA_HANDSETS_INFO));
    ASSERT_NE(pMyObject->pHandsets, nullptr) << "Failed to allocate memory for pHandsets";

    for (ULONG i = 0; i < pMyObject->HandsetsNumber; ++i) {
        pMyObject->pHandsets[i].InstanceNumber = i + 1;
        snprintf(pMyObject->pHandsets[i].HandsetName, sizeof(pMyObject->pHandsets[i].HandsetName), "Handset%d", i + 1);
        snprintf(pMyObject->pHandsets[i].LastActiveTime, sizeof(pMyObject->pHandsets[i].LastActiveTime), "2024-10-11 10:0%d:00", i);
    }

    ULONG insNumber = 0;
    PCOSA_MTA_HANDSETS_INFO result = (PCOSA_MTA_HANDSETS_INFO)Handsets_GetEntry(NULL, 1, &insNumber);

    ASSERT_NE(result, nullptr);
    EXPECT_EQ(insNumber, 2);  
    EXPECT_STREQ(result->HandsetName, "Handset2");
    EXPECT_STREQ(result->LastActiveTime, "2024-10-11 10:01:00");

    result = (PCOSA_MTA_HANDSETS_INFO)Handsets_GetEntry(NULL, 5, &insNumber);
    EXPECT_EQ(result, nullptr);  

    free(pMyObject->pHandsets);
    free(g_pCosaBEManager->hMTA);
    free(g_pCosaBEManager);
}

//DSXLog_GetEntry
TEST_F(CcspMtaTestFixture, TestDSXLog_GetEntry_ValidIndex) {
    g_pCosaBEManager = (PCOSA_BACKEND_MANAGER_OBJECT)malloc(sizeof(COSA_BACKEND_MANAGER_OBJECT));
    ASSERT_NE(g_pCosaBEManager, nullptr) << "Failed to allocate memory for g_pCosaBEManager";

    g_pCosaBEManager->hMTA = (PCOSA_DATAMODEL_MTA)malloc(sizeof(COSA_DATAMODEL_MTA));
    ASSERT_NE(g_pCosaBEManager->hMTA, nullptr) << "Failed to allocate memory for g_pCosaBEManager->hMTA";

    PCOSA_DATAMODEL_MTA pMyObject = (PCOSA_DATAMODEL_MTA)g_pCosaBEManager->hMTA;
    memset(pMyObject, 0, sizeof(COSA_DATAMODEL_MTA));

    ULONG maxEntries = 3;
    pMyObject->pDSXLog = (PCOSA_MTA_DSXLOG)malloc(maxEntries * sizeof(COSA_MTA_DSXLOG));
    ASSERT_NE(pMyObject->pDSXLog, nullptr) << "Failed to allocate memory for pDSXLog";
    pMyObject->DSXLogNumber = maxEntries;

    for (ULONG i = 0; i < maxEntries; ++i) {
        strcpy(pMyObject->pDSXLog[i].Time, "2024-10-10 12:00:00");
        strcpy(pMyObject->pDSXLog[i].Description, "Log description");
        pMyObject->pDSXLog[i].ID = i + 1;
        pMyObject->pDSXLog[i].Level = i;
    }

    ULONG insNumber = 0;
    ULONG indexToTest = 1;
    ANSC_HANDLE result = DSXLog_GetEntry(NULL, indexToTest, &insNumber);

    ASSERT_NE(result, nullptr); 
    EXPECT_EQ(insNumber, indexToTest + 1);
    EXPECT_STREQ(((PCOSA_MTA_DSXLOG)result)->Time, "2024-10-10 12:00:00");
    EXPECT_STREQ(((PCOSA_MTA_DSXLOG)result)->Description, "Log description");
    EXPECT_EQ(((PCOSA_MTA_DSXLOG)result)->ID, indexToTest + 1);
    EXPECT_EQ(((PCOSA_MTA_DSXLOG)result)->Level, indexToTest);

    free(pMyObject->pDSXLog);
    free(g_pCosaBEManager->hMTA);
    free(g_pCosaBEManager);
}

TEST_F(CcspMtaTestFixture, TestDSXLog_GetEntry_InvalidIndex) {
    g_pCosaBEManager = (PCOSA_BACKEND_MANAGER_OBJECT)malloc(sizeof(COSA_BACKEND_MANAGER_OBJECT));
    ASSERT_NE(g_pCosaBEManager, nullptr) << "Failed to allocate memory for g_pCosaBEManager";

    g_pCosaBEManager->hMTA = (PCOSA_DATAMODEL_MTA)malloc(sizeof(COSA_DATAMODEL_MTA));
    ASSERT_NE(g_pCosaBEManager->hMTA, nullptr) << "Failed to allocate memory for g_pCosaBEManager->hMTA";

    PCOSA_DATAMODEL_MTA pMyObject = (PCOSA_DATAMODEL_MTA)g_pCosaBEManager->hMTA;
    memset(pMyObject, 0, sizeof(COSA_DATAMODEL_MTA));

    ULONG maxEntries = 3;
    pMyObject->pDSXLog = (PCOSA_MTA_DSXLOG)malloc(maxEntries * sizeof(COSA_MTA_DSXLOG));
    ASSERT_NE(pMyObject->pDSXLog, nullptr) << "Failed to allocate memory for pDSXLog";
    pMyObject->DSXLogNumber = maxEntries;

    ULONG insNumber = 0;
    ULONG invalidIndex = 5;

    ANSC_HANDLE result = DSXLog_GetEntry(NULL, invalidIndex, &insNumber);

    EXPECT_EQ(result, nullptr);

    free(pMyObject->pDSXLog);
    free(g_pCosaBEManager->hMTA);
    free(g_pCosaBEManager);
}

//MTALog_GetEntry
TEST_F(CcspMtaTestFixture, TestMTALog_GetEntry_ValidIndex) {
    g_pCosaBEManager = (PCOSA_BACKEND_MANAGER_OBJECT)malloc(sizeof(COSA_BACKEND_MANAGER_OBJECT));
    ASSERT_NE(g_pCosaBEManager, nullptr) << "Failed to allocate memory for g_pCosaBEManager";

    g_pCosaBEManager->hMTA = (PCOSA_DATAMODEL_MTA)malloc(sizeof(COSA_DATAMODEL_MTA));
    ASSERT_NE(g_pCosaBEManager->hMTA, nullptr) << "Failed to allocate memory for g_pCosaBEManager->hMTA";

    PCOSA_DATAMODEL_MTA pMyObject = (PCOSA_DATAMODEL_MTA)g_pCosaBEManager->hMTA;
    memset(pMyObject, 0, sizeof(COSA_DATAMODEL_MTA));

    ULONG maxEntries = 3;
    pMyObject->pMtaLog = (PCOSA_DML_MTALOG_FULL)malloc(maxEntries * sizeof(COSA_DML_MTALOG_FULL));
    ASSERT_NE(pMyObject->pMtaLog, nullptr) << "Failed to allocate memory for pMtaLog";
    pMyObject->MtaLogNumber = maxEntries;
    for (ULONG i = 0; i < maxEntries; ++i) {
        pMyObject->pMtaLog[i].Index = i + 1;
        pMyObject->pMtaLog[i].EventID = i + 100;
        strcpy(pMyObject->pMtaLog[i].EventLevel, "INFO");
        strcpy(pMyObject->pMtaLog[i].Time, "2024-10-11 12:00:00");
        pMyObject->pMtaLog[i].pDescription = strdup("Log description");
    }

    ULONG insNumber = 0;
    ULONG indexToTest = 1;

    ANSC_HANDLE result = MTALog_GetEntry(NULL, indexToTest, &insNumber);

    ASSERT_NE(result, nullptr); 
    EXPECT_EQ(insNumber, indexToTest + 1);
    EXPECT_EQ(((PCOSA_DML_MTALOG_FULL)result)->Index, indexToTest + 1);
    EXPECT_EQ(((PCOSA_DML_MTALOG_FULL)result)->EventID, indexToTest + 100);
    EXPECT_STREQ(((PCOSA_DML_MTALOG_FULL)result)->EventLevel, "INFO");
    EXPECT_STREQ(((PCOSA_DML_MTALOG_FULL)result)->Time, "2024-10-11 12:00:00");
    EXPECT_STREQ(((PCOSA_DML_MTALOG_FULL)result)->pDescription, "Log description");

    for (ULONG i = 0; i < maxEntries; ++i) {
        free(pMyObject->pMtaLog[i].pDescription);
    }
    free(pMyObject->pMtaLog);
    free(g_pCosaBEManager->hMTA);
    free(g_pCosaBEManager);
}

TEST_F(CcspMtaTestFixture, TestMTALog_GetEntry_InvalidIndex) {
    g_pCosaBEManager = (PCOSA_BACKEND_MANAGER_OBJECT)malloc(sizeof(COSA_BACKEND_MANAGER_OBJECT));
    ASSERT_NE(g_pCosaBEManager, nullptr) << "Failed to allocate memory for g_pCosaBEManager";

    g_pCosaBEManager->hMTA = (PCOSA_DATAMODEL_MTA)malloc(sizeof(COSA_DATAMODEL_MTA));
    ASSERT_NE(g_pCosaBEManager->hMTA, nullptr) << "Failed to allocate memory for g_pCosaBEManager->hMTA";

    PCOSA_DATAMODEL_MTA pMyObject = (PCOSA_DATAMODEL_MTA)g_pCosaBEManager->hMTA;
    memset(pMyObject, 0, sizeof(COSA_DATAMODEL_MTA));

    ULONG maxEntries = 3;
    pMyObject->pMtaLog = (PCOSA_DML_MTALOG_FULL)malloc(maxEntries * sizeof(COSA_DML_MTALOG_FULL));
    ASSERT_NE(pMyObject->pMtaLog, nullptr) << "Failed to allocate memory for pMtaLog";
    pMyObject->MtaLogNumber = maxEntries;

    ULONG insNumber = 0;
    ULONG invalidIndex = 5; 

    ANSC_HANDLE result = MTALog_GetEntry(NULL, invalidIndex, &insNumber);

    EXPECT_EQ(result, nullptr);

    // Clean up
    free(pMyObject->pMtaLog);
    free(g_pCosaBEManager->hMTA);
    free(g_pCosaBEManager);
}

//DECTLog_GetEntry
TEST_F(CcspMtaTestFixture, TestDECTLog_GetEntry_ValidIndex) {
   
    g_pCosaBEManager = (PCOSA_BACKEND_MANAGER_OBJECT)malloc(sizeof(COSA_BACKEND_MANAGER_OBJECT));
    ASSERT_NE(g_pCosaBEManager, nullptr) << "Failed to allocate memory for g_pCosaBEManager";

    g_pCosaBEManager->hMTA = (PCOSA_DATAMODEL_MTA)malloc(sizeof(COSA_DATAMODEL_MTA));
    ASSERT_NE(g_pCosaBEManager->hMTA, nullptr) << "Failed to allocate memory for g_pCosaBEManager->hMTA";

    PCOSA_DATAMODEL_MTA pMyObject = (PCOSA_DATAMODEL_MTA)g_pCosaBEManager->hMTA;
    memset(pMyObject, 0, sizeof(COSA_DATAMODEL_MTA));

    ULONG maxEntries = 3;
    pMyObject->pDectLog = (PCOSA_DML_DECTLOG_FULL)malloc(maxEntries * sizeof(COSA_DML_DECTLOG_FULL));
    ASSERT_NE(pMyObject->pDectLog, nullptr) << "Failed to allocate memory for pDectLog";
    pMyObject->DectLogNumber = maxEntries;

    
    for (ULONG i = 0; i < maxEntries; ++i) {
        pMyObject->pDectLog[i].Index = i + 1;
        pMyObject->pDectLog[i].EventID = i + 100;
        strcpy(pMyObject->pDectLog[i].Time, "2024-10-11 12:00:00");
        strcpy(pMyObject->pDectLog[i].Description, "DECT Log description");
    }

    ULONG insNumber = 0;
    ULONG indexToTest = 1; 

    ANSC_HANDLE result = DECTLog_GetEntry(NULL, indexToTest, &insNumber);

    ASSERT_NE(result, nullptr); 
    

   
    free(pMyObject->pDectLog);
    free(g_pCosaBEManager->hMTA);
    free(g_pCosaBEManager);
}

TEST_F(CcspMtaTestFixture, TestDECTLog_GetEntry_InvalidIndex) {
   
    g_pCosaBEManager = (PCOSA_BACKEND_MANAGER_OBJECT)malloc(sizeof(COSA_BACKEND_MANAGER_OBJECT));
    ASSERT_NE(g_pCosaBEManager, nullptr) << "Failed to allocate memory for g_pCosaBEManager";

    g_pCosaBEManager->hMTA = (PCOSA_DATAMODEL_MTA)malloc(sizeof(COSA_DATAMODEL_MTA));
    ASSERT_NE(g_pCosaBEManager->hMTA, nullptr) << "Failed to allocate memory for g_pCosaBEManager->hMTA";

   
    PCOSA_DATAMODEL_MTA pMyObject = (PCOSA_DATAMODEL_MTA)g_pCosaBEManager->hMTA;
    memset(pMyObject, 0, sizeof(COSA_DATAMODEL_MTA));

    ULONG maxEntries = 3;
    pMyObject->pDectLog = (PCOSA_DML_DECTLOG_FULL)malloc(maxEntries * sizeof(COSA_DML_DECTLOG_FULL));
    ASSERT_NE(pMyObject->pDectLog, nullptr) << "Failed to allocate memory for pDectLog";
    pMyObject->DectLogNumber = maxEntries;

    ULONG insNumber = 0;
    ULONG invalidIndex = 5; 

    
    ANSC_HANDLE result = DECTLog_GetEntry(NULL, invalidIndex, &insNumber);

    EXPECT_EQ(result, nullptr);

    // Clean up
    free(pMyObject->pDectLog);
    free(g_pCosaBEManager->hMTA);
    free(g_pCosaBEManager);
}

//X_CISCO_COM_MTA_V6_GetParamUlongValue
TEST_F(CcspMtaTestFixture, X_CISCO_COM_MTA_V6_GetParamUlongValue_Success) {
    ANSC_HANDLE hInsContext = nullptr;
    char paramName[] = "LeaseTimeRemaining";
    ULONG leaseTimeRemaining;
    
    COSA_MTA_DHCPv6_INFO dhcpv6Info;
    dhcpv6Info.LeaseTimeRemaining = 3600;

     EXPECT_CALL(*g_mtaHALMock, mta_hal_GetDHCPV6Info(testing::_))
        .WillOnce(testing::Return(ANSC_STATUS_SUCCESS));

    
    BOOL result = X_CISCO_COM_MTA_V6_GetParamUlongValue(hInsContext, paramName, &leaseTimeRemaining);

    EXPECT_TRUE(result);  
 
}

TEST_F(CcspMtaTestFixture, X_CISCO_COM_MTA_V6_GetParamUlongValue_Failure) {
    ANSC_HANDLE hInsContext = nullptr;
    char paramName[] = "LeaseTimeRemaining";
    ULONG leaseTimeRemaining;

    // Mock the return of CosaDmlMTAGetDHCPV6Info to failure
     EXPECT_CALL(*g_mtaHALMock, mta_hal_GetDHCPV6Info(testing::_))
        .WillOnce(testing::Return(ANSC_STATUS_FAILURE));

    // Call the function under test
    BOOL result = X_CISCO_COM_MTA_V6_GetParamUlongValue(hInsContext, paramName, &leaseTimeRemaining);

    // Validate the expected behavior
    EXPECT_FALSE(result);  // Function should return FALSE
}

// Test for unsupported parameter name
TEST_F(CcspMtaTestFixture, X_CISCO_COM_MTA_V6_GetParamUlongValue_UnsupportedParam) {
    ANSC_HANDLE hInsContext = nullptr;
    char paramName[] = "UnsupportedParam";  // Unsupported parameter
    ULONG leaseTimeRemaining;
   EXPECT_CALL(*g_mtaHALMock, mta_hal_GetDHCPV6Info(testing::_))
        .WillOnce(testing::Return(ANSC_STATUS_SUCCESS));
    BOOL result = X_CISCO_COM_MTA_V6_GetParamUlongValue(hInsContext, paramName, &leaseTimeRemaining);

    EXPECT_FALSE(result);
}


//X_CISCO_COM_MTA_V6_GetParamStringValue
TEST_F(CcspMtaTestFixture, X_CISCO_COM_MTA_V6_GetParamStringValue_Success) {
    ANSC_HANDLE hInsContext = nullptr;
    char paramName[] = "IPV6Address";
    char value[INET6_ADDRSTRLEN];
    ULONG ulSize = sizeof(value);

    // Define expected DHCPv6 info structure
    _COSA_MTA_DHCPv6_INFO dhcpv6Info;
    strcpy(dhcpv6Info.IPV6Address, "2001:db8::1");

    // Mock the return of mta_hal_GetDHCPV6Info to success
    EXPECT_CALL(*g_mtaHALMock, mta_hal_GetDHCPV6Info(testing::_))
        .WillOnce(testing::Return(RETURN_OK));

    // Call the function under test
    ULONG result = X_CISCO_COM_MTA_V6_GetParamStringValue(hInsContext, paramName, value, &ulSize);

    // Validate the expected behavior
    EXPECT_EQ(result, 0);  // Success case
//     EXPECT_STREQ(value, "2001:db8::1");  // Ensure the value is as expected
}

// Failure case for X_CISCO_COM_MTA_V6_GetParamStringValue when mta_hal_GetDHCPV6Info fails
TEST_F(CcspMtaTestFixture, X_CISCO_COM_MTA_V6_GetParamStringValue_Failure) {
    ANSC_HANDLE hInsContext = nullptr;
    char paramName[] = "IPV6Address";
    char value[INET6_ADDRSTRLEN];
    ULONG ulSize = sizeof(value);

    // Mock the return of mta_hal_GetDHCPV6Info to failure
    EXPECT_CALL(*g_mtaHALMock, mta_hal_GetDHCPV6Info(testing::_))
        .WillOnce(testing::Return(RETURN_ERR));
    ULONG result = X_CISCO_COM_MTA_V6_GetParamStringValue(hInsContext, paramName, value, &ulSize);
    // Validate the expected behavior
    EXPECT_EQ(result, -1);  // Failure case, should return -1
}
// Success case for "Prefix"
TEST_F(CcspMtaTestFixture, X_CISCO_COM_MTA_V6_GetParamStringValue_Prefix_Success) {
    ANSC_HANDLE hInsContext = nullptr;
    char paramName[] = "Prefix";
    char value[INET6_ADDRSTRLEN];
    ULONG ulSize = sizeof(value);

    _COSA_MTA_DHCPv6_INFO dhcpv6Info;
    strcpy(dhcpv6Info.Prefix, "2001:db8::/32");

    EXPECT_CALL(*g_mtaHALMock, mta_hal_GetDHCPV6Info(testing::_))
        .WillOnce(testing::Return(RETURN_OK));

    ULONG result = X_CISCO_COM_MTA_V6_GetParamStringValue(hInsContext, paramName, value, &ulSize);
    EXPECT_EQ(result, 0);
}

// Failure case for "Prefix"
TEST_F(CcspMtaTestFixture, X_CISCO_COM_MTA_V6_GetParamStringValue_Prefix_Failure) {
    ANSC_HANDLE hInsContext = nullptr;
    char paramName[] = "Prefix";
    char value[INET6_ADDRSTRLEN];
    ULONG ulSize = sizeof(value);

    EXPECT_CALL(*g_mtaHALMock, mta_hal_GetDHCPV6Info(testing::_))
        .WillOnce(testing::Return(RETURN_ERR));
    
    ULONG result = X_CISCO_COM_MTA_V6_GetParamStringValue(hInsContext, paramName, value, &ulSize);
    EXPECT_EQ(result, -1);
}

// Success case for "BootFileName"
TEST_F(CcspMtaTestFixture, X_CISCO_COM_MTA_V6_GetParamStringValue_BootFileName_Success) {
    ANSC_HANDLE hInsContext = nullptr;
    char paramName[] = "BootFileName";
    char value[INET6_ADDRSTRLEN];
    ULONG ulSize = sizeof(value);

    _COSA_MTA_DHCPv6_INFO dhcpv6Info;
    strcpy(dhcpv6Info.BootFileName, "bootfile.bin");

    EXPECT_CALL(*g_mtaHALMock, mta_hal_GetDHCPV6Info(testing::_))
        .WillOnce(testing::Return(RETURN_OK));

    ULONG result = X_CISCO_COM_MTA_V6_GetParamStringValue(hInsContext, paramName, value, &ulSize);
    EXPECT_EQ(result, 0);
}

// Failure case for "BootFileName"
TEST_F(CcspMtaTestFixture, X_CISCO_COM_MTA_V6_GetParamStringValue_BootFileName_Failure) {
    ANSC_HANDLE hInsContext = nullptr;
    char paramName[] = "BootFileName";
    char value[INET6_ADDRSTRLEN];
    ULONG ulSize = sizeof(value);

    EXPECT_CALL(*g_mtaHALMock, mta_hal_GetDHCPV6Info(testing::_))
        .WillOnce(testing::Return(RETURN_ERR));
    
    ULONG result = X_CISCO_COM_MTA_V6_GetParamStringValue(hInsContext, paramName, value, &ulSize);
    EXPECT_EQ(result, -1);
}

// Success case for "FQDN"
TEST_F(CcspMtaTestFixture, X_CISCO_COM_MTA_V6_GetParamStringValue_FQDN_Success) {
    ANSC_HANDLE hInsContext = nullptr;
    char paramName[] = "FQDN";
    char value[INET6_ADDRSTRLEN];
    ULONG ulSize = sizeof(value);

    _COSA_MTA_DHCPv6_INFO dhcpv6Info;
    strcpy(dhcpv6Info.FQDN, "example.com");

    EXPECT_CALL(*g_mtaHALMock, mta_hal_GetDHCPV6Info(testing::_))
        .WillOnce(testing::Return(RETURN_OK));

    ULONG result = X_CISCO_COM_MTA_V6_GetParamStringValue(hInsContext, paramName, value, &ulSize);
    EXPECT_EQ(result, 0);
   // EXPECT_STREQ(value, "example.com");
}

// Failure case for "FQDN"
TEST_F(CcspMtaTestFixture, X_CISCO_COM_MTA_V6_GetParamStringValue_FQDN_Failure) {
    ANSC_HANDLE hInsContext = nullptr;
    char paramName[] = "FQDN";
    char value[INET6_ADDRSTRLEN];
    ULONG ulSize = sizeof(value);

    EXPECT_CALL(*g_mtaHALMock, mta_hal_GetDHCPV6Info(testing::_))
        .WillOnce(testing::Return(RETURN_ERR));
    
    ULONG result = X_CISCO_COM_MTA_V6_GetParamStringValue(hInsContext, paramName, value, &ulSize);
    EXPECT_EQ(result, -1);
}

// Success case for "Gateway"
TEST_F(CcspMtaTestFixture, X_CISCO_COM_MTA_V6_GetParamStringValue_Gateway_Success) {
    ANSC_HANDLE hInsContext = nullptr;
    char paramName[] = "Gateway";
    char value[INET6_ADDRSTRLEN];
    ULONG ulSize = sizeof(value);

    _COSA_MTA_DHCPv6_INFO dhcpv6Info;
    strcpy(dhcpv6Info.Gateway, "2001:db8::1");

    EXPECT_CALL(*g_mtaHALMock, mta_hal_GetDHCPV6Info(testing::_))
        .WillOnce(testing::Return(RETURN_OK));

    ULONG result = X_CISCO_COM_MTA_V6_GetParamStringValue(hInsContext, paramName, value, &ulSize);
    EXPECT_EQ(result, 0);
   
}

// Failure case for "Gateway"
TEST_F(CcspMtaTestFixture, X_CISCO_COM_MTA_V6_GetParamStringValue_Gateway_Failure) {
    ANSC_HANDLE hInsContext = nullptr;
    char paramName[] = "Gateway";
    char value[INET6_ADDRSTRLEN];
    ULONG ulSize = sizeof(value);

    EXPECT_CALL(*g_mtaHALMock, mta_hal_GetDHCPV6Info(testing::_))
        .WillOnce(testing::Return(RETURN_ERR));
    
    ULONG result = X_CISCO_COM_MTA_V6_GetParamStringValue(hInsContext, paramName, value, &ulSize);
    EXPECT_EQ(result, -1);
}

// Success case for "RebindTimeRemaining"
TEST_F(CcspMtaTestFixture, X_CISCO_COM_MTA_V6_GetParamStringValue_RebindTimeRemaining_Success) {
    ANSC_HANDLE hInsContext = nullptr;
    char paramName[] = "RebindTimeRemaining";
    char value[INET6_ADDRSTRLEN];
    ULONG ulSize = sizeof(value);

    _COSA_MTA_DHCPv6_INFO dhcpv6Info;
    strcpy(dhcpv6Info.RebindTimeRemaining, "3600");

    EXPECT_CALL(*g_mtaHALMock, mta_hal_GetDHCPV6Info(testing::_))
        .WillOnce(testing::Return(RETURN_OK));

    ULONG result = X_CISCO_COM_MTA_V6_GetParamStringValue(hInsContext, paramName, value, &ulSize);
    EXPECT_EQ(result, 0);
    
}

// Failure case for "RebindTimeRemaining"
TEST_F(CcspMtaTestFixture, X_CISCO_COM_MTA_V6_GetParamStringValue_RebindTimeRemaining_Failure) {
    ANSC_HANDLE hInsContext = nullptr;
    char paramName[] = "RebindTimeRemaining";
    char value[INET6_ADDRSTRLEN];
    ULONG ulSize = sizeof(value);

    EXPECT_CALL(*g_mtaHALMock, mta_hal_GetDHCPV6Info(testing::_))
        .WillOnce(testing::Return(RETURN_ERR));
    
    ULONG result = X_CISCO_COM_MTA_V6_GetParamStringValue(hInsContext, paramName, value, &ulSize);
    EXPECT_EQ(result, -1);
}

// Success case for "RenewTimeRemaining"
TEST_F(CcspMtaTestFixture, X_CISCO_COM_MTA_V6_GetParamStringValue_RenewTimeRemaining_Success) {
    ANSC_HANDLE hInsContext = nullptr;
    char paramName[] = "RenewTimeRemaining";
    char value[INET6_ADDRSTRLEN];
    ULONG ulSize = sizeof(value);

    _COSA_MTA_DHCPv6_INFO dhcpv6Info;
    strcpy(dhcpv6Info.RenewTimeRemaining, "1800");

    EXPECT_CALL(*g_mtaHALMock, mta_hal_GetDHCPV6Info(testing::_))
        .WillOnce(testing::Return(RETURN_OK));

    ULONG result = X_CISCO_COM_MTA_V6_GetParamStringValue(hInsContext, paramName, value, &ulSize);
    EXPECT_EQ(result, 0);

}

// Failure case for "RenewTimeRemaining"
TEST_F(CcspMtaTestFixture, X_CISCO_COM_MTA_V6_GetParamStringValue_RenewTimeRemaining_Failure) {
    ANSC_HANDLE hInsContext = nullptr;
    char paramName[] = "RenewTimeRemaining";
    char value[INET6_ADDRSTRLEN];
    ULONG ulSize = sizeof(value);

    EXPECT_CALL(*g_mtaHALMock, mta_hal_GetDHCPV6Info(testing::_))
        .WillOnce(testing::Return(RETURN_ERR));
    
    ULONG result = X_CISCO_COM_MTA_V6_GetParamStringValue(hInsContext, paramName, value, &ulSize);
    EXPECT_EQ(result, -1);
}

// Success case for "PrimaryDNS"
TEST_F(CcspMtaTestFixture, X_CISCO_COM_MTA_V6_GetParamStringValue_PrimaryDNS_Success) {
    ANSC_HANDLE hInsContext = nullptr;
    char paramName[] = "PrimaryDNS";
    char value[INET6_ADDRSTRLEN];
    ULONG ulSize = sizeof(value);

    _COSA_MTA_DHCPv6_INFO dhcpv6Info;
    strcpy(dhcpv6Info.PrimaryDNS, "2001:db8::53");

    EXPECT_CALL(*g_mtaHALMock, mta_hal_GetDHCPV6Info(testing::_))
        .WillOnce(testing::Return(RETURN_OK));

    ULONG result = X_CISCO_COM_MTA_V6_GetParamStringValue(hInsContext, paramName, value, &ulSize);
    EXPECT_EQ(result, 0);
}

// Failure case for "PrimaryDNS"
TEST_F(CcspMtaTestFixture, X_CISCO_COM_MTA_V6_GetParamStringValue_PrimaryDNS_Failure) {
    ANSC_HANDLE hInsContext = nullptr;
    char paramName[] = "PrimaryDNS";
    char value[INET6_ADDRSTRLEN];
    ULONG ulSize = sizeof(value);

    EXPECT_CALL(*g_mtaHALMock, mta_hal_GetDHCPV6Info(testing::_))
        .WillOnce(testing::Return(RETURN_ERR));
    
    ULONG result = X_CISCO_COM_MTA_V6_GetParamStringValue(hInsContext, paramName, value, &ulSize);
    EXPECT_EQ(result, -1);
}


// Test case for unsupported parameter name
TEST_F(CcspMtaTestFixture, X_CISCO_COM_MTA_V6_GetParamStringValue_UnsupportedParam) {
    ANSC_HANDLE hInsContext = nullptr;
    char paramName[] = "UnsupportedParam";
    char value[INET6_ADDRSTRLEN];
    ULONG ulSize = sizeof(value);
     EXPECT_CALL(*g_mtaHALMock, mta_hal_GetDHCPV6Info(testing::_))
        .WillOnce(testing::Return(RETURN_OK));
    ULONG result = X_CISCO_COM_MTA_V6_GetParamStringValue(hInsContext, paramName, value, &ulSize);

    EXPECT_EQ(result, -1);
}


// Success case for "SecondaryDNS"
TEST_F(CcspMtaTestFixture, X_CISCO_COM_MTA_V6_GetParamStringValue_SecondaryDNS_Success) {
    ANSC_HANDLE hInsContext = nullptr;
    char paramName[] = "SecondaryDNS";
    char value[INET6_ADDRSTRLEN];
    ULONG ulSize = sizeof(value);

    _COSA_MTA_DHCPv6_INFO dhcpv6Info;
    strcpy(dhcpv6Info.SecondaryDNS, "2001:db8:1::1");

    EXPECT_CALL(*g_mtaHALMock, mta_hal_GetDHCPV6Info(testing::_))
         .WillOnce(testing::Return(RETURN_OK));
    ULONG result = X_CISCO_COM_MTA_V6_GetParamStringValue(hInsContext, paramName, value, &ulSize);

    EXPECT_EQ(result, 0);  
    
}

// Failure case for "SecondaryDNS"
TEST_F(CcspMtaTestFixture, X_CISCO_COM_MTA_V6_GetParamStringValue_SecondaryDNS_Failure) {
    ANSC_HANDLE hInsContext = nullptr;
    char paramName[] = "SecondaryDNS";
    char value[INET6_ADDRSTRLEN];
    ULONG ulSize = sizeof(value);

    EXPECT_CALL(*g_mtaHALMock, mta_hal_GetDHCPV6Info(testing::_))
        .WillOnce(testing::Return(RETURN_ERR));

    ULONG result = X_CISCO_COM_MTA_V6_GetParamStringValue(hInsContext, paramName, value, &ulSize);

    EXPECT_EQ(result, -1);  
}

// Success case for "DHCPOption3"
TEST_F(CcspMtaTestFixture, X_CISCO_COM_MTA_V6_GetParamStringValue_DHCPOption3_Success) {
    ANSC_HANDLE hInsContext = nullptr;
    char paramName[] = "DHCPOption3";
    char value[INET6_ADDRSTRLEN];
    ULONG ulSize = sizeof(value);

    _COSA_MTA_DHCPv6_INFO dhcpv6Info;
    strcpy(dhcpv6Info.DHCPOption3, "OptionValue3");

    EXPECT_CALL(*g_mtaHALMock, mta_hal_GetDHCPV6Info(testing::_))
         .WillOnce(testing::Return(RETURN_OK));

    ULONG result = X_CISCO_COM_MTA_V6_GetParamStringValue(hInsContext, paramName, value, &ulSize);
    EXPECT_EQ(result, 0);
    
}

// Failure case for "DHCPOption3"
TEST_F(CcspMtaTestFixture, X_CISCO_COM_MTA_V6_GetParamStringValue_DHCPOption3_Failure) {
    ANSC_HANDLE hInsContext = nullptr;
    char paramName[] = "DHCPOption3";
    char value[INET6_ADDRSTRLEN];
    ULONG ulSize = sizeof(value);

    EXPECT_CALL(*g_mtaHALMock, mta_hal_GetDHCPV6Info(testing::_))
        .WillOnce(testing::Return(RETURN_ERR));

    ULONG result = X_CISCO_COM_MTA_V6_GetParamStringValue(hInsContext, paramName, value, &ulSize);
    EXPECT_EQ(result, -1);
}

// Success case for "DHCPOption6"
TEST_F(CcspMtaTestFixture, X_CISCO_COM_MTA_V6_GetParamStringValue_DHCPOption6_Success) {
    ANSC_HANDLE hInsContext = nullptr;
    char paramName[] = "DHCPOption6";
    char value[INET6_ADDRSTRLEN];
    ULONG ulSize = sizeof(value);

    _COSA_MTA_DHCPv6_INFO dhcpv6Info;
    strcpy(dhcpv6Info.DHCPOption6, "OptionValue6");

    EXPECT_CALL(*g_mtaHALMock, mta_hal_GetDHCPV6Info(testing::_))
         .WillOnce(testing::Return(RETURN_OK));

    ULONG result = X_CISCO_COM_MTA_V6_GetParamStringValue(hInsContext, paramName, value, &ulSize);
    EXPECT_EQ(result, 0);
}

// Failure case for "DHCPOption6"
TEST_F(CcspMtaTestFixture, X_CISCO_COM_MTA_V6_GetParamStringValue_DHCPOption6_Failure) {
    ANSC_HANDLE hInsContext = nullptr;
    char paramName[] = "DHCPOption6";
    char value[INET6_ADDRSTRLEN];
    ULONG ulSize = sizeof(value);

    EXPECT_CALL(*g_mtaHALMock, mta_hal_GetDHCPV6Info(testing::_))
        .WillOnce(testing::Return(RETURN_ERR));

    ULONG result = X_CISCO_COM_MTA_V6_GetParamStringValue(hInsContext, paramName, value, &ulSize);
    EXPECT_EQ(result, -1);
}

TEST_F(CcspMtaTestFixture, X_CISCO_COM_MTA_V6_GetParamStringValue_PrimaryDHCPv6Server_Success) {
    ANSC_HANDLE hInsContext = nullptr;
    char paramName[] = "PrimaryDHCPv6Server";
    char value[INET6_ADDRSTRLEN];
    ULONG ulSize = sizeof(value);

    _COSA_MTA_DHCPv6_INFO dhcpv6Info;
    strcpy(dhcpv6Info.PrimaryDHCPv6Server, "2001:db8::1");

    EXPECT_CALL(*g_mtaHALMock, mta_hal_GetDHCPV6Info(testing::_))
        .WillOnce(testing::Return(RETURN_OK));

    ULONG result = X_CISCO_COM_MTA_V6_GetParamStringValue(hInsContext, paramName, value, &ulSize);
    EXPECT_EQ(result, 0);
}

// Failure case for "PrimaryDHCPv6Server"
TEST_F(CcspMtaTestFixture, X_CISCO_COM_MTA_V6_GetParamStringValue_PrimaryDHCPv6Server_Failure) {
    ANSC_HANDLE hInsContext = nullptr;
    char paramName[] = "PrimaryDHCPv6Server";
    char value[INET6_ADDRSTRLEN];
    ULONG ulSize = sizeof(value);

    EXPECT_CALL(*g_mtaHALMock, mta_hal_GetDHCPV6Info(testing::_))
        .WillOnce(testing::Return(RETURN_ERR));

    ULONG result = X_CISCO_COM_MTA_V6_GetParamStringValue(hInsContext, paramName, value, &ulSize);
    EXPECT_EQ(result, -1);
}

// Success case for "SecondaryDHCPv6Server"
TEST_F(CcspMtaTestFixture, X_CISCO_COM_MTA_V6_GetParamStringValue_SecondaryDHCPv6Server_Success) {
    ANSC_HANDLE hInsContext = nullptr;
    char paramName[] = "SecondaryDHCPv6Server";
    char value[INET6_ADDRSTRLEN];
    ULONG ulSize = sizeof(value);

    _COSA_MTA_DHCPv6_INFO dhcpv6Info;
    strcpy(dhcpv6Info.SecondaryDHCPv6Server, "2001:db8::2");

    EXPECT_CALL(*g_mtaHALMock, mta_hal_GetDHCPV6Info(testing::_))
        .WillOnce(testing::Return(RETURN_OK));
    ULONG result = X_CISCO_COM_MTA_V6_GetParamStringValue(hInsContext, paramName, value, &ulSize);
    EXPECT_EQ(result, 0);
}

// Failure case for "SecondaryDHCPv6Server"
TEST_F(CcspMtaTestFixture, X_CISCO_COM_MTA_V6_GetParamStringValue_SecondaryDHCPv6Server_Failure) {
    ANSC_HANDLE hInsContext = nullptr;
    char paramName[] = "SecondaryDHCPv6Server";
    char value[INET6_ADDRSTRLEN];
    ULONG ulSize = sizeof(value);

    EXPECT_CALL(*g_mtaHALMock, mta_hal_GetDHCPV6Info(testing::_))
        .WillOnce(testing::Return(RETURN_ERR));

    ULONG result = X_CISCO_COM_MTA_V6_GetParamStringValue(hInsContext, paramName, value, &ulSize);
    EXPECT_EQ(result, -1);
}

// Success case for "DHCPOption7"
TEST_F(CcspMtaTestFixture, X_CISCO_COM_MTA_V6_GetParamStringValue_DHCPOption7_Success) {
    ANSC_HANDLE hInsContext = nullptr;
    char paramName[] = "DHCPOption7";
    char value[INET6_ADDRSTRLEN];
    ULONG ulSize = sizeof(value);

    _COSA_MTA_DHCPv6_INFO dhcpv6Info;
    strcpy(dhcpv6Info.DHCPOption7, "OptionValue7");

    EXPECT_CALL(*g_mtaHALMock, mta_hal_GetDHCPV6Info(testing::_))
         .WillOnce(testing::Return(RETURN_OK));

    ULONG result = X_CISCO_COM_MTA_V6_GetParamStringValue(hInsContext, paramName, value, &ulSize);
    EXPECT_EQ(result, 0);
}

// Failure case for "DHCPOption7"
TEST_F(CcspMtaTestFixture, X_CISCO_COM_MTA_V6_GetParamStringValue_DHCPOption7_Failure) {
    ANSC_HANDLE hInsContext = nullptr;
    char paramName[] = "DHCPOption7";
    char value[INET6_ADDRSTRLEN];
    ULONG ulSize = sizeof(value);

    EXPECT_CALL(*g_mtaHALMock, mta_hal_GetDHCPV6Info(testing::_))
        .WillOnce(testing::Return(RETURN_ERR));

    ULONG result = X_CISCO_COM_MTA_V6_GetParamStringValue(hInsContext, paramName, value, &ulSize);
    EXPECT_EQ(result, -1);
}

// Success case for "DHCPOption8"
TEST_F(CcspMtaTestFixture, X_CISCO_COM_MTA_V6_GetParamStringValue_DHCPOption8_Success) {
    ANSC_HANDLE hInsContext = nullptr;
    char paramName[] = "DHCPOption8";
    char value[INET6_ADDRSTRLEN];
    ULONG ulSize = sizeof(value);

    _COSA_MTA_DHCPv6_INFO dhcpv6Info;
    strcpy(dhcpv6Info.DHCPOption8, "OptionValue8");

    EXPECT_CALL(*g_mtaHALMock, mta_hal_GetDHCPV6Info(testing::_))
         .WillOnce(testing::Return(RETURN_OK));

    ULONG result = X_CISCO_COM_MTA_V6_GetParamStringValue(hInsContext, paramName, value, &ulSize);
    EXPECT_EQ(result, 0);
}

// Failure case for "DHCPOption8"
TEST_F(CcspMtaTestFixture, X_CISCO_COM_MTA_V6_GetParamStringValue_DHCPOption8_Failure) {
    ANSC_HANDLE hInsContext = nullptr;
    char paramName[] = "DHCPOption8";
    char value[INET6_ADDRSTRLEN];
    ULONG ulSize = sizeof(value);

    EXPECT_CALL(*g_mtaHALMock, mta_hal_GetDHCPV6Info(testing::_))
        .WillOnce(testing::Return(RETURN_ERR));

    ULONG result = X_CISCO_COM_MTA_V6_GetParamStringValue(hInsContext, paramName, value, &ulSize);
    EXPECT_EQ(result, -1);
}

// Success case for "PCVersion"
TEST_F(CcspMtaTestFixture, X_CISCO_COM_MTA_V6_GetParamStringValue_PCVersion_Success) {
    ANSC_HANDLE hInsContext = nullptr;
    char paramName[] = "PCVersion";
    char value[INET6_ADDRSTRLEN];
    ULONG ulSize = sizeof(value);

    _COSA_MTA_DHCPv6_INFO dhcpv6Info;
    strcpy(dhcpv6Info.PCVersion, "1.0.0");

    EXPECT_CALL(*g_mtaHALMock, mta_hal_GetDHCPV6Info(testing::_))
        .WillOnce(testing::Return(RETURN_OK));

    ULONG result = X_CISCO_COM_MTA_V6_GetParamStringValue(hInsContext, paramName, value, &ulSize);
    EXPECT_EQ(result, 0);
}

// Failure case for "PCVersion"
TEST_F(CcspMtaTestFixture, X_CISCO_COM_MTA_V6_GetParamStringValue_PCVersion_Failure) {
    ANSC_HANDLE hInsContext = nullptr;
    char paramName[] = "PCVersion";
    char value[INET6_ADDRSTRLEN];
    ULONG ulSize = sizeof(value);

    EXPECT_CALL(*g_mtaHALMock, mta_hal_GetDHCPV6Info(testing::_))
        .WillOnce(testing::Return(RETURN_ERR));

    ULONG result = X_CISCO_COM_MTA_V6_GetParamStringValue(hInsContext, paramName, value, &ulSize);
    EXPECT_EQ(result, -1);
}

// Success case for "MACAddress"
TEST_F(CcspMtaTestFixture, X_CISCO_COM_MTA_V6_GetParamStringValue_MACAddress_Success) {
    ANSC_HANDLE hInsContext = nullptr;
    char paramName[] = "MACAddress";
    char value[INET6_ADDRSTRLEN];
    ULONG ulSize = sizeof(value);

    _COSA_MTA_DHCPv6_INFO dhcpv6Info;
    strcpy(dhcpv6Info.MACAddress, "00:1A:2B:3C:4D:5E");

    EXPECT_CALL(*g_mtaHALMock, mta_hal_GetDHCPV6Info(testing::_))
        .WillOnce(testing::Return(RETURN_OK));

    ULONG result = X_CISCO_COM_MTA_V6_GetParamStringValue(hInsContext, paramName, value, &ulSize);
    EXPECT_EQ(result, 0);
}

// Failure case for "MACAddress"
TEST_F(CcspMtaTestFixture, X_CISCO_COM_MTA_V6_GetParamStringValue_MACAddress_Failure) {
    ANSC_HANDLE hInsContext = nullptr;
    char paramName[] = "MACAddress";
    char value[INET6_ADDRSTRLEN];
    ULONG ulSize = sizeof(value);

    EXPECT_CALL(*g_mtaHALMock, mta_hal_GetDHCPV6Info(testing::_))
        .WillOnce(testing::Return(RETURN_ERR));

    ULONG result = X_CISCO_COM_MTA_V6_GetParamStringValue(hInsContext, paramName, value, &ulSize);
    EXPECT_EQ(result, -1);
}

//LineTable_GetParamBoolValue
TEST_F(CcspMtaTestFixture, LineTable_GetParamBoolValue_TriggerDiagnostics_Success) {
    ANSC_HANDLE hInsContext = nullptr;
    char paramName[] = "TriggerDiagnostics";
    BOOL value;
    int comparisonResult = 0;  

    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(testing::StrEq("TriggerDiagnostics"), testing::_, testing::StrEq(paramName), testing::_, testing::_, testing::_))
        .WillOnce(testing::DoAll(testing::SetArgPointee<3>(comparisonResult), testing::Return(0)));  // Use SetArgPointee to set the value pointed by the 4th argument

    BOOL result = LineTable_GetParamBoolValue(hInsContext, paramName, &value);

  
    EXPECT_TRUE(result);

    
}

TEST_F(CcspMtaTestFixture, LineTable_GetParamBoolValue_UnsupportedParameter_Failure) {
    ANSC_HANDLE hInsContext = nullptr;
    char paramName[] = "UnsupportedParam";
    BOOL value;
    int comparisonResult = 1;  

    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(testing::StrEq("TriggerDiagnostics"), testing::_, testing::StrEq(paramName), testing::_, testing::_, testing::_))
        .WillOnce(testing::DoAll(testing::SetArgPointee<3>(comparisonResult), testing::Return(0)));  // SetArgPointee for int*

  
    BOOL result = LineTable_GetParamBoolValue(hInsContext, paramName, &value);

   
    EXPECT_FALSE(result);
}

//VQM_GetParamBoolValue
TEST_F(CcspMtaTestFixture, VQM_GetParamBoolValue_ResetStats_Success) {
    ANSC_HANDLE hInsContext = nullptr;
    char paramName[] = "ResetStats";
    BOOL value;
    int ind = 0;

    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(testing::StrEq("ResetStats"), testing::_, testing::StrEq(paramName), testing::_, testing::_, testing::_))
        .WillOnce(testing::DoAll(testing::SetArgPointee<3>(ind), testing::Return(0)));  // Use SetArgPointee to set the value pointed by the 4th argument

    BOOL result = VQM_GetParamBoolValue(hInsContext, paramName, &value);

    EXPECT_TRUE(result);  
}


TEST_F(CcspMtaTestFixture, VQM_GetParamBoolValue_UnsupportedParam_Failure) {
    ANSC_HANDLE hInsContext = nullptr;
    char paramName[] = "UnsupportedParam";
    BOOL value;
    int ind = 1;

    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(testing::StrEq("ResetStats"), testing::_, testing::StrEq(paramName), testing::_, testing::_, testing::_))
        .WillOnce(testing::DoAll(testing::SetArgPointee<3>(ind), testing::Return(0)));  // SetArgPointee for int*


    BOOL result = VQM_GetParamBoolValue(hInsContext, paramName, &value);

    EXPECT_FALSE(result);  
}

//ServiceFlow_GetParamBoolValue
TEST_F(CcspMtaTestFixture, ServiceFlow_GetParamBoolValue_DefaultFlow_Success) {
    PCOSA_MTA_SERVICE_FLOW pFlow = new COSA_MTA_SERVICE_FLOW;
    ANSC_HANDLE hInsContext = (ANSC_HANDLE)pFlow;
    char paramName[] = "DefaultFlow";
    BOOL value;
    int ind = 0;

    pFlow->DefaultFlow = TRUE;

    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(testing::StrEq("DefaultFlow"), testing::_, testing::StrEq(paramName), testing::_, testing::_, testing::_))
        .WillOnce(testing::DoAll(testing::SetArgPointee<3>(ind), testing::Return(0)));  // Simulate a successful string comparison (EOK)

    BOOL result = ServiceFlow_GetParamBoolValue(hInsContext, paramName, &value);

    EXPECT_TRUE(result);  
    EXPECT_TRUE(value);   
    delete pFlow;         
}

TEST_F(CcspMtaTestFixture, ServiceFlow_GetParamBoolValue_UnsupportedParam_Failure) {
    // Set up the context and parameter values
    PCOSA_MTA_SERVICE_FLOW pFlow = new COSA_MTA_SERVICE_FLOW;
    ANSC_HANDLE hInsContext = (ANSC_HANDLE)pFlow;
    char paramName[] = "UnsupportedParam";
    BOOL value;
    int ind = 1;

    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(testing::StrEq("DefaultFlow"), testing::_, testing::StrEq(paramName), testing::_, testing::_, testing::_))
        .WillOnce(testing::DoAll(testing::SetArgPointee<3>(ind), testing::Return(0)));  // Simulate a failed string comparison (unsupported)


    BOOL result = ServiceFlow_GetParamBoolValue(hInsContext, paramName, &value);

    // Verify the results
    EXPECT_FALSE(result);  
    delete pFlow;         
}

//X_RDKCENTRAL_COM_MTA_GetParamBoolValue
TEST_F(CcspMtaTestFixture, X_RDKCENTRAL_COM_MTA_GetParamBoolValue_PktcMtaDevResetNow_Success) {
    ANSC_HANDLE hInsContext = nullptr; 
    char paramName[] = "pktcMtaDevResetNow";
    BOOL value;
    int ind = 0;

    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(testing::StrEq("pktcMtaDevResetNow"), testing::_, testing::StrEq(paramName), testing::_, testing::_, testing::_))
        .WillOnce(testing::DoAll(testing::SetArgPointee<3>(ind), testing::Return(0)));  // Simulate a successful string comparison (EOK)

    BOOL result = X_RDKCENTRAL_COM_MTA_GetParamBoolValue(hInsContext, paramName, &value);

    EXPECT_TRUE(result);  
    EXPECT_FALSE(value);  
}

TEST_F(CcspMtaTestFixture, X_RDKCENTRAL_COM_MTA_GetParamBoolValue_UnsupportedParam_Failure) {
    ANSC_HANDLE hInsContext = nullptr; 
    char paramName[] = "UnsupportedParam";
    BOOL value;
    int ind = 1;  

    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(testing::StrEq("pktcMtaDevResetNow"), testing::_, testing::StrEq(paramName), testing::_, testing::_, testing::_))
        .WillOnce(testing::DoAll(testing::SetArgPointee<3>(ind), testing::Return(0)));  // Simulate a failed string comparison (unsupported)

    BOOL result = X_RDKCENTRAL_COM_MTA_GetParamBoolValue(hInsContext, paramName, &value);

    EXPECT_FALSE(result);  
}

//DSXLog_GetParamUlongValue
TEST_F(CcspMtaTestFixture, DSXLog_GetParamUlongValue_ID_Success) {
    ANSC_HANDLE hInsContext = nullptr; 
    char paramName[] = "ID";
    ULONG value;
    int ind = 0;
    PCOSA_MTA_DSXLOG pDSXLog = new COSA_MTA_DSXLOG(); 
    pDSXLog->ID = 12345; 

    hInsContext = reinterpret_cast<ANSC_HANDLE>(pDSXLog);

    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(testing::StrEq("ID"), testing::_, testing::StrEq(paramName), testing::_, testing::_, testing::_))
        .WillOnce(testing::DoAll(testing::SetArgPointee<3>(ind), testing::Return(0)));  
    BOOL result = DSXLog_GetParamUlongValue(hInsContext, paramName, &value);

    EXPECT_TRUE(result);  
    EXPECT_EQ(value, 12345);  
    delete pDSXLog; 
}

TEST_F(CcspMtaTestFixture, DSXLog_GetParamUlongValue_Level_Success) {
    ANSC_HANDLE hInsContext = nullptr; 
    char paramName[] = "Level";
    ULONG value;
    int ind = 0;
    PCOSA_MTA_DSXLOG pDSXLog = new COSA_MTA_DSXLOG(); 
    pDSXLog->Level = 5; 

    
    hInsContext = reinterpret_cast<ANSC_HANDLE>(pDSXLog);

    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(testing::StrEq("ID"), testing::_, testing::StrEq(paramName), testing::_, testing::_, testing::_))
        .WillOnce(testing::DoAll(testing::SetArgPointee<3>(1), testing::Return(0)));  

    
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(testing::StrEq("Level"), testing::_, testing::StrEq(paramName), testing::_, testing::_, testing::_))
        .WillOnce(testing::DoAll(testing::SetArgPointee<3>(ind), testing::Return(0)));  

    
    BOOL result = DSXLog_GetParamUlongValue(hInsContext, paramName, &value);

    
    EXPECT_TRUE(result);  
    EXPECT_EQ(value, 5);  
    delete pDSXLog; 
}

TEST_F(CcspMtaTestFixture, DSXLog_GetParamUlongValue_UnsupportedParam_Failure) {
    ANSC_HANDLE hInsContext = nullptr; 
    char paramName[] = "UnsupportedParam";
    ULONG value;
    int ind = 1;  
    PCOSA_MTA_DSXLOG pDSXLog = new COSA_MTA_DSXLOG(); 

  
    hInsContext = reinterpret_cast<ANSC_HANDLE>(pDSXLog);

    
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(testing::StrEq("ID"), testing::_, testing::StrEq(paramName), testing::_, testing::_, testing::_))
        .WillOnce(testing::DoAll(testing::SetArgPointee<3>(ind), testing::Return(0)));  // Simulate a failed string comparison for ID

    
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(testing::StrEq("Level"), testing::_, testing::StrEq(paramName), testing::_, testing::_, testing::_))
        .WillOnce(testing::DoAll(testing::SetArgPointee<3>(ind), testing::Return(0)));  // Simulate a failed string comparison for Level

    
    BOOL result = DSXLog_GetParamUlongValue(hInsContext, paramName, &value);

    
    EXPECT_FALSE(result);  
    delete pDSXLog; 
}

//MTALog_GetParamUlongValue
TEST_F(CcspMtaTestFixture, MTALog_GetParamUlongValue_Index_Success) {
    ANSC_HANDLE hInsContext = nullptr; 
    char paramName[] = "Index";
    ULONG value;
    int ind = 0;
    PCOSA_DML_MTALOG_FULL pConf = new COSA_DML_MTALOG_FULL(); 
    pConf->Index = 42; 

    hInsContext = reinterpret_cast<ANSC_HANDLE>(pConf);

    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(testing::_, testing::_, testing::_ , testing::_, testing::_, testing::_))
        .Times(1)
        .WillOnce(testing::DoAll(testing::SetArgPointee<3>(ind), testing::Return(0))); 

    BOOL result = MTALog_GetParamUlongValue(hInsContext, paramName, &value);

    EXPECT_TRUE(result); 
    delete pConf; 
} 

TEST_F(CcspMtaTestFixture, MTALog_GetParamUlongValue_EventID_Success) {
    ANSC_HANDLE hInsContext = nullptr; 
    char paramName[] = "EventID";
    ULONG value;
    int ind = 0;
    PCOSA_DML_MTALOG_FULL pConf = new COSA_DML_MTALOG_FULL(); 
    pConf->EventID = 98765; 

    hInsContext = reinterpret_cast<ANSC_HANDLE>(pConf);

    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(testing::StrEq("Index"), testing::_, testing::StrEq(paramName), testing::_, testing::_, testing::_))
        .WillOnce(testing::DoAll(testing::SetArgPointee<3>(1), testing::Return(0)));  // Simulate a failed string comparison for ID

    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(testing::StrEq("EventID"), testing::_, testing::StrEq(paramName), testing::_, testing::_, testing::_))
        .WillOnce(testing::DoAll(testing::SetArgPointee<3>(ind), testing::Return(0)));  // Simulate a failed string comparison for Level
    BOOL result = MTALog_GetParamUlongValue(hInsContext, paramName, &value);

    EXPECT_TRUE(result); 
    delete pConf; 
}

TEST_F(CcspMtaTestFixture, MTALog_GetParamUlongValue_UnsupportedParam_Failure) {
    ANSC_HANDLE hInsContext = nullptr; 
    char paramName[] = "UnsupportedParam";
    ULONG value;
    int ind = 1; 
    PCOSA_DML_MTALOG_FULL pConf = new COSA_DML_MTALOG_FULL(); 

    hInsContext = reinterpret_cast<ANSC_HANDLE>(pConf);

    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(
        testing::StrEq("Index"),
        testing::_,
        testing::StrEq(paramName),
        testing::_,
        testing::_,
        testing::_))
    .WillOnce(testing::DoAll(testing::SetArgPointee<3>(ind), testing::Return(0))); 

    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(
        testing::StrEq("EventID"),
        testing::_,
        testing::StrEq(paramName),
        testing::_,
        testing::_,
        testing::_))
    .WillOnce(testing::DoAll(testing::SetArgPointee<3>(ind), testing::Return(0))); 

    BOOL result = MTALog_GetParamUlongValue(hInsContext, paramName, &value);

    EXPECT_FALSE(result); 
    delete pConf; 
}

//Battery_GetParamBoolValue
TEST_F(CcspMtaTestFixture, GetParamBoolValue_Success) {
    BOOLEAN installed = false;
    
   
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(testing::StrEq("Installed"), 9, testing::StrEq("Installed"), testing::_, testing::_, testing::_))
        .WillOnce(testing::DoAll(testing::SetArgPointee<3>(0), testing::Return(EOK)));

    
    EXPECT_CALL(*g_mtaHALMock, mta_hal_BatteryGetInstalled(testing::_))
        .WillOnce(testing::DoAll(testing::SetArgPointee<0>(true), testing::Return(RETURN_OK)));

    BOOL success = Battery_GetParamBoolValue(NULL, "Installed", &installed);

   
    EXPECT_TRUE(success); 
    EXPECT_TRUE(installed);  
}

TEST_F(CcspMtaTestFixture, GetParamBoolValue_Failure) {
    BOOLEAN installed = false;
    
    
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(testing::StrEq("Installed"), 9, testing::StrEq("Installed"), testing::_, testing::_, testing::_))
        .WillOnce(testing::DoAll(testing::SetArgPointee<3>(0), testing::Return(EOK)));

   
    EXPECT_CALL(*g_mtaHALMock, mta_hal_BatteryGetInstalled(testing::_))
        .WillOnce(testing::Return(1));  

    BOOL success = Battery_GetParamBoolValue(NULL, "Installed", &installed);

    // Assertions
    EXPECT_FALSE(success);  
    EXPECT_FALSE(installed); 
}

// Test case for unsupported parameter
TEST_F(CcspMtaTestFixture, GetParamBoolValue_UnsupportedParam) {
    BOOLEAN installed = false;

    
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(testing::StrEq("Installed"), 9, testing::StrEq("UnsupportedParam"), testing::_, testing::_, testing::_))
        .WillOnce(testing::DoAll(testing::SetArgPointee<3>(1), testing::Return(EOK)));

    BOOL success = Battery_GetParamBoolValue(NULL, "UnsupportedParam", &installed);

    // Assertions
    EXPECT_FALSE(success);  
}

//LineTable_SetParamBoolValue
TEST_F(CcspMtaTestFixture, SetParamBoolValue_TriggerDiagnosticsTrue) {
    
    PCOSA_MTA_LINETABLE_INFO pInfo = (PCOSA_MTA_LINETABLE_INFO)malloc(sizeof(COSA_MTA_LINETABLE_INFO));
    pInfo->InstanceNumber = 2;  
    ULONG nIndex = pInfo->InstanceNumber - 1;

    
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_, _, _, _, _, _))
        .WillOnce(testing::DoAll(testing::SetArgPointee<3>(0), testing::Return(EOK)));  

   
    EXPECT_CALL(*g_mtaHALMock, mta_hal_TriggerDiagnostics(nIndex))
        .WillOnce(testing::Return(RETURN_OK));

    
    BOOL result = LineTable_SetParamBoolValue((ANSC_HANDLE)pInfo, "TriggerDiagnostics", TRUE);

  
    ASSERT_TRUE(result);


    free(pInfo);
}


TEST_F(CcspMtaTestFixture, SetParamBoolValue_UnsupportedParameter) {
    
    PCOSA_MTA_LINETABLE_INFO pInfo = (PCOSA_MTA_LINETABLE_INFO)malloc(sizeof(COSA_MTA_LINETABLE_INFO));

    
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_, _, _, _, _, _))
        .WillOnce(testing::DoAll(testing::SetArgPointee<3>(1), testing::Return(EOK)));  

    
    BOOL result = LineTable_SetParamBoolValue((ANSC_HANDLE)pInfo, "UnsupportedParam", TRUE);

    
    ASSERT_FALSE(result);

   
    free(pInfo);
}

//Calls_IsUpdated
TEST_F(CcspMtaTestFixture, WhenFirstTime_ShouldReturnTrue) {
    PCOSA_MTA_LINETABLE_INFO pMyObject = new COSA_MTA_LINETABLE_INFO; 
    pMyObject->CallsUpdateTime = 0; 

    
    EXPECT_CALL(*g_usertimeMock, UserGetTickInSeconds2())
        .WillOnce(::testing::Return(0)); 

    BOOL result = Calls_IsUpdated((ANSC_HANDLE)pMyObject);
    EXPECT_TRUE(result); 

    delete pMyObject; 
    
}
TEST_F(CcspMtaTestFixture, WhenAfterRefreshInterval_ShouldReturnTrue) {
    
    PCOSA_MTA_LINETABLE_INFO pMyObject = new COSA_MTA_LINETABLE_INFO;

    
    pMyObject->CallsUpdateTime = 100;

   
    EXPECT_CALL(*g_usertimeMock, UserGetTickInSeconds2())
        .Times(3) 
        .WillOnce(Return(300)) 
        .WillOnce(Return(300))  
        .WillOnce(Return(400)); 

    
    BOOL result = Calls_IsUpdated((ANSC_HANDLE)pMyObject);

    
    EXPECT_TRUE(result);

  
    delete pMyObject;

}


TEST_F(CcspMtaTestFixture, WhenWithinRefreshInterval_ShouldReturnFalse) {
   
    PCOSA_MTA_LINETABLE_INFO pMyObject = new COSA_MTA_LINETABLE_INFO;

    
    pMyObject->CallsUpdateTime = 300;

    
    EXPECT_CALL(*g_usertimeMock, UserGetTickInSeconds2())
        .WillOnce(Return(350))   
        .WillOnce(Return(350));  

   
    BOOL result = Calls_IsUpdated((ANSC_HANDLE)pMyObject);

    
    EXPECT_FALSE(result);

  
    delete pMyObject;
   
}

//VQM_SetParamBoolValue
TEST_F(CcspMtaTestFixture, TestResetStats) {
    const char* paramName = "ResetStats";
    int ind = -1; 

    
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk("ResetStats", strlen("ResetStats"), paramName, _, _, _))
        .WillOnce(DoAll(
            SetArgPointee<3>(0), 
            Return(EOK)
        ));

   
    BOOL result = VQM_SetParamBoolValue(nullptr, const_cast<char*>(paramName), TRUE);

    
    EXPECT_TRUE(result); 
}

TEST_F(CcspMtaTestFixture, TestUnsupportedParameter) {
    const char* paramName = "UnsupportedParam"; 
    int ind = -1; 

    
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk("ResetStats", strlen("ResetStats"), paramName, _, _, _))
        .WillOnce(DoAll(
            SetArgPointee<3>(1), 
            Return(EOK)         
        ));

    // Call the function to be tested
    BOOL result = VQM_SetParamBoolValue(nullptr, const_cast<char*>(paramName), TRUE);

    
    EXPECT_FALSE(result);
}

//getRequestorString
TEST_F(CcspMtaTestFixture, ShouldReturnWebPAWhenEntityIsWebPA) {
   
    g_currentWriteEntity = 0x0A;

    
    char* result = getRequestorString();

    
    EXPECT_STREQ(result, "webpa");
}

TEST_F(CcspMtaTestFixture, ShouldReturnWebPAWhenEntityIsWebPAB) {
    
    g_currentWriteEntity = 0x0B;

   
    char* result = getRequestorString();

    
    EXPECT_STREQ(result, "webpa");
}

TEST_F(CcspMtaTestFixture, ShouldReturnWebPAWhenEntityIsrfc8) {
    
    g_currentWriteEntity = 0x08;

    
    char* result = getRequestorString();

    
    EXPECT_STREQ(result, "rfc");
}

TEST_F(CcspMtaTestFixture, ShouldReturnWebPAWhenEntityIsrfc10) {
    
    g_currentWriteEntity = 0x10;

    
    char* result = getRequestorString();

    
    EXPECT_STREQ(result, "rfc");
}

TEST_F(CcspMtaTestFixture, ShouldReturnUnknownForOtherEntities) {
    
    g_currentWriteEntity = 0x99; 

    
    char* result = getRequestorString();

    
    EXPECT_STREQ(result, "unknown");
}

//VoiceService_GetParamStringValue
TEST_F(CcspMtaTestFixture, ShouldReturnZeroAndEmptyStringForDataParam) {
    // Arrange
    char pValue[256];  
    ULONG ulSize = sizeof(pValue); 
    ANSC_HANDLE hInsContext = nullptr; 

    // Act
    ULONG result = VoiceService_GetParamStringValue(hInsContext, "Data", pValue, &ulSize);

    // Assert
    EXPECT_EQ(result, 0);                       
    EXPECT_STREQ(pValue, "");                  
    EXPECT_EQ(ulSize, sizeof(pValue));          
}

TEST_F(CcspMtaTestFixture, ShouldReturnMinusOneForInvalidParam) {
    
    char pValue[256];  
    ULONG ulSize = sizeof(pValue);
    ANSC_HANDLE hInsContext = nullptr;

    // Act
    ULONG result = VoiceService_GetParamStringValue(hInsContext, "InvalidParam", pValue, &ulSize);

    // Assert
    EXPECT_EQ(result, -1);                       
}

//Calls_GetParamBoolValue
TEST_F(CcspMtaTestFixture, Calls_GetParamBoolValueJitter_Success) {
    ANSC_HANDLE hInsContext = nullptr; 
    char paramName[] = "JitterBufferAdaptive";
    BOOL boolValue = FALSE;
    int ind = 0;
    PCOSA_MTA_CALLS pCalls = new COSA_MTA_CALLS(); 
    pCalls->JitterBufferAdaptive = TRUE ;

    hInsContext = reinterpret_cast<ANSC_HANDLE>(pCalls);

    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(testing::_, testing::_, testing::_ , testing::_, testing::_, testing::_))
        .Times(1)
        .WillOnce(testing::DoAll(testing::SetArgPointee<3>(ind), testing::Return(0))); 

    BOOL result = Calls_GetParamBoolValue(hInsContext, paramName, &boolValue);

    EXPECT_TRUE(result); 
   
    delete pCalls; 
} 

TEST_F(CcspMtaTestFixture, Calls_GetParamBoolValueorginator_Success) {
    ANSC_HANDLE hInsContext = nullptr; 
    char paramName[] = "Originator";
     BOOL boolValue = FALSE;
    int ind = 0;
     PCOSA_MTA_CALLS pCalls = new COSA_MTA_CALLS(); 
    pCalls->Originator = TRUE; 

    hInsContext = reinterpret_cast<ANSC_HANDLE>(pCalls);

    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(testing::StrEq("JitterBufferAdaptive"), testing::_, testing::StrEq(paramName), testing::_, testing::_, testing::_))
        .WillOnce(testing::DoAll(testing::SetArgPointee<3>(1), testing::Return(0)));  // Simulate a failed string comparison for ID

    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(testing::StrEq("Originator"), testing::_, testing::StrEq(paramName), testing::_, testing::_, testing::_))
        .WillOnce(testing::DoAll(testing::SetArgPointee<3>(ind), testing::Return(0)));  // Simulate a failed string comparison for Level
    BOOL result = Calls_GetParamBoolValue(hInsContext, paramName, &boolValue);

    EXPECT_TRUE(result); 
    delete pCalls; 
}

TEST_F(CcspMtaTestFixture, Calls_GetParamBoolValueRemotejitter_Success) {
    ANSC_HANDLE hInsContext = nullptr; 
    char paramName[] = "RemoteJitterBufferAdaptive";
     BOOL boolValue = FALSE;
    int ind = 0;
     PCOSA_MTA_CALLS pCalls = new COSA_MTA_CALLS(); 
    pCalls->RemoteJitterBufferAdaptive = TRUE; 

    hInsContext = reinterpret_cast<ANSC_HANDLE>(pCalls);

    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(testing::StrEq("JitterBufferAdaptive"), testing::_, testing::StrEq(paramName), testing::_, testing::_, testing::_))
        .WillOnce(testing::DoAll(testing::SetArgPointee<3>(1), testing::Return(0)));  // Simulate a failed string comparison for ID

    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(testing::StrEq("Originator"), testing::_, testing::StrEq(paramName), testing::_, testing::_, testing::_))
        .WillOnce(testing::DoAll(testing::SetArgPointee<3>(2), testing::Return(0)));  // Simulate a failed string comparison for Level
   
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(testing::StrEq("RemoteJitterBufferAdaptive"), testing::_, testing::StrEq(paramName), testing::_, testing::_, testing::_))
        .WillOnce(testing::DoAll(testing::SetArgPointee<3>(ind), testing::Return(0)));  // Simulate a failed string comparison for Level
    BOOL result = Calls_GetParamBoolValue(hInsContext, paramName, &boolValue);
    
    

    EXPECT_TRUE(result); 
    delete pCalls; 
}


TEST_F(CcspMtaTestFixture, Calls_GetParamBoolValueOther_unspported) {
    ANSC_HANDLE hInsContext = nullptr; 
    char paramName[] = "UnsupportedParam";
     BOOL boolValue = FALSE;
    int ind = 1;
     PCOSA_MTA_CALLS pCalls = new COSA_MTA_CALLS();

    hInsContext = reinterpret_cast<ANSC_HANDLE>(pCalls);

    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(testing::StrEq("JitterBufferAdaptive"), testing::_, testing::StrEq(paramName), testing::_, testing::_, testing::_))
        .WillOnce(testing::DoAll(testing::SetArgPointee<3>(ind), testing::Return(0)));  // Simulate a failed string comparison for ID

    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(testing::StrEq("Originator"), testing::_, testing::StrEq(paramName), testing::_, testing::_, testing::_))
        .WillOnce(testing::DoAll(testing::SetArgPointee<3>(ind), testing::Return(0)));  // Simulate a failed string comparison for Level
   
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(testing::StrEq("RemoteJitterBufferAdaptive"), testing::_, testing::StrEq(paramName), testing::_, testing::_, testing::_))
        .WillOnce(testing::DoAll(testing::SetArgPointee<3>(ind), testing::Return(0)));  // Simulate a failed string comparison for Level
    BOOL result = Calls_GetParamBoolValue(hInsContext, paramName, &boolValue);
    
    

    EXPECT_FALSE(result);
    delete pCalls; 
}

//Calls_GetParamUlongValue
TEST_F(CcspMtaTestFixture, Calls_GetParamUlongValue_RemoteIPAddress_Success) {
    ANSC_HANDLE hInsContext = nullptr; 
    char paramName[] = "RemoteIPAddress";
    ULONG value;
    int ind = 0;
    PCOSA_MTA_CALLS pCalls = new COSA_MTA_CALLS(); 
    inet_pton(AF_INET, "192.168.0.1", &(pCalls->RemoteIPAddress.Value));
    hInsContext = reinterpret_cast<ANSC_HANDLE>(pCalls);

    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(testing::_, testing::_, testing::_ , testing::_, testing::_, testing::_))
        .Times(1)
        .WillOnce(testing::DoAll(testing::SetArgPointee<3>(ind), testing::Return(0))); 

    BOOL result = Calls_GetParamUlongValue(hInsContext, paramName, &value);

    EXPECT_TRUE(result); 
    delete pCalls; 
}

TEST_F(CcspMtaTestFixture, Calls_GetParamUlongValue_CallDuration_Success) {
    ANSC_HANDLE hInsContext = nullptr; 
    char paramName[] = "CallDuration";
    ULONG value;
    int ind = 0;
    PCOSA_MTA_CALLS pCalls = new COSA_MTA_CALLS(); 
    pCalls->CallDuration = 42; 

    hInsContext = reinterpret_cast<ANSC_HANDLE>(pCalls);
 EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(testing::StrEq("RemoteIPAddress"), testing::_, testing::StrEq(paramName), testing::_, testing::_, testing::_))
        .WillOnce(testing::DoAll(testing::SetArgPointee<3>(1), testing::Return(0)));  // Simulate a failed string comparison for ID

    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(testing::StrEq("CallDuration"), testing::_, testing::StrEq(paramName), testing::_, testing::_, testing::_))
        .WillOnce(testing::DoAll(testing::SetArgPointee<3>(ind), testing::Return(0)));  // Simulate a failed string comparison for Level

    BOOL result = Calls_GetParamUlongValue(hInsContext, paramName, &value);

    EXPECT_TRUE(result); 
   
    delete pCalls; 
}

TEST_F(CcspMtaTestFixture, Calls_GetParamUlongValue_unsupported) {
    ANSC_HANDLE hInsContext = nullptr; 
    char paramName[] = "unsupported";
    ULONG value;
    int ind = 1; 

    PCOSA_MTA_CALLS pCalls = new COSA_MTA_CALLS(); 
    hInsContext = reinterpret_cast<ANSC_HANDLE>(pCalls);

    
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(
        testing::StrEq("RemoteIPAddress"),
        testing::_,
        testing::StrEq(paramName),
        testing::_,
        testing::_,
        testing::_))
        .WillOnce(testing::DoAll(testing::SetArgPointee<3>(ind), testing::Return(0))); // Simulate mismatch

    // Mock for "CallDuration"
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(
        testing::StrEq("CallDuration"),
        testing::_,
        testing::StrEq(paramName),
        testing::_,
        testing::_,
        testing::_))
        .WillOnce(testing::DoAll(testing::SetArgPointee<3>(ind), testing::Return(0))); // Simulate mismatch

    
    BOOL result = Calls_GetParamUlongValue(hInsContext, paramName, &value);

   
    EXPECT_FALSE(result);

    
    delete pCalls;
}

//Dect_GetParamUlongValue
TEST_F(CcspMtaTestFixture, Dect_GetParamUlongValue_RegisterDectHandset_Success) {
    ANSC_HANDLE hInsContext = nullptr;  
    char paramName[] = "RegisterDectHandset";
    ULONG ulongValue = 42;  
    errno_t rc = EOK;
    int ind = 0;

    
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(testing::_, testing::_, testing::_ , testing::_, testing::_, testing::_))
        .Times(1)
        .WillOnce(testing::DoAll(testing::SetArgPointee<3>(ind), testing::Return(0))); 

    BOOL result = Dect_GetParamUlongValue(hInsContext, paramName, &ulongValue);

    EXPECT_TRUE(result);
    EXPECT_EQ(ulongValue, 0);  
}

// Test for DeregisterDectHandset - success case
TEST_F(CcspMtaTestFixture, Dect_GetParamUlongValue_DeregisterDectHandset_Success) {
    ANSC_HANDLE hInsContext = nullptr;  
    char paramName[] = "DeregisterDectHandset";
    ULONG ulongValue = 42;  
    errno_t rc = EOK;
    int ind = 0;

   
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(testing::StrEq("RegisterDectHandset"), testing::_, testing::StrEq(paramName), testing::_, testing::_, testing::_))
        .WillOnce(testing::DoAll(testing::SetArgPointee<3>(1), testing::Return(rc)));  // Simulate successful comparison
    
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(testing::StrEq("DeregisterDectHandset"), testing::_, testing::StrEq(paramName), testing::_, testing::_, testing::_))
        .WillOnce(testing::DoAll(testing::SetArgPointee<3>(ind), testing::Return(rc)));  // Simulate successful comparison

    BOOL result = Dect_GetParamUlongValue(hInsContext, paramName, &ulongValue);

    EXPECT_TRUE(result);
    EXPECT_EQ(ulongValue, 0);  
}

// Test for unsupported parameter
TEST_F(CcspMtaTestFixture, Dect_GetParamUlongValue_UnsupportedParam) {
    ANSC_HANDLE hInsContext = nullptr;  
    char paramName[] = "UnsupportedParam";
    ULONG ulongValue = 42;  
    errno_t rc = EOK;
    int ind = 1;  

   
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(testing::StrEq("RegisterDectHandset"), testing::_, testing::StrEq(paramName), testing::_, testing::_, testing::_))
        .WillOnce(testing::DoAll(testing::SetArgPointee<3>(ind), testing::Return(rc)));  // Simulate failed comparison

    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(testing::StrEq("DeregisterDectHandset"), testing::_, testing::StrEq(paramName), testing::_, testing::_, testing::_))
        .WillOnce(testing::DoAll(testing::SetArgPointee<3>(ind), testing::Return(rc)));  // Simulate failed comparison

    BOOL result = Dect_GetParamUlongValue(hInsContext, paramName, &ulongValue);

    EXPECT_FALSE(result);
    EXPECT_EQ(ulongValue, 42); 
}


//Dect_SetParamStringValue
TEST_F(CcspMtaTestFixture, Dect_SetParamStringValue_PIN_Success) {
    ANSC_HANDLE hInsContext = nullptr;
    char paramName[] = "PIN";
    char pinString[] = "1234";
    int ind = 0;

   
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(testing::StrEq("PIN"), testing::_, testing::StrEq(paramName), testing::_, testing::_, testing::_))
        .WillOnce(testing::DoAll(testing::SetArgPointee<3>(ind), testing::Return(EOK)));

    
    EXPECT_CALL(*g_mtaHALMock, mta_hal_SetDectPIN(testing::StrEq(pinString)))
        .WillOnce(testing::Return(RETURN_OK));

    BOOL result = Dect_SetParamStringValue(hInsContext, paramName, pinString);

    EXPECT_TRUE(result);
}

TEST_F(CcspMtaTestFixture, Dect_SetParamStringValue_UnsupportedParam) {
    ANSC_HANDLE hInsContext = nullptr;
    char paramName[] = "UnsupportedParam";
    char pinString[] = "1234";
    int ind = 1;

    
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(testing::StrEq("PIN"), testing::_, testing::StrEq(paramName), testing::_, testing::_, testing::_))
        .WillOnce(testing::DoAll(testing::SetArgPointee<3>(ind), testing::Return(EOK)));

    
    EXPECT_CALL(*g_mtaHALMock, mta_hal_SetDectPIN(testing::_)).Times(0);

    BOOL result = Dect_SetParamStringValue(hInsContext, paramName, pinString);

    EXPECT_FALSE(result);
}

TEST_F(CcspMtaTestFixture, Dect_SetParamStringValue_PIN_Failure) {
    ANSC_HANDLE hInsContext = nullptr;
    char paramName[] = "PIN";
    char pinString[] = "1234";
    int ind = 0;

 
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(testing::StrEq("PIN"), testing::_, testing::StrEq(paramName), testing::_, testing::_, testing::_))
        .WillOnce(testing::DoAll(testing::SetArgPointee<3>(ind), testing::Return(EOK)));

   
    EXPECT_CALL(*g_mtaHALMock, mta_hal_SetDectPIN(testing::StrEq(pinString)))
        .WillOnce(testing::Return(1));  

    BOOL result = Dect_SetParamStringValue(hInsContext, paramName, pinString);

    EXPECT_TRUE(result);
}

//Dect_GetParamBoolValue
TEST_F(CcspMtaTestFixture, Dect_GetParamBoolValue_Enable_Success) {
    ANSC_HANDLE hInsContext = nullptr;
    char paramName[] = "Enable";
    BOOL enable = FALSE;
    int ind = 0;

    
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(testing::_, testing::_, testing::_ , testing::_, testing::_, testing::_))
        .Times(1)
        .WillOnce(testing::DoAll(testing::SetArgPointee<3>(ind), testing::Return(0))); 

    
    EXPECT_CALL(*g_mtaHALMock, mta_hal_DectGetEnable(testing::_))
        .WillOnce(testing::DoAll(testing::SetArgPointee<0>(TRUE), testing::Return(RETURN_OK)));

    BOOL result = Dect_GetParamBoolValue(hInsContext, paramName, &enable);

    EXPECT_TRUE(result);
}  

// Test for unsupported parameter
TEST_F(CcspMtaTestFixture, Dect_GetParamBoolValue_UnsupportedParam) {
    ANSC_HANDLE hInsContext = nullptr;
    char paramName[] = "UnsupportedParam";
    BOOL boolValue = FALSE;
    int ind = 1;

    
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(testing::StrEq("Enable"), testing::_, testing::StrEq(paramName), testing::_, testing::_, testing::_))
        .WillOnce(testing::DoAll(testing::SetArgPointee<3>(ind), testing::Return(EOK)));

    
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(testing::StrEq("RegistrationMode"), testing::_, testing::StrEq(paramName), testing::_, testing::_, testing::_))
        .WillOnce(testing::DoAll(testing::SetArgPointee<3>(ind), testing::Return(EOK)));

    
    EXPECT_CALL(*g_mtaHALMock, mta_hal_DectGetEnable(testing::_)).Times(0);

    BOOL result = Dect_GetParamBoolValue(hInsContext, paramName, &boolValue);

    EXPECT_FALSE(result);
}

// Test for failure in mta_hal_DectGetEnable
TEST_F(CcspMtaTestFixture, Dect_GetParamBoolValue_Enable_Failure) {
    ANSC_HANDLE hInsContext = nullptr;
    char paramName[] = "Enable";
    BOOL enable = FALSE;
    int ind = 0;

   
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(testing::_, testing::_, testing::_ , testing::_, testing::_, testing::_))
        .Times(1)
        .WillOnce(testing::DoAll(testing::SetArgPointee<3>(ind), testing::Return(0))); 

    
    EXPECT_CALL(*g_mtaHALMock, mta_hal_DectGetEnable(testing::_))
        .WillOnce(testing::Return(1));  

    BOOL result = Dect_GetParamBoolValue(hInsContext, paramName, &enable);

    EXPECT_TRUE(result);
   
}


TEST_F(CcspMtaTestFixture, Dect_GetParamBoolValue_RegistrationMode_Success) {
    ANSC_HANDLE hInsContext = nullptr;
    char paramName[] = "RegistrationMode";
    BOOL regMode = FALSE;
    int ind = 0;

   
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(testing::StrEq("Enable"), testing::_, testing::StrEq(paramName), testing::_, testing::_, testing::_))
        .WillOnce(testing::DoAll(testing::SetArgPointee<3>(1), testing::Return(EOK)));

    
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(testing::StrEq("RegistrationMode"), testing::_, testing::StrEq(paramName), testing::_, testing::_, testing::_))
        .WillOnce(testing::DoAll(testing::SetArgPointee<3>(ind), testing::Return(EOK)));

    EXPECT_CALL(*g_mtaHALMock, mta_hal_DectGetRegistrationMode(testing::_))
        .WillOnce(testing::DoAll(testing::SetArgPointee<0>(TRUE), testing::Return(RETURN_OK)));

    BOOL result = Dect_GetParamBoolValue(hInsContext, paramName, &regMode);

    EXPECT_TRUE(result);
}

TEST_F(CcspMtaTestFixture, Dect_GetParamBoolValue_RegistrationMode_Failure) {
    ANSC_HANDLE hInsContext = nullptr;
    char paramName[] = "RegistrationMode";
    BOOL regMode = FALSE;
    int ind = 0;

    
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(testing::StrEq("Enable"), testing::_, testing::StrEq(paramName), testing::_, testing::_, testing::_))
        .WillOnce(testing::DoAll(testing::SetArgPointee<3>(1), testing::Return(EOK)));

    
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(testing::StrEq("RegistrationMode"), testing::_, testing::StrEq(paramName), testing::_, testing::_, testing::_))
        .WillOnce(testing::DoAll(testing::SetArgPointee<3>(ind), testing::Return(EOK)));

   
    EXPECT_CALL(*g_mtaHALMock, mta_hal_DectGetRegistrationMode(testing::_))
        .WillOnce(testing::Return(1));  

    BOOL result = Dect_GetParamBoolValue(hInsContext, paramName, &regMode);

    EXPECT_TRUE(result);  
}

//Dect_SetParamBoolValue
TEST_F(CcspMtaTestFixture, Dect_SetParamBoolValue_Enable_Success) {
    ANSC_HANDLE hInsContext = nullptr;
    char paramName[] = "Enable";
    BOOL enable = TRUE;  
    int ind = 0;

   
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(testing::_, testing::_, testing::_ , testing::_, testing::_, testing::_))
        .Times(1)
        .WillOnce(testing::DoAll(testing::SetArgPointee<3>(ind), testing::Return(0))); 

    
     EXPECT_CALL(*g_mtaHALMock, mta_hal_DectSetEnable(enable))
        .WillOnce(Return(RETURN_OK));

    
    BOOL result = Dect_SetParamBoolValue(hInsContext, paramName, enable);

   
    EXPECT_TRUE(result);
}

TEST_F(CcspMtaTestFixture, Dect_SetParamBoolValue_UnsupportedParam) {
    ANSC_HANDLE hInsContext = nullptr;
    char paramName[] = "UnsupportedParam";
    BOOL boolValue = FALSE;
    int ind = 1;

   
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(testing::StrEq("Enable"), testing::_, testing::StrEq(paramName), testing::_, testing::_, testing::_))
        .WillOnce(testing::DoAll(testing::SetArgPointee<3>(ind), testing::Return(EOK)));

    
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(testing::StrEq("RegistrationMode"), testing::_, testing::StrEq(paramName), testing::_, testing::_, testing::_))
        .WillOnce(testing::DoAll(testing::SetArgPointee<3>(ind), testing::Return(EOK)));

    

    BOOL result = Dect_SetParamBoolValue(hInsContext, paramName, boolValue);

    EXPECT_FALSE(result);
}

TEST_F(CcspMtaTestFixture, Dect_SetParamBoolValue_Enable_Failure) {
    ANSC_HANDLE hInsContext = nullptr;
    char paramName[] = "Enable";
    BOOL enable = FALSE;
    int ind = 0;

   
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(testing::_, testing::_, testing::_ , testing::_, testing::_, testing::_))
        .Times(1)
        .WillOnce(testing::DoAll(testing::SetArgPointee<3>(ind), testing::Return(0))); 

   
    EXPECT_CALL(*g_mtaHALMock, mta_hal_DectSetEnable(enable))
        .WillOnce(testing::Return(1));  

    BOOL result = Dect_SetParamBoolValue(hInsContext, paramName, enable);

    EXPECT_TRUE(result);
   
}

TEST_F(CcspMtaTestFixture, Dect_SetParamBoolValue_RegistrationMode_Success) {
    ANSC_HANDLE hInsContext = nullptr;
    char paramName[] = "RegistrationMode";
    BOOL regMode = FALSE;
    int ind = 0;

   
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(testing::StrEq("Enable"), testing::_, testing::StrEq(paramName), testing::_, testing::_, testing::_))
        .WillOnce(testing::DoAll(testing::SetArgPointee<3>(1), testing::Return(EOK)));

    
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(testing::StrEq("RegistrationMode"), testing::_, testing::StrEq(paramName), testing::_, testing::_, testing::_))
        .WillOnce(testing::DoAll(testing::SetArgPointee<3>(ind), testing::Return(EOK)));

    
    EXPECT_CALL(*g_mtaHALMock, mta_hal_DectSetRegistrationMode(regMode))
        .WillOnce(Return(RETURN_OK));

    BOOL result = Dect_SetParamBoolValue(hInsContext, paramName, regMode);

    EXPECT_TRUE(result);
}

TEST_F(CcspMtaTestFixture, Dect_SetParamBoolValue_RegistrationMode_Failure) {
    ANSC_HANDLE hInsContext = nullptr;
    char paramName[] = "RegistrationMode";
    BOOL regMode = FALSE;
    int ind = 0;

    
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(testing::StrEq("Enable"), testing::_, testing::StrEq(paramName), testing::_, testing::_, testing::_))
        .WillOnce(testing::DoAll(testing::SetArgPointee<3>(1), testing::Return(EOK)));

   
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(testing::StrEq("RegistrationMode"), testing::_, testing::StrEq(paramName), testing::_, testing::_, testing::_))
        .WillOnce(testing::DoAll(testing::SetArgPointee<3>(ind), testing::Return(EOK)));

   
    EXPECT_CALL(*g_mtaHALMock, mta_hal_DectSetRegistrationMode(regMode))
        .WillOnce(testing::Return(1));  
    BOOL result = Dect_SetParamBoolValue(hInsContext, paramName, regMode);

    EXPECT_TRUE(result);  
}

//Handsets_GetParamBoolValue
TEST_F(CcspMtaTestFixture, TestHandsetsGetParamBoolValue_StatusTrue)
{
    ANSC_HANDLE hInsContext = nullptr;
    char paramName[] = "Status";
    BOOL resultBool;
    BOOL returnValue;
    int ind = 0;  
    PCOSA_MTA_HANDSETS_INFO pInfo = new COSA_MTA_HANDSETS_INFO();
    hInsContext = reinterpret_cast<ANSC_HANDLE>(pInfo);
    
  
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk("Status", strlen("Status"), "Status", testing::_, testing::_, testing::_))
        .WillOnce(DoAll(testing::SetArgPointee<3>(ind), testing::Return(EOK)));

    
    returnValue = Handsets_GetParamBoolValue((ANSC_HANDLE)pInfo, "Status", &resultBool);

  
    EXPECT_TRUE(returnValue);
}

TEST_F(CcspMtaTestFixture, TestHandsetsGetParamBoolValue_StatusFalse)
{
    ANSC_HANDLE hInsContext = nullptr;
    char paramName[] = "unsupported";
    BOOL resultBool;
    BOOL returnValue;
    int ind = 1;  
    PCOSA_MTA_HANDSETS_INFO pInfo = new COSA_MTA_HANDSETS_INFO();
    hInsContext = reinterpret_cast<ANSC_HANDLE>(pInfo);
    
    
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk("Status", strlen("Status"), "Status", testing::_, testing::_, testing::_))
        .WillOnce(DoAll(testing::SetArgPointee<3>(ind), testing::Return(EOK)));

    
    returnValue = Handsets_GetParamBoolValue((ANSC_HANDLE)pInfo, "Status", &resultBool);

  
    EXPECT_FALSE(returnValue);
}

//Handsets_SetParamStringValue
TEST_F(CcspMtaTestFixture, TestHandsetsSetParamStringValue_OperatingTN_Success)
{
    ANSC_HANDLE hInsContext = nullptr;
    BOOL returnValue;
    int ind = 0;  
    errno_t strcpyResult = EOK;  
    PCOSA_MTA_HANDSETS_INFO pInfo = new COSA_MTA_HANDSETS_INFO();
    
   
    memset(pInfo->OperatingTN, 0, sizeof(pInfo->OperatingTN));
    
    hInsContext = reinterpret_cast<ANSC_HANDLE>(pInfo);

    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk("OperatingTN", strlen("OperatingTN"), "OperatingTN", testing::_, testing::_, testing::_))
        .WillOnce(DoAll(testing::SetArgPointee<3>(ind), testing::Return(EOK)));


    EXPECT_CALL(*g_safecLibMock, _strcpy_s_chk(testing::_, sizeof(pInfo->OperatingTN), "NewOperatingTN", testing::_))
        .WillOnce(DoAll(testing::Invoke([pInfo](char* dest, rsize_t, const char* src, size_t) {
            
            strncpy(pInfo->OperatingTN, src, sizeof(pInfo->OperatingTN) - 1);
            return EOK;
        }), testing::Return(strcpyResult)));


    returnValue = Handsets_SetParamStringValue((ANSC_HANDLE)pInfo, "OperatingTN", (char*)"NewOperatingTN");


    EXPECT_TRUE(returnValue);

    delete pInfo;
}

TEST_F(CcspMtaTestFixture, TestHandsetsSetParamStringValue_OperatingTN_Failure)
{
    ANSC_HANDLE hInsContext = nullptr;
    BOOL returnValue;
    int ind = 0; 
    errno_t strcpyResult = -1;  
    PCOSA_MTA_HANDSETS_INFO pInfo = new COSA_MTA_HANDSETS_INFO();
    

    strncpy(pInfo->OperatingTN, "ExistingValue", sizeof(pInfo->OperatingTN) - 1);
    
    hInsContext = reinterpret_cast<ANSC_HANDLE>(pInfo);
    
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk("OperatingTN", strlen("OperatingTN"), "OperatingTN", testing::_, testing::_, testing::_))
        .WillOnce(DoAll(testing::SetArgPointee<3>(ind), testing::Return(EOK)));

    EXPECT_CALL(*g_safecLibMock, _strcpy_s_chk(testing::_, sizeof(pInfo->OperatingTN), "NewOperatingTN", testing::_))
        .WillOnce(testing::Return(strcpyResult));  // Simulate failure by returning non-EOK value

    returnValue = Handsets_SetParamStringValue((ANSC_HANDLE)pInfo, "OperatingTN", (char*)"NewOperatingTN");

    EXPECT_FALSE(returnValue);
    

    delete pInfo;
}

TEST_F(CcspMtaTestFixture, TestHandsetsSetParamStringValue_OperatingTN_strcmp_s_Failure)
{
    ANSC_HANDLE hInsContext = nullptr;
    BOOL returnValue;
    int ind = 1;  
    errno_t strcpyResult = EOK;  
    PCOSA_MTA_HANDSETS_INFO pInfo = new COSA_MTA_HANDSETS_INFO();
    

    memset(pInfo->OperatingTN, 0, sizeof(pInfo->OperatingTN));
    
    hInsContext = reinterpret_cast<ANSC_HANDLE>(pInfo);
    
    
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk("OperatingTN", strlen("OperatingTN"), "OperatingTN", testing::_, testing::_, testing::_))
        .WillOnce(DoAll(testing::SetArgPointee<3>(ind), testing::Return(EOK)));

   
    
  
    returnValue = Handsets_SetParamStringValue((ANSC_HANDLE)pInfo, "OperatingTN", (char*)"NewOperatingTN");

   
    EXPECT_FALSE(returnValue);
    
    
    delete pInfo;
}

//DECTLog_GetParamUlongValue
TEST_F(CcspMtaTestFixture, DECTLog_GetParamUlongValue_Index_Success) {
    ANSC_HANDLE hInsContext = nullptr; 
    char paramName[] = "Index";
    ULONG value;
    int ind = 0;
    PCOSA_DML_DECTLOG_FULL pConf = new COSA_DML_DECTLOG_FULL(); 
    pConf->Index = 42; 

    hInsContext = reinterpret_cast<ANSC_HANDLE>(pConf);

    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(testing::_, testing::_, testing::_ , testing::_, testing::_, testing::_))
        .Times(1)
        .WillOnce(testing::DoAll(testing::SetArgPointee<3>(ind), testing::Return(0))); 

    BOOL result = DECTLog_GetParamUlongValue(hInsContext, paramName, &value);

    EXPECT_TRUE(result); 
    delete pConf; 
} 

TEST_F(CcspMtaTestFixture, DECTLog_GetParamUlongValue_EventID_Success) {
    ANSC_HANDLE hInsContext = nullptr; 
    char paramName[] = "EventID";
    ULONG value;
    int ind = 0;
    PCOSA_DML_DECTLOG_FULL pConf = new COSA_DML_DECTLOG_FULL(); 
    pConf->EventID = 98765; 

    hInsContext = reinterpret_cast<ANSC_HANDLE>(pConf);

    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(testing::StrEq("Index"), testing::_, testing::StrEq(paramName), testing::_, testing::_, testing::_))
        .WillOnce(testing::DoAll(testing::SetArgPointee<3>(1), testing::Return(0)));  // Simulate a failed string comparison for ID

    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(testing::StrEq("EventID"), testing::_, testing::StrEq(paramName), testing::_, testing::_, testing::_))
        .WillOnce(testing::DoAll(testing::SetArgPointee<3>(ind), testing::Return(0)));  // Simulate a failed string comparison for Level
    BOOL result = DECTLog_GetParamUlongValue(hInsContext, paramName, &value);

    EXPECT_TRUE(result); 
    delete pConf; 
}

TEST_F(CcspMtaTestFixture, DECTLog_GetParamUlongValue_UnsupportedParam_Failure) {
    ANSC_HANDLE hInsContext = nullptr; 
    char paramName[] = "UnsupportedParam";
    ULONG value;
    int ind = 1; 
    PCOSA_DML_DECTLOG_FULL pConf = new COSA_DML_DECTLOG_FULL(); 

    hInsContext = reinterpret_cast<ANSC_HANDLE>(pConf);

    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(
        testing::StrEq("Index"),
        testing::_,
        testing::StrEq(paramName),
        testing::_,
        testing::_,
        testing::_))
    .WillOnce(testing::DoAll(testing::SetArgPointee<3>(ind), testing::Return(0))); 

    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(
        testing::StrEq("EventID"),
        testing::_,
        testing::StrEq(paramName),
        testing::_,
        testing::_,
        testing::_))
    .WillOnce(testing::DoAll(testing::SetArgPointee<3>(ind), testing::Return(0))); 

    BOOL result = DECTLog_GetParamUlongValue(hInsContext, paramName, &value);

    EXPECT_FALSE(result); 
    delete pConf; 
}

TEST_F(CcspMtaTestFixture, DECTLog_GetParamUlongValue_EventLevel_Success) {
    // Arrange
    PCOSA_DML_DECTLOG_FULL pConf = new COSA_DML_DECTLOG_FULL(); 
    pConf->EventLevel = 5;  
    ANSC_HANDLE hInsContext = reinterpret_cast<ANSC_HANDLE>(pConf);  
    char paramName[] = "EventLevel";
     int ind = 0;
    ULONG value;
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(testing::StrEq("Index"), testing::_, testing::StrEq(paramName), testing::_, testing::_, testing::_))
        .WillOnce(testing::DoAll(testing::SetArgPointee<3>(1), testing::Return(0))); 

    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(testing::StrEq("EventID"), testing::_, testing::StrEq(paramName), testing::_, testing::_, testing::_))
        .WillOnce(testing::DoAll(testing::SetArgPointee<3>(2), testing::Return(0)));  
    
   
    BOOL result = DECTLog_GetParamUlongValue(hInsContext, paramName, &value);

    
    EXPECT_TRUE(result);  
    EXPECT_EQ(value, 5);  

    
    delete pConf;
}