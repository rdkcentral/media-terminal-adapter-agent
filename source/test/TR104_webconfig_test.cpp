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
#include <stdint.h>
#include <string.h>
#include "mta_mock.h"
#include "webconfig_framework.h"

using ::testing::_;
using ::testing::DoAll;
using ::testing::StrEq;
using ::testing::Return;
using ::testing::Invoke;
using ::testing::ReturnArg;
using ::testing::SetArgPointee;
using ::testing::SetArrayArgument;


extern SyscfgMock * g_syscfgMock;
extern webconfigFwMock *g_webconfigFwMock;

extern "C"
{
   typedef struct
{
    bool DevEnabled;
}VoIPProfile_Table_t;

typedef struct
{
    char LocalTimeZone[64];
}POTS_Table_t;

typedef struct
{
    bool Enable;
}FXS_Table_t;

typedef struct
{
#define NETWORK_TABLE_MEMBER_COUNT 2
    char ProxyServer[64];
    int ProxyServerPort;
}Network_Table_t;

typedef struct
{
    char Domain[64];
}Operator_Table_t;

typedef struct
{
#define CLIENT_TABLE_MEMBER_COUNT 3
    char IMPUId[64];
    char AuthUserName[64];
    char AuthPassword[64];
}Client_Table_t;

typedef struct
{
    bool CallWaitingEnable;
}Set_Table_t;

typedef struct
{
    int profile_count;
    VoIPProfile_Table_t* profileTable;
    int pots_count;
    POTS_Table_t* potsTable;
    int fxs_count;
    FXS_Table_t* fxsTable;
    int network_count;
    Network_Table_t* networkTable;
    int operator_count;
    Operator_Table_t* operatorTable;
    int client_count;
    Client_Table_t* clientTable;
    int set_count;
    Set_Table_t* setTable;
}VoiceService_Table_t;

typedef struct
{
    void     *data;
    int      voiceservice_count;
    VoiceService_Table_t* voiceserviceTable;
    char      subdoc_name[32];
    uint64_t  version;
    uint16_t  transaction_id;
}TR104_WebConfig_t;
    

    #include "mta_hal.h"
    void TR104_free_TR181_resources(int total_params,char** aParamDetail);
    void TR104_Process_free_resources(void *arg);
    int param_count(TR104_WebConfig_t* pTR104_WebConfig);  
    unsigned int getTr104DataBlobVersion(char *pSubDoc);
    int setTr104DataBlobVersion(char *pSubDoc, unsigned int version);
    int WebConfig_Process_set_Params(Set_Table_t* e,msgpack_object_map *map);
    int WebConfig_Process_client_Params(Client_Table_t* e,msgpack_object_map *map);
    int WebConfig_Process_operator_Params(Operator_Table_t* e,msgpack_object_map *map);
    int WebConfig_Process_network_Params(Network_Table_t* e,msgpack_object_map *map);
    int WebConfig_Process_fxs_Params(FXS_Table_t* e,msgpack_object_map *map);
    int WebConfig_Process_pots_Params(POTS_Table_t* e,msgpack_object_map *map);
    int WebConfig_Process_profile_Params(VoIPProfile_Table_t* e,msgpack_object_map *map);
    int WebConfig_Process_voiceservice_Params(VoiceService_Table_t* e, msgpack_object_map *map);
    ANSC_STATUS init_TR104_web_config();

}

extern MtaHalMock *g_mtaHALMock;


//Test cases for TR104_free_TR181_resources
TEST_F(CcspMtaTestFixture, FreeValidParams) {
    int total_params = 3;
    char** aParamDetail = (char**)malloc(total_params * sizeof(char*));
    
    for (int i = 0; i < total_params; ++i) {
        aParamDetail[i] = (char*)malloc(10 * sizeof(char));
        strcpy(aParamDetail[i], "test");
    }

    TR104_free_TR181_resources(total_params, aParamDetail);

}

TEST_F(CcspMtaTestFixture, SomeParamsAreNull) {
    int total_params = 3;
    char** aParamDetail = (char**)malloc(total_params * sizeof(char*));
    
    aParamDetail[0] = (char*)malloc(10 * sizeof(char));
    strcpy(aParamDetail[0], "test");
    aParamDetail[1] = NULL; 
    aParamDetail[2] = (char*)malloc(10 * sizeof(char));
    strcpy(aParamDetail[2], "test");
    TR104_free_TR181_resources(total_params, aParamDetail);
}

TEST_F(CcspMtaTestFixture, NullParamDetail) {
    char** aParamDetail = nullptr;

    TR104_free_TR181_resources(0, aParamDetail);

    EXPECT_EQ(aParamDetail, nullptr);
}


// Test cases for param_count
TEST_F(CcspMtaTestFixture, ValidInput) {
   
    TR104_WebConfig_t config;
    config.voiceservice_count = 2; 
    config.voiceserviceTable = new VoiceService_Table_t[config.voiceservice_count];

    // Fill in the first service
    config.voiceserviceTable[0].profile_count = 1;
    config.voiceserviceTable[0].pots_count = 1;
    config.voiceserviceTable[0].fxs_count = 1;
    config.voiceserviceTable[0].network_count = 2; // NETWORK_TABLE_MEMBER_COUNT = 2
    config.voiceserviceTable[0].client_count = 1; // CLIENT_TABLE_MEMBER_COUNT = 3
    config.voiceserviceTable[0].operator_count = 1;
    config.voiceserviceTable[0].set_count = 1;

    // Fill in the second service
    config.voiceserviceTable[1].profile_count = 2;
    config.voiceserviceTable[1].pots_count = 0;
    config.voiceserviceTable[1].fxs_count = 2;
    config.voiceserviceTable[1].network_count = 1; // NETWORK_TABLE_MEMBER_COUNT = 2
    config.voiceserviceTable[1].client_count = 2; // CLIENT_TABLE_MEMBER_COUNT = 3
    config.voiceserviceTable[1].operator_count = 1;
    config.voiceserviceTable[1].set_count = 1;

    int count = param_count(&config);

    int expected_count = 26; //count based on above values
    ASSERT_EQ(count, expected_count);

   
    delete[] config.voiceserviceTable;
}

TEST_F(CcspMtaTestFixture, NoVoiceServices) {
    
    TR104_WebConfig_t config;
    config.voiceservice_count = 0;
    config.voiceserviceTable = nullptr; 

    int count = param_count(&config);

    ASSERT_EQ(count, 0);
}

TEST_F(CcspMtaTestFixture, SingleServiceZeroCounts) {
    
    TR104_WebConfig_t config;
    config.voiceservice_count = 1;
    config.voiceserviceTable = new VoiceService_Table_t[config.voiceservice_count];

    // Set all counts to zero
    config.voiceserviceTable[0].profile_count = 0;
    config.voiceserviceTable[0].pots_count = 0;
    config.voiceserviceTable[0].fxs_count = 0;
    config.voiceserviceTable[0].network_count = 0;
    config.voiceserviceTable[0].client_count = 0;
    config.voiceserviceTable[0].operator_count = 0;
    config.voiceserviceTable[0].set_count = 0;

    int count = param_count(&config);

    ASSERT_EQ(count, 0);

    delete[] config.voiceserviceTable;
}

TEST_F(CcspMtaTestFixture, CleanUpMemory) {

    TR104_WebConfig_t config;
    config.voiceservice_count = 2;
    config.voiceserviceTable = new VoiceService_Table_t[config.voiceservice_count];

    // Fill in data
    for (int i = 0; i < config.voiceservice_count; ++i) {
        config.voiceserviceTable[i].profile_count = i + 1;  // 1 for i=0, 2 for i=1
        config.voiceserviceTable[i].pots_count = i;          // 0 for i=0, 1 for i=1
        config.voiceserviceTable[i].fxs_count = 0;           // Always 0
        config.voiceserviceTable[i].network_count = 0;       // Always 0
        config.voiceserviceTable[i].client_count = 0;        // Always 0
        config.voiceserviceTable[i].operator_count = 0;      // Always 0
        config.voiceserviceTable[i].set_count = 0;           // Always 0
    }

    int count = param_count(&config);
    delete[] config.voiceserviceTable;
    ASSERT_EQ(count, 4);
}

// Test cases for getTr104DataBlobVersion
TEST_F(CcspMtaTestFixture, GetBlobVersionSuccess) {
    const char* subDoc = "example_subdoc";
    char returnedVersionStr[64] = "42"; 

    EXPECT_CALL(*g_syscfgMock, syscfg_get(nullptr, StrEq("example_subdoc_version"), _, sizeof(returnedVersionStr)))
        .WillOnce(DoAll(SetArrayArgument<2>(returnedVersionStr, returnedVersionStr + strlen(returnedVersionStr) + 1), Return(0))); // Copy the version string to the argument

    unsigned int version = getTr104DataBlobVersion(const_cast<char*>(subDoc));

    ASSERT_EQ(version, 42);
}

TEST_F(CcspMtaTestFixture, GetBlobVersionFailure) {
    const char* subDoc = "example_subdoc";

    EXPECT_CALL(*g_syscfgMock, syscfg_get(nullptr, StrEq("example_subdoc_version"), _, _))
        .WillOnce(Return(-1)); 

    unsigned int version = getTr104DataBlobVersion(const_cast<char*>(subDoc));

    ASSERT_EQ(version, 0); 
}

TEST_F(CcspMtaTestFixture, SetBlobVersionSuccess) {
    const char* subDoc = "example_subdoc";
    unsigned int version = 42;

    EXPECT_CALL(*g_syscfgMock, syscfg_set_nns_commit(StrEq("example_subdoc_version"), StrEq("42")))
        .WillOnce(Return(0)); 

    int result = setTr104DataBlobVersion(const_cast<char*>(subDoc), version);

    ASSERT_EQ(result, 0); 
}

TEST_F(CcspMtaTestFixture, SetBlobVersionFailure) {
    const char* subDoc = "example_subdoc";
    unsigned int version = 42;

    // Mock syscfg_set_nns_commit to simulate failure
    EXPECT_CALL(*g_syscfgMock, syscfg_set_nns_commit(StrEq("example_subdoc_version"), StrEq("42")))
        .WillOnce(Return(-1)); 

    int result = setTr104DataBlobVersion(const_cast<char*>(subDoc), version);

    ASSERT_EQ(result, -1);  
}

//Test cases for init_TR104_web_config
TEST_F(CcspMtaTestFixture, InitTR104WebConfigSuccess) {
    EXPECT_CALL(*g_webconfigFwMock, register_sub_docs(_, 1, _, _))
        .Times(1)
        .WillOnce(Invoke([](_blobRegInfo*, int, unsigned int (*)(char*), int (*)(char*, unsigned int)) {
        }));

    ANSC_STATUS result = init_TR104_web_config();
    ASSERT_EQ(result, ANSC_STATUS_SUCCESS);
}


// Test cases for WebConfig_Process_set_Params
TEST_F(CcspMtaTestFixture, CallWaitingEnable_True) {
    Set_Table_t e;
    msgpack_object_kv kv;
    
    // Prepare msgpack_object_kv for "CallWaitingEnable" with true value
    kv.key.via.str.ptr = "CallWaitingEnable";
    kv.key.via.str.size = strlen(kv.key.via.str.ptr);
    kv.val.type = MSGPACK_OBJECT_BOOLEAN;
    kv.val.via.boolean = true;

    // Prepare msgpack_object_map with the kv
    msgpack_object_map map;
    map.size = 1;
    map.ptr = &kv;

    int result = WebConfig_Process_set_Params(&e, &map);

    ASSERT_EQ(result, RETURN_OK);
    ASSERT_TRUE(e.CallWaitingEnable); 
}


TEST_F(CcspMtaTestFixture, CallWaitingEnable_False) {
    Set_Table_t e;
    msgpack_object_kv kv;

    // Prepare msgpack_object_kv for "CallWaitingEnable" with false value
    kv.key.via.str.ptr = "CallWaitingEnable";
    kv.key.via.str.size = strlen(kv.key.via.str.ptr);
    kv.val.type = MSGPACK_OBJECT_BOOLEAN;
    kv.val.via.boolean = false;

    // Prepare msgpack_object_map with the kv
    msgpack_object_map map;
    map.size = 1;
    map.ptr = &kv;

    int result = WebConfig_Process_set_Params(&e, &map);

    ASSERT_EQ(result, RETURN_OK);
    ASSERT_FALSE(e.CallWaitingEnable); 
}

TEST_F(CcspMtaTestFixture, CallWaitingEnable_InvalidType) {
    Set_Table_t e;
    msgpack_object_kv kv;

    // Prepare msgpack_object_kv for "CallWaitingEnable" with an invalid type (not boolean)
    kv.key.via.str.ptr = "CallWaitingEnable";
    kv.key.via.str.size = strlen(kv.key.via.str.ptr);
    kv.val.type = MSGPACK_OBJECT_POSITIVE_INTEGER ; // Using Unsigned integer as an invalid type
    kv.val.via.u64 = 1; // Set a dummy value

    // Prepare msgpack_object_map with the kv
    msgpack_object_map map;
    map.size = 1;
    map.ptr = &kv;

    int result = WebConfig_Process_set_Params(&e, &map);

    ASSERT_EQ(result, VALIDATION_FALIED);
}


TEST_F(CcspMtaTestFixture, CallWaitingEnable_InvalidKey) {
    Set_Table_t e;
    msgpack_object_kv kv;

    // Prepare msgpack_object_kv for an invalid key
    kv.key.via.str.ptr = "InvalidKey"; // Mismatched key
    kv.key.via.str.size = strlen(kv.key.via.str.ptr);
    kv.val.type = MSGPACK_OBJECT_BOOLEAN;
    kv.val.via.boolean = true;

    // Prepare msgpack_object_map with the kv
    msgpack_object_map map;
    map.size = 1;
    map.ptr = &kv;

    int result = WebConfig_Process_set_Params(&e, &map);
    ASSERT_EQ(result, VALIDATION_FALIED); 
}

//Test cases for WebConfig_Process_client_Params
TEST_F(CcspMtaTestFixture, ClientParams_Success) {

    Client_Table_t e;

    // Prepare msgpack_object_kv for each key with valid string values
    msgpack_object_kv kv[3];

    // IMPUId
    kv[0].key.via.str.ptr = "IMPUId";
    kv[0].key.via.str.size = strlen(kv[0].key.via.str.ptr);
    kv[0].val.type = MSGPACK_OBJECT_STR;
    kv[0].val.via.str.ptr = "impu-id-value";
    kv[0].val.via.str.size = strlen(kv[0].val.via.str.ptr);

    // AuthUserName
    kv[1].key.via.str.ptr = "AuthUserName";
    kv[1].key.via.str.size = strlen(kv[1].key.via.str.ptr);
    kv[1].val.type = MSGPACK_OBJECT_STR;
    kv[1].val.via.str.ptr = "auth-user";
    kv[1].val.via.str.size = strlen(kv[1].val.via.str.ptr);

    // AuthPassword
    kv[2].key.via.str.ptr = "AuthPassword";
    kv[2].key.via.str.size = strlen(kv[2].key.via.str.ptr);
    kv[2].val.type = MSGPACK_OBJECT_STR;
    kv[2].val.via.str.ptr = "auth-pass";
    kv[2].val.via.str.size = strlen(kv[2].val.via.str.ptr);

    // Prepare msgpack_object_map with the kv array
    msgpack_object_map map;
    map.size = 3;
    map.ptr = kv;

    // Call the function
    int result = WebConfig_Process_client_Params(&e, &map);

    // Verify results
    ASSERT_EQ(result, RETURN_OK);
    ASSERT_STREQ(e.IMPUId, "impu-id-value");
    ASSERT_STREQ(e.AuthUserName, "auth-user");
    ASSERT_STREQ(e.AuthPassword, "auth-pass");
}

TEST_F(CcspMtaTestFixture, ClientParams_Failure_InvalidKey) {
    
    Client_Table_t e;

    // Prepare msgpack_object_kv with an invalid key
    msgpack_object_kv kv;
    kv.key.via.str.ptr = "InvalidKey";
    kv.key.via.str.size = strlen(kv.key.via.str.ptr);
    kv.val.type = MSGPACK_OBJECT_STR;
    kv.val.via.str.ptr = "test_value";
    kv.val.via.str.size = strlen(kv.val.via.str.ptr);

    // Prepare msgpack_object_map with the kv
    msgpack_object_map map;
    map.size = 1;
    map.ptr = &kv;

    int result = WebConfig_Process_client_Params(&e, &map);
    ASSERT_EQ(result, VALIDATION_FALIED); 
}

TEST_F(CcspMtaTestFixture, ClientParams_Failure_InvalidType) {
    Client_Table_t e;

    // Prepare msgpack_object_kv for "AuthUserName" with an invalid type (not string)
    msgpack_object_kv kv;
    kv.key.via.str.ptr = "AuthUserName";
    kv.key.via.str.size = strlen(kv.key.via.str.ptr);
    kv.val.type = MSGPACK_OBJECT_BOOLEAN;  // Invalid type
    kv.val.via.boolean = true;

    // Prepare msgpack_object_map with the kv
    msgpack_object_map map;
    map.size = 1;
    map.ptr = &kv;

    int result = WebConfig_Process_client_Params(&e, &map);

    ASSERT_EQ(result, VALIDATION_FALIED); 
}

TEST_F(CcspMtaTestFixture, ClientParams_Failure_NullPointer) {
    
    Client_Table_t e;

    // Prepare msgpack_object_map with a null pointer
    msgpack_object_map map;
    map.size = 1;
    map.ptr = NULL;

    int result = WebConfig_Process_client_Params(&e, &map);

  
    ASSERT_EQ(result, RETURN_ERR);  
}

//Test cases forWebConfig_Process_operator_Params
TEST_F(CcspMtaTestFixture, OperatorParams_Success) {

    Operator_Table_t e;

    // Prepare msgpack_object_kv for "Domain" with a valid string value
    msgpack_object_kv kv;
    kv.key.via.str.ptr = "Domain";
    kv.key.via.str.size = strlen(kv.key.via.str.ptr);
    kv.val.type = MSGPACK_OBJECT_STR;
    kv.val.via.str.ptr = "example.com";
    kv.val.via.str.size = strlen(kv.val.via.str.ptr);

    // Prepare msgpack_object_map with the kv
    msgpack_object_map map;
    map.size = 1;
    map.ptr = &kv;

    int result = WebConfig_Process_operator_Params(&e, &map);

    ASSERT_EQ(result, RETURN_OK);
    ASSERT_STREQ(e.Domain, "example.com");
}

TEST_F(CcspMtaTestFixture, OperatorParams_Failure_InvalidType) {

    Operator_Table_t e;

    // Prepare msgpack_object_kv for "Domain" with an invalid type (not string)
    msgpack_object_kv kv;
    kv.key.via.str.ptr = "Domain";
    kv.key.via.str.size = strlen(kv.key.via.str.ptr);
    kv.val.type = MSGPACK_OBJECT_BOOLEAN;  // Invalid type
    kv.val.via.boolean = true;

    // Prepare msgpack_object_map with the kv
    msgpack_object_map map;
    map.size = 1;
    map.ptr = &kv;

    int result = WebConfig_Process_operator_Params(&e, &map);

    ASSERT_EQ(result, VALIDATION_FALIED);
}

TEST_F(CcspMtaTestFixture, OperatorParams_Failure_InvalidKey) {

    Operator_Table_t e;

    // Prepare msgpack_object_kv with an invalid key
    msgpack_object_kv kv;
    kv.key.via.str.ptr = "InvalidKey";
    kv.key.via.str.size = strlen(kv.key.via.str.ptr);
    kv.val.type = MSGPACK_OBJECT_STR;
    kv.val.via.str.ptr = "some-value";
    kv.val.via.str.size = strlen(kv.val.via.str.ptr);

    // Prepare msgpack_object_map with the kv
    msgpack_object_map map;
    map.size = 1;
    map.ptr = &kv;

    int result = WebConfig_Process_operator_Params(&e, &map);

    ASSERT_EQ(result, VALIDATION_FALIED); 
}

TEST_F(CcspMtaTestFixture, OperatorParams_Failure_NullPointer) {

    Operator_Table_t e;

    // Prepare msgpack_object_map with a null pointer
    msgpack_object_map map;
    map.size = 1;
    map.ptr = NULL;

    int result = WebConfig_Process_operator_Params(&e, &map);

    ASSERT_EQ(result, RETURN_ERR);
}

// Test cases for WebConfig_Process_fxs_Params
TEST_F(CcspMtaTestFixture, FXSParams_Success_EnableTrue) {
    FXS_Table_t e;

    // Prepare msgpack_object_kv for "Enable" with a boolean true value
    msgpack_object_kv kv;
    kv.key.via.str.ptr = "Enable";
    kv.key.via.str.size = strlen(kv.key.via.str.ptr);
    kv.val.type = MSGPACK_OBJECT_BOOLEAN;
    kv.val.via.boolean = true;

    // Prepare msgpack_object_map with the kv
    msgpack_object_map map;
    map.size = 1;
    map.ptr = &kv;

    int result = WebConfig_Process_fxs_Params(&e, &map);

    ASSERT_EQ(result, RETURN_OK);
    ASSERT_TRUE(e.Enable); 
}

TEST_F(CcspMtaTestFixture, FXSParams_Success_EnableFalse) {
    FXS_Table_t e;

    // Prepare msgpack_object_kv for "Enable" with a boolean false value
    msgpack_object_kv kv;
    kv.key.via.str.ptr = "Enable";
    kv.key.via.str.size = strlen(kv.key.via.str.ptr);
    kv.val.type = MSGPACK_OBJECT_BOOLEAN;
    kv.val.via.boolean = false;

    // Prepare msgpack_object_map with the kv
    msgpack_object_map map;
    map.size = 1;
    map.ptr = &kv;

    int result = WebConfig_Process_fxs_Params(&e, &map);

    ASSERT_EQ(result, RETURN_OK);
    ASSERT_FALSE(e.Enable); 
}

TEST_F(CcspMtaTestFixture, FXSParams_Failure_InvalidType) {
    FXS_Table_t e;

    // Prepare msgpack_object_kv for "Enable" with an invalid type (not boolean)
    msgpack_object_kv kv;
    kv.key.via.str.ptr = "Enable";
    kv.key.via.str.size = strlen(kv.key.via.str.ptr);
    kv.val.type = MSGPACK_OBJECT_STR;
    kv.val.via.str.ptr = "true";
    kv.val.via.str.size = strlen(kv.val.via.str.ptr);

    // Prepare msgpack_object_map with the kv
    msgpack_object_map map;
    map.size = 1;
    map.ptr = &kv;

    int result = WebConfig_Process_fxs_Params(&e, &map);

    ASSERT_EQ(result, VALIDATION_FALIED);
}

TEST_F(CcspMtaTestFixture, FXSParams_Failure_InvalidKey) {
    FXS_Table_t e;

    // Prepare msgpack_object_kv with an invalid key
    msgpack_object_kv kv;
    kv.key.via.str.ptr = "InvalidKey";
    kv.key.via.str.size = strlen(kv.key.via.str.ptr);
    kv.val.type = MSGPACK_OBJECT_BOOLEAN;
    kv.val.via.boolean = true;

    // Prepare msgpack_object_map with the kv
    msgpack_object_map map;
    map.size = 1;
    map.ptr = &kv;

    int result = WebConfig_Process_fxs_Params(&e, &map);

    ASSERT_EQ(result, VALIDATION_FALIED);
}

// Test cases for WebConfig_Process_network_Params
TEST_F(CcspMtaTestFixture, NetworkParams_Success_ProxyServer) {
    Network_Table_t e;

    // Prepare msgpack_object_kv for "ProxyServer" with a valid string value
    msgpack_object_kv kv;
    kv.key.via.str.ptr = "ProxyServer";
    kv.key.via.str.size = strlen(kv.key.via.str.ptr);
    kv.val.type = MSGPACK_OBJECT_STR;
    kv.val.via.str.ptr = "proxy.example.com";
    kv.val.via.str.size = strlen(kv.val.via.str.ptr);

    // Prepare msgpack_object_map with the kv
    msgpack_object_map map;
    map.size = 1;
    map.ptr = &kv;

    int result = WebConfig_Process_network_Params(&e, &map);

    ASSERT_EQ(result, RETURN_OK);
    ASSERT_STREQ(e.ProxyServer, "proxy.example.com");
}

TEST_F(CcspMtaTestFixture, NetworkParams_Success_ProxyServerPort_PositiveInteger) {
    Network_Table_t e;

    // Prepare msgpack_object_kv for "ProxyServerPort" with a positive integer
    msgpack_object_kv kv;
    kv.key.via.str.ptr = "ProxyServerPort";
    kv.key.via.str.size = strlen(kv.key.via.str.ptr);
    kv.val.type = MSGPACK_OBJECT_POSITIVE_INTEGER;
    kv.val.via.u64 = 8080;

    // Prepare msgpack_object_map with the kv
    msgpack_object_map map;
    map.size = 1;
    map.ptr = &kv;

    int result = WebConfig_Process_network_Params(&e, &map);

    ASSERT_EQ(result, RETURN_OK);
    ASSERT_EQ(e.ProxyServerPort, 8080);
}

TEST_F(CcspMtaTestFixture, NetworkParams_Success_ProxyServerPort_NegativeInteger) {

    Network_Table_t e;

    // Prepare msgpack_object_kv for "ProxyServerPort" with a negative integer
    msgpack_object_kv kv;
    kv.key.via.str.ptr = "ProxyServerPort";
    kv.key.via.str.size = strlen(kv.key.via.str.ptr);
    kv.val.type = MSGPACK_OBJECT_NEGATIVE_INTEGER;
    kv.val.via.i64 = -1;

    // Prepare msgpack_object_map with the kv
    msgpack_object_map map;
    map.size = 1;
    map.ptr = &kv;

    int result = WebConfig_Process_network_Params(&e, &map);

    ASSERT_EQ(result, RETURN_OK);
    ASSERT_EQ(e.ProxyServerPort, -1);
}

TEST_F(CcspMtaTestFixture, NetworkParams_Failure_InvalidType_ProxyServer) {
    Network_Table_t e;

    // Prepare msgpack_object_kv for "ProxyServer" with an invalid type (not string)
    msgpack_object_kv kv;
    kv.key.via.str.ptr = "ProxyServer";
    kv.key.via.str.size = strlen(kv.key.via.str.ptr);
    kv.val.type = MSGPACK_OBJECT_BOOLEAN;  // Invalid type
    kv.val.via.boolean = true;

    // Prepare msgpack_object_map with the kv
    msgpack_object_map map;
    map.size = 1;
    map.ptr = &kv;

    int result = WebConfig_Process_network_Params(&e, &map);

    ASSERT_EQ(result, VALIDATION_FALIED); 
}

TEST_F(CcspMtaTestFixture, NetworkParams_Failure_InvalidKey) {
    Network_Table_t e;

    // Prepare msgpack_object_kv with an invalid key
    msgpack_object_kv kv;
    kv.key.via.str.ptr = "InvalidKey";
    kv.key.via.str.size = strlen(kv.key.via.str.ptr);
    kv.val.type = MSGPACK_OBJECT_STR;
    kv.val.via.str.ptr = "some-value";
    kv.val.via.str.size = strlen(kv.val.via.str.ptr);

    // Prepare msgpack_object_map with the kv
    msgpack_object_map map;
    map.size = 1;
    map.ptr = &kv;

    int result = WebConfig_Process_network_Params(&e, &map);

    ASSERT_EQ(result, VALIDATION_FALIED);
}

TEST_F(CcspMtaTestFixture, NetworkParams_Failure_NullPointer) {
    Network_Table_t e;

    // Prepare msgpack_object_map with a null pointer
    msgpack_object_map map;
    map.size = 1;
    map.ptr = NULL;  

    int result = WebConfig_Process_network_Params(&e, &map);

    ASSERT_EQ(result, RETURN_ERR); 
}

//Test cases for WebConfig_Process_pots_Params
TEST_F(CcspMtaTestFixture, POTSParams_ValidLocalTimeZoneString) {

    POTS_Table_t potsTable;
    memset(&potsTable, 0, sizeof(potsTable));

    msgpack_object_map map;
    memset(&map, 0, sizeof(map));

    msgpack_object_kv keyValue;
    memset(&keyValue, 0, sizeof(keyValue));

    msgpack_object_str keyStr;
    keyStr.ptr = "LocalTimeZone";
    keyStr.size = strlen(keyStr.ptr);

    msgpack_object val;
    val.type = MSGPACK_OBJECT_STR;
    val.via.str.ptr = "UTC+05:30";
    val.via.str.size = strlen(val.via.str.ptr);

    keyValue.key.type = MSGPACK_OBJECT_STR;
    keyValue.key.via.str = keyStr;
    keyValue.val = val;

    map.ptr = &keyValue;
    map.size = 1;

    int result = WebConfig_Process_pots_Params(&potsTable, &map);

    ASSERT_EQ(result, RETURN_OK);
    ASSERT_STREQ(potsTable.LocalTimeZone, "UTC+05:30");
}

TEST_F(CcspMtaTestFixture, POTSParams_InvalidTypeForLocalTimeZone) {

    POTS_Table_t potsTable;
    memset(&potsTable, 0, sizeof(potsTable));

    msgpack_object_map map;
    memset(&map, 0, sizeof(map));

    msgpack_object_kv keyValue;
    memset(&keyValue, 0, sizeof(keyValue));

    msgpack_object_str keyStr;
    keyStr.ptr = "LocalTimeZone";
    keyStr.size = strlen(keyStr.ptr);

    msgpack_object val;
    val.type = MSGPACK_OBJECT_POSITIVE_INTEGER;  // Invalid type for LocalTimeZone
    val.via.u64 = 12345;

    keyValue.key.type = MSGPACK_OBJECT_STR;
    keyValue.key.via.str = keyStr;
    keyValue.val = val;

    map.ptr = &keyValue;
    map.size = 1;

    int result = WebConfig_Process_pots_Params(&potsTable, &map);

    ASSERT_EQ(result, VALIDATION_FALIED);
}

TEST_F(CcspMtaTestFixture, POTSParams_MissingLocalTimeZone) {
   
    POTS_Table_t potsTable;
    memset(&potsTable, 0, sizeof(potsTable));

    msgpack_object_map map;
    memset(&map, 0, sizeof(map));

    msgpack_object_kv keyValue;
    memset(&keyValue, 0, sizeof(keyValue));

    msgpack_object_str keyStr;
    keyStr.ptr = "InvalidKey";
    keyStr.size = strlen(keyStr.ptr);

    msgpack_object val;
    val.type = MSGPACK_OBJECT_STR;
    val.via.str.ptr = "InvalidValue";
    val.via.str.size = strlen(val.via.str.ptr);

    keyValue.key.type = MSGPACK_OBJECT_STR;
    keyValue.key.via.str = keyStr;
    keyValue.val = val;

    map.ptr = &keyValue;
    map.size = 1;

    int result = WebConfig_Process_pots_Params(&potsTable, &map);

    ASSERT_EQ(result, VALIDATION_FALIED);
}

//Test cases for WebConfig_Process_profile_Params
TEST_F(CcspMtaTestFixture, WebConfig_Process_profile_Params_DevEnabledTrue) {
    VoIPProfile_Table_t profileTable;
    msgpack_object_map map;
    msgpack_object_kv keyValue;
    msgpack_object_str keyStr;
    msgpack_object val;

    // Initialize the objects
    memset(&profileTable, 0, sizeof(profileTable));
    memset(&map, 0, sizeof(map));
    memset(&keyValue, 0, sizeof(keyValue));
    memset(&keyStr, 0, sizeof(keyStr));
    memset(&val, 0, sizeof(val));

    // Setup key-value pair
    keyStr.ptr = "DevEnabled";
    keyStr.size = strlen("DevEnabled");
    keyValue.key.type = MSGPACK_OBJECT_STR;
    keyValue.key.via.str = keyStr;

    // Setup boolean value as true
    val.type = MSGPACK_OBJECT_BOOLEAN;
    val.via.boolean = true;
    keyValue.val = val;

    // Setup map
    map.ptr = &keyValue;
    map.size = 1;

    int result = WebConfig_Process_profile_Params(&profileTable, &map);

    EXPECT_EQ(result, RETURN_OK);
    EXPECT_TRUE(profileTable.DevEnabled);
}

TEST_F(CcspMtaTestFixture, WebConfig_Process_profile_Params_DevEnabledFalse) {
    VoIPProfile_Table_t profileTable;
    msgpack_object_map map;
    msgpack_object_kv keyValue;
    msgpack_object_str keyStr;
    msgpack_object val;

    // Initialize the objects
    memset(&profileTable, 0, sizeof(profileTable));
    memset(&map, 0, sizeof(map));
    memset(&keyValue, 0, sizeof(keyValue));
    memset(&keyStr, 0, sizeof(keyStr));
    memset(&val, 0, sizeof(val));

    // Setup key-value pair
    keyStr.ptr = "DevEnabled";
    keyStr.size = strlen("DevEnabled");
    keyValue.key.type = MSGPACK_OBJECT_STR;
    keyValue.key.via.str = keyStr;

    // Setup boolean value as false
    val.type = MSGPACK_OBJECT_BOOLEAN;
    val.via.boolean = false;
    keyValue.val = val;

    // Setup map
    map.ptr = &keyValue;
    map.size = 1;

    int result = WebConfig_Process_profile_Params(&profileTable, &map);

    EXPECT_EQ(result, RETURN_OK);
    EXPECT_FALSE(profileTable.DevEnabled);
}

TEST_F(CcspMtaTestFixture, WebConfig_Process_profile_Params_InvalidKey) {
    VoIPProfile_Table_t profileTable;
    msgpack_object_map map;
    msgpack_object_kv keyValue;
    msgpack_object_str keyStr;
    msgpack_object val;

    // Initialize the objects
    memset(&profileTable, 0, sizeof(profileTable));
    memset(&map, 0, sizeof(map));
    memset(&keyValue, 0, sizeof(keyValue));
    memset(&keyStr, 0, sizeof(keyStr));
    memset(&val, 0, sizeof(val));

    // Setup key-value pair with an invalid key
    keyStr.ptr = "InvalidKey";
    keyStr.size = strlen("InvalidKey");
    keyValue.key.type = MSGPACK_OBJECT_STR;
    keyValue.key.via.str = keyStr;

    // Setup boolean value
    val.type = MSGPACK_OBJECT_BOOLEAN;
    val.via.boolean = true;
    keyValue.val = val;

    // Setup map
    map.ptr = &keyValue;
    map.size = 1;

    int result = WebConfig_Process_profile_Params(&profileTable, &map);

    EXPECT_EQ(result, VALIDATION_FALIED);
}

TEST_F(CcspMtaTestFixture, WebConfig_Process_profile_Params_InvalidType) {
    VoIPProfile_Table_t profileTable;
    msgpack_object_map map;
    msgpack_object_kv keyValue;
    msgpack_object_str keyStr;
    msgpack_object val;

    // Initialize the objects
    memset(&profileTable, 0, sizeof(profileTable));
    memset(&map, 0, sizeof(map));
    memset(&keyValue, 0, sizeof(keyValue));
    memset(&keyStr, 0, sizeof(keyStr));
    memset(&val, 0, sizeof(val));

    // Setup key-value pair with the correct key
    keyStr.ptr = "DevEnabled";
    keyStr.size = strlen("DevEnabled");
    keyValue.key.type = MSGPACK_OBJECT_STR;
    keyValue.key.via.str = keyStr;

    // Setup an invalid type for the value (e.g., integer instead of boolean)
    val.type = MSGPACK_OBJECT_POSITIVE_INTEGER;
    val.via.u64 = 123;
    keyValue.val = val;

    // Setup map
    map.ptr = &keyValue;
    map.size = 1;

    int result = WebConfig_Process_profile_Params(&profileTable, &map);
    EXPECT_EQ(result, VALIDATION_FALIED);
}

// Test cases for TR104_Process_free_resources
TEST_F(CcspMtaTestFixture, TR104_Process_free_resources_NullArg) {
    // Call the function with a NULL argument
    TR104_Process_free_resources(NULL);

}