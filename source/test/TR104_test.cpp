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
using ::testing::AnyNumber;
using ::testing::SetArgPointee;
using ::testing::SetArrayArgument;
using ::testing::SetArgumentPointee;

extern "C"
{
    #include "TR104.h"
    #include "mta_hal.h"
}

extern MtaHalMock *g_mtaHALMock;
extern rbusMock *g_rbusMock;
extern MtaHalMock *g_mtaHALMock;
extern webconfigFwMock *g_webconfigFwMock;



// Test Cases for TR104Services_TableHandler
TEST_F(CcspMtaTestFixture, TR104Services_TableHandler_ProvisioningStatusFailure) {

    EXPECT_CALL(*g_mtaHALMock, mta_hal_getMtaProvisioningStatus(_))
        .WillOnce(Return(-1)); 

    rbusProperty_t prop = nullptr;
    rbusGetHandlerOptions_t opts = {};
    rbusError_t result = TR104Services_TableHandler(nullptr, prop, &opts);

    EXPECT_EQ(result, RBUS_ERROR_DESTINATION_RESPONSE_FAILURE);
}

TEST_F(CcspMtaTestFixture, TR104Services_TableHandler_NotProvisioned) {
   
    MTAMGMT_MTA_PROVISION_STATUS status = MTA_NON_PROVISIONED;

    EXPECT_CALL(*g_mtaHALMock, mta_hal_getMtaProvisioningStatus(_))
        .WillOnce(DoAll(SetArgPointee<0>(status), Return(0)));  

    rbusProperty_t prop = nullptr; 
    rbusGetHandlerOptions_t opts = {};
    rbusError_t result = TR104Services_TableHandler(nullptr, prop, &opts);

    EXPECT_EQ(result, RBUS_ERROR_ACCESS_NOT_ALLOWED);
}

TEST_F(CcspMtaTestFixture, TR104Services_TableHandler_Provisioned_Success) {

    MTAMGMT_MTA_PROVISION_STATUS status = MTA_PROVISIONED; 

    EXPECT_CALL(*g_mtaHALMock, mta_hal_getMtaProvisioningStatus(_))
        .WillOnce(DoAll(SetArgPointee<0>(status), Return(0))); 

    char* paramValues[] = { strdup("ParamName1,string,ParamValue1") };
    EXPECT_CALL(*g_mtaHALMock, mta_hal_getTR104parameterValues(_, _, _))
        .WillOnce(DoAll(SetArgPointee<1>(1), SetArgPointee<2>(paramValues), Return(0)));

    EXPECT_CALL(*g_rbusMock, rbusValue_SetFromString(_, RBUS_STRING, StrEq("ParamValue1")))
        .Times(1);

    EXPECT_CALL(*g_rbusMock, rbusProperty_Init(_, StrEq("ParamName1"), _))
        .Times(1);

    EXPECT_CALL(*g_rbusMock, rbusProperty_GetName(_)).Times(AnyNumber());
    EXPECT_CALL(*g_rbusMock, rbusValue_Init(_)).Times(AnyNumber());
    EXPECT_CALL(*g_rbusMock, rbusProperty_SetValue(_, _)).Times(AnyNumber());
    EXPECT_CALL(*g_rbusMock, rbusProperty_Release(_)).Times(AnyNumber());
    EXPECT_CALL(*g_rbusMock, rbusValue_Release(_)).Times(AnyNumber());
    EXPECT_CALL(*g_mtaHALMock, mta_hal_freeTR104parameterValues(_, _)).Times(AnyNumber());

    EXPECT_CALL(*g_rbusMock, rbusProperty_Append(_, _))
        .Times(1); 

    rbusProperty_t prop = nullptr; 
    rbusGetHandlerOptions_t opts = {};
    rbusError_t result = TR104Services_TableHandler(nullptr, prop, &opts);

    EXPECT_EQ(result, RBUS_ERROR_SUCCESS);
    free(paramValues[0]);
}

TEST_F(CcspMtaTestFixture, TR104Services_TableHandler_HALFailure) {

    MTAMGMT_MTA_PROVISION_STATUS status = MTA_PROVISIONED; 

    EXPECT_CALL(*g_mtaHALMock, mta_hal_getMtaProvisioningStatus(_))
        .WillOnce(DoAll(SetArgPointee<0>(status), Return(0))); 

    EXPECT_CALL(*g_mtaHALMock, mta_hal_getTR104parameterValues(_, _, _))
        .WillOnce(Return(-1)); 

    EXPECT_CALL(*g_rbusMock, rbusProperty_GetName(_)).Times(AnyNumber());    

    rbusProperty_t prop = nullptr;
    rbusGetHandlerOptions_t opts = {};
    rbusError_t result = TR104Services_TableHandler(nullptr, prop, &opts);

    EXPECT_EQ(result, RBUS_ERROR_DESTINATION_RESPONSE_FAILURE);
}

TEST_F(CcspMtaTestFixture, TR104Services_TableHandler_StringType) {
    char* paramValues[] = { strdup("ParamName1,string,ParamValue1") };

    EXPECT_CALL(*g_mtaHALMock, mta_hal_getMtaProvisioningStatus(_))
        .WillOnce(DoAll(SetArgPointee<0>(MTA_PROVISIONED), Return(0)));

    EXPECT_CALL(*g_mtaHALMock, mta_hal_getTR104parameterValues(_, _, _))
        .WillOnce(DoAll(SetArgPointee<1>(1), SetArgPointee<2>(paramValues), Return(0)));

    EXPECT_CALL(*g_rbusMock, rbusProperty_Init(_, _, _))
        .WillOnce(Return((rbusProperty_t)malloc(sizeof(rbusProperty_t))));
    
    EXPECT_CALL(*g_rbusMock, rbusValue_SetFromString(_, RBUS_STRING, _))
        .Times(1);

    EXPECT_CALL(*g_rbusMock, rbusProperty_GetName(_)).Times(AnyNumber());
    EXPECT_CALL(*g_rbusMock, rbusValue_Init(_)).Times(AnyNumber());
    EXPECT_CALL(*g_rbusMock, rbusProperty_SetValue(_, _)).Times(AnyNumber());
    EXPECT_CALL(*g_rbusMock, rbusProperty_Append(_, _)).Times(AnyNumber());
    EXPECT_CALL(*g_rbusMock, rbusProperty_Release(_)).Times(AnyNumber());
    EXPECT_CALL(*g_rbusMock, rbusValue_Release(_)).Times(AnyNumber());
    EXPECT_CALL(*g_mtaHALMock, mta_hal_freeTR104parameterValues(_, _)).Times(AnyNumber());

    rbusProperty_t prop = nullptr;
    rbusGetHandlerOptions_t opts = {};
    rbusError_t result = TR104Services_TableHandler(nullptr, prop, &opts);

    EXPECT_EQ(result, RBUS_ERROR_SUCCESS);
    
    free(paramValues[0]);
}


TEST_F(CcspMtaTestFixture, TR104Services_TableHandler_IntType) {
    char* paramValues[] = { strdup("ParamName1,int,123") };

    EXPECT_CALL(*g_mtaHALMock, mta_hal_getMtaProvisioningStatus(_))
        .WillOnce(DoAll(SetArgPointee<0>(MTA_PROVISIONED), Return(0)));

    EXPECT_CALL(*g_mtaHALMock, mta_hal_getTR104parameterValues(_, _, _))
        .WillOnce(DoAll(SetArgPointee<1>(1), SetArgPointee<2>(paramValues), Return(0)));

    EXPECT_CALL(*g_rbusMock, rbusProperty_Init(_, _, _))
        .WillOnce(Return((rbusProperty_t)malloc(sizeof(rbusProperty_t))));    

    EXPECT_CALL(*g_rbusMock, rbusValue_SetFromString(_, RBUS_INT32, _))
        .Times(1);

    EXPECT_CALL(*g_rbusMock, rbusProperty_GetName(_)).Times(AnyNumber());
    EXPECT_CALL(*g_rbusMock, rbusValue_Init(_)).Times(AnyNumber());
    EXPECT_CALL(*g_rbusMock, rbusProperty_SetValue(_, _)).Times(AnyNumber());
    EXPECT_CALL(*g_rbusMock, rbusProperty_Append(_, _)).Times(AnyNumber());
    EXPECT_CALL(*g_rbusMock, rbusProperty_Release(_)).Times(AnyNumber());
    EXPECT_CALL(*g_rbusMock, rbusValue_Release(_)).Times(AnyNumber());
    EXPECT_CALL(*g_mtaHALMock, mta_hal_freeTR104parameterValues(_, _)).Times(AnyNumber());

    rbusProperty_t prop = nullptr;
    rbusGetHandlerOptions_t opts = {};
    rbusError_t result = TR104Services_TableHandler(nullptr, prop, &opts);

    EXPECT_EQ(result, RBUS_ERROR_SUCCESS);
    
    free(paramValues[0]);
}
TEST_F(CcspMtaTestFixture, TR104Services_TableHandler_UnsignedInt) {
    MTAMGMT_MTA_PROVISION_STATUS status = MTA_PROVISIONED;

    EXPECT_CALL(*g_mtaHALMock, mta_hal_getMtaProvisioningStatus(_))
        .WillOnce(DoAll(SetArgPointee<0>(status), Return(0))); 


     EXPECT_CALL(*g_rbusMock, rbusProperty_Init(_, _, _))
        .WillOnce(Return((rbusProperty_t)malloc(sizeof(rbusProperty_t))));
    
    EXPECT_CALL(*g_rbusMock, rbusValue_SetFromString(_, RBUS_UINT32, _))
        .Times(1);    

    char* paramValues[] = { strdup("ParamName1,unsignedInt,12345") };
    EXPECT_CALL(*g_mtaHALMock, mta_hal_getTR104parameterValues(_, _, _))
        .WillOnce(DoAll(SetArgPointee<1>(1), SetArgPointee<2>(paramValues), Return(0)));

    EXPECT_CALL(*g_rbusMock, rbusProperty_GetName(_)).Times(AnyNumber());
    EXPECT_CALL(*g_rbusMock, rbusValue_Init(_)).Times(AnyNumber());
    EXPECT_CALL(*g_rbusMock, rbusProperty_SetValue(_, _)).Times(AnyNumber());
    EXPECT_CALL(*g_rbusMock, rbusProperty_Append(_, _)).Times(AnyNumber());
    EXPECT_CALL(*g_rbusMock, rbusProperty_Release(_)).Times(AnyNumber());
    EXPECT_CALL(*g_rbusMock, rbusValue_Release(_)).Times(AnyNumber());
    EXPECT_CALL(*g_mtaHALMock, mta_hal_freeTR104parameterValues(_, _)).Times(AnyNumber());

    rbusProperty_t prop = nullptr;
    rbusGetHandlerOptions_t opts = {};
    rbusError_t result = TR104Services_TableHandler(nullptr, prop, &opts);

    EXPECT_EQ(result, RBUS_ERROR_SUCCESS);
    free(paramValues[0]);
}

TEST_F(CcspMtaTestFixture, TR104Services_TableHandler_UnsignedLong) {
    MTAMGMT_MTA_PROVISION_STATUS status = MTA_PROVISIONED;

    EXPECT_CALL(*g_mtaHALMock, mta_hal_getMtaProvisioningStatus(_))
        .WillOnce(DoAll(SetArgPointee<0>(status), Return(0))); 

    char* paramValues[] = { strdup("ParamName1,unsignedLong,1234567890") };
    EXPECT_CALL(*g_mtaHALMock, mta_hal_getTR104parameterValues(_, _, _))
        .WillOnce(DoAll(SetArgPointee<1>(1), SetArgPointee<2>(paramValues), Return(0)));

     EXPECT_CALL(*g_rbusMock, rbusProperty_Init(_, _, _))
        .WillOnce(Return((rbusProperty_t)malloc(sizeof(rbusProperty_t))));
    
    EXPECT_CALL(*g_rbusMock, rbusValue_SetFromString(_, RBUS_UINT64, _))
        .Times(1);        

    EXPECT_CALL(*g_rbusMock, rbusProperty_GetName(_)).Times(AnyNumber());
    EXPECT_CALL(*g_rbusMock, rbusValue_Init(_)).Times(AnyNumber());
    EXPECT_CALL(*g_rbusMock, rbusProperty_SetValue(_, _)).Times(AnyNumber());
    EXPECT_CALL(*g_rbusMock, rbusProperty_Append(_, _)).Times(AnyNumber());
    EXPECT_CALL(*g_rbusMock, rbusProperty_Release(_)).Times(AnyNumber());
    EXPECT_CALL(*g_rbusMock, rbusValue_Release(_)).Times(AnyNumber());
    EXPECT_CALL(*g_mtaHALMock, mta_hal_freeTR104parameterValues(_, _)).Times(AnyNumber());

    rbusProperty_t prop = nullptr;
    rbusGetHandlerOptions_t opts = {};
    rbusError_t result = TR104Services_TableHandler(nullptr, prop, &opts);

    EXPECT_EQ(result, RBUS_ERROR_SUCCESS);

    free(paramValues[0]);
}

TEST_F(CcspMtaTestFixture, TR104Services_TableHandler_Long) {
    MTAMGMT_MTA_PROVISION_STATUS status = MTA_PROVISIONED;

    EXPECT_CALL(*g_mtaHALMock, mta_hal_getMtaProvisioningStatus(_))
        .WillOnce(DoAll(SetArgPointee<0>(status), Return(0))); 

    char* paramValues[] = { strdup("ParamName1,long,-1234567890") };
    EXPECT_CALL(*g_mtaHALMock, mta_hal_getTR104parameterValues(_, _, _))
        .WillOnce(DoAll(SetArgPointee<1>(1), SetArgPointee<2>(paramValues), Return(0)));

     EXPECT_CALL(*g_rbusMock, rbusProperty_Init(_, _, _))
        .WillOnce(Return((rbusProperty_t)malloc(sizeof(rbusProperty_t))));
    
    EXPECT_CALL(*g_rbusMock, rbusValue_SetFromString(_, RBUS_INT64, _))
        .Times(1);      

    EXPECT_CALL(*g_rbusMock, rbusProperty_GetName(_)).Times(AnyNumber());
    EXPECT_CALL(*g_rbusMock, rbusValue_Init(_)).Times(AnyNumber());
    EXPECT_CALL(*g_rbusMock, rbusProperty_SetValue(_, _)).Times(AnyNumber());
    EXPECT_CALL(*g_rbusMock, rbusProperty_Append(_, _)).Times(AnyNumber());
    EXPECT_CALL(*g_rbusMock, rbusProperty_Release(_)).Times(AnyNumber());
    EXPECT_CALL(*g_rbusMock, rbusValue_Release(_)).Times(AnyNumber());
    EXPECT_CALL(*g_mtaHALMock, mta_hal_freeTR104parameterValues(_, _)).Times(AnyNumber());

    rbusProperty_t prop = nullptr;
    rbusGetHandlerOptions_t opts = {};
    rbusError_t result = TR104Services_TableHandler(nullptr, prop, &opts);

    EXPECT_EQ(result, RBUS_ERROR_SUCCESS);
    free(paramValues[0]);
}

TEST_F(CcspMtaTestFixture, TR104Services_TableHandler_Boolean) {
    MTAMGMT_MTA_PROVISION_STATUS status = MTA_PROVISIONED;

    EXPECT_CALL(*g_mtaHALMock, mta_hal_getMtaProvisioningStatus(_))
        .WillOnce(DoAll(SetArgPointee<0>(status), Return(0))); 

    char* paramValues[] = { strdup("ParamName1,boolean,true") };
    EXPECT_CALL(*g_mtaHALMock, mta_hal_getTR104parameterValues(_, _, _))
        .WillOnce(DoAll(SetArgPointee<1>(1), SetArgPointee<2>(paramValues), Return(0)));


    EXPECT_CALL(*g_rbusMock, rbusProperty_Init(_, _, _))
        .WillOnce(Return((rbusProperty_t)malloc(sizeof(rbusProperty_t))));
    
    EXPECT_CALL(*g_rbusMock, rbusValue_SetFromString(_, RBUS_BOOLEAN, _))
        .Times(1);         

    EXPECT_CALL(*g_rbusMock, rbusProperty_GetName(_)).Times(AnyNumber());
    EXPECT_CALL(*g_rbusMock, rbusValue_Init(_)).Times(AnyNumber());
    EXPECT_CALL(*g_rbusMock, rbusProperty_SetValue(_, _)).Times(AnyNumber());
    EXPECT_CALL(*g_rbusMock, rbusProperty_Append(_, _)).Times(AnyNumber());
    EXPECT_CALL(*g_rbusMock, rbusProperty_Release(_)).Times(AnyNumber());
    EXPECT_CALL(*g_rbusMock, rbusValue_Release(_)).Times(AnyNumber());
    EXPECT_CALL(*g_mtaHALMock, mta_hal_freeTR104parameterValues(_, _)).Times(AnyNumber());

    rbusProperty_t prop = nullptr;
    rbusGetHandlerOptions_t opts = {};
    rbusError_t result = TR104Services_TableHandler(nullptr, prop, &opts);

    EXPECT_EQ(result, RBUS_ERROR_SUCCESS);

    free(paramValues[0]);
}

TEST_F(CcspMtaTestFixture, TR104Services_TableHandler_DateTime) {
    MTAMGMT_MTA_PROVISION_STATUS status = MTA_PROVISIONED;

    EXPECT_CALL(*g_mtaHALMock, mta_hal_getMtaProvisioningStatus(_))
        .WillOnce(DoAll(SetArgPointee<0>(status), Return(0)));

    char* paramValues[] = { strdup("ParamName1,datetime,2024-01-01T12:00:00Z") };
    EXPECT_CALL(*g_mtaHALMock, mta_hal_getTR104parameterValues(_, _, _))
        .WillOnce(DoAll(SetArgPointee<1>(1), SetArgPointee<2>(paramValues), Return(0)));

    EXPECT_CALL(*g_rbusMock, rbusProperty_Init(_, _, _))
        .WillOnce(Return((rbusProperty_t)malloc(sizeof(rbusProperty_t))));
    
    EXPECT_CALL(*g_rbusMock, rbusValue_SetFromString(_, RBUS_DATETIME, _))
        .Times(1);         

    EXPECT_CALL(*g_rbusMock, rbusProperty_GetName(_)).Times(AnyNumber());
    EXPECT_CALL(*g_rbusMock, rbusValue_Init(_)).Times(AnyNumber());
    EXPECT_CALL(*g_rbusMock, rbusProperty_SetValue(_, _)).Times(AnyNumber());
    EXPECT_CALL(*g_rbusMock, rbusProperty_Append(_, _)).Times(AnyNumber());
    EXPECT_CALL(*g_rbusMock, rbusProperty_Release(_)).Times(AnyNumber());
    EXPECT_CALL(*g_rbusMock, rbusValue_Release(_)).Times(AnyNumber());
    EXPECT_CALL(*g_mtaHALMock, mta_hal_freeTR104parameterValues(_, _)).Times(AnyNumber());

    rbusProperty_t prop = nullptr;
    rbusGetHandlerOptions_t opts = {};
    rbusError_t result = TR104Services_TableHandler(nullptr, prop, &opts);

    EXPECT_EQ(result, RBUS_ERROR_SUCCESS);

    free(paramValues[0]);
}

TEST_F(CcspMtaTestFixture, TR104Services_TableHandler_InvalidPropertyType) {
    MTAMGMT_MTA_PROVISION_STATUS status = MTA_PROVISIONED;

    EXPECT_CALL(*g_mtaHALMock, mta_hal_getMtaProvisioningStatus(_))
        .WillOnce(DoAll(SetArgPointee<0>(status), Return(0))); 


    char* paramValues[] = { strdup("ParamName1,unsupportedType,ParamValue1") };
    EXPECT_CALL(*g_mtaHALMock, mta_hal_getTR104parameterValues(_, _, _))
        .WillOnce(DoAll(SetArgPointee<1>(1), SetArgPointee<2>(paramValues), Return(0)));

   
    EXPECT_CALL(*g_rbusMock, rbusProperty_GetName(_)).Times(AnyNumber());
    EXPECT_CALL(*g_rbusMock, rbusValue_Init(_)).Times(AnyNumber());
    EXPECT_CALL(*g_rbusMock, rbusProperty_SetValue(_, _)).Times(AnyNumber());
    EXPECT_CALL(*g_rbusMock, rbusProperty_Append(_, _)).Times(AnyNumber());
    EXPECT_CALL(*g_rbusMock, rbusProperty_Release(_)).Times(AnyNumber());
    EXPECT_CALL(*g_rbusMock, rbusValue_Release(_)).Times(AnyNumber());
    EXPECT_CALL(*g_mtaHALMock, mta_hal_freeTR104parameterValues(_, _)).Times(AnyNumber());

    rbusProperty_t prop = nullptr;
    rbusGetHandlerOptions_t opts = {};
    rbusError_t result = TR104Services_TableHandler(nullptr, prop, &opts);
    EXPECT_EQ(result, RBUS_ERROR_SUCCESS);

    free(paramValues[0]);
}

TEST_F(CcspMtaTestFixture, TR104Services_TableHandler_EmptyPropertyValue) {
    MTAMGMT_MTA_PROVISION_STATUS status = MTA_PROVISIONED;

    EXPECT_CALL(*g_mtaHALMock, mta_hal_getMtaProvisioningStatus(_))
        .WillOnce(DoAll(SetArgPointee<0>(status), Return(0))); 


    char* paramValues[] = { strdup("ParamName1,string,") };
    EXPECT_CALL(*g_mtaHALMock, mta_hal_getTR104parameterValues(_, _, _))
        .WillOnce(DoAll(SetArgPointee<1>(1), SetArgPointee<2>(paramValues), Return(0)));

    EXPECT_CALL(*g_rbusMock, rbusProperty_Init(_, _, _))
        .WillOnce(Return((rbusProperty_t)malloc(sizeof(rbusProperty_t))));
    
    EXPECT_CALL(*g_rbusMock, rbusValue_SetFromString(_, RBUS_STRING, _))
        .Times(1);         

   
    EXPECT_CALL(*g_rbusMock, rbusProperty_GetName(_)).Times(AnyNumber());
    EXPECT_CALL(*g_rbusMock, rbusValue_Init(_)).Times(AnyNumber());
    EXPECT_CALL(*g_rbusMock, rbusProperty_SetValue(_, _)).Times(AnyNumber());
    EXPECT_CALL(*g_rbusMock, rbusProperty_Append(_, _)).Times(AnyNumber());
    EXPECT_CALL(*g_rbusMock, rbusProperty_Release(_)).Times(AnyNumber());
    EXPECT_CALL(*g_rbusMock, rbusValue_Release(_)).Times(AnyNumber());
    EXPECT_CALL(*g_mtaHALMock, mta_hal_freeTR104parameterValues(_, _)).Times(AnyNumber());

    rbusProperty_t prop = nullptr;
    rbusGetHandlerOptions_t opts = {};
    rbusError_t result = TR104Services_TableHandler(nullptr, prop, &opts);

    EXPECT_EQ(result, RBUS_ERROR_SUCCESS);

    free(paramValues[0]);
}

TEST_F(CcspMtaTestFixture, TR104Services_TableHandler_FreeTR104ParameterValues) {
    MTAMGMT_MTA_PROVISION_STATUS status = MTA_PROVISIONED;

    EXPECT_CALL(*g_mtaHALMock, mta_hal_getMtaProvisioningStatus(_))
        .WillOnce(DoAll(SetArgPointee<0>(status), Return(0)));  

    char* paramValues[] = { strdup("ParamName1,string,ParamValue1") };
    EXPECT_CALL(*g_mtaHALMock, mta_hal_getTR104parameterValues(_, _, _))
        .WillOnce(DoAll(SetArgPointee<1>(1), SetArgPointee<2>(paramValues), Return(0)));

    EXPECT_CALL(*g_mtaHALMock, mta_hal_freeTR104parameterValues(_, _))
        .Times(1); 

    EXPECT_CALL(*g_rbusMock, rbusProperty_Init(_, _, _))
        .WillOnce(Return((rbusProperty_t)malloc(sizeof(rbusProperty_t))));
    
    EXPECT_CALL(*g_rbusMock, rbusValue_SetFromString(_, RBUS_STRING, _))
        .Times(1);         

   
    EXPECT_CALL(*g_rbusMock, rbusProperty_GetName(_)).Times(AnyNumber());
    EXPECT_CALL(*g_rbusMock, rbusValue_Init(_)).Times(AnyNumber());
    EXPECT_CALL(*g_rbusMock, rbusProperty_SetValue(_, _)).Times(AnyNumber());
    EXPECT_CALL(*g_rbusMock, rbusProperty_Append(_, _)).Times(AnyNumber());
    EXPECT_CALL(*g_rbusMock, rbusProperty_Release(_)).Times(AnyNumber());
    EXPECT_CALL(*g_rbusMock, rbusValue_Release(_)).Times(AnyNumber());

    rbusProperty_t prop = nullptr;
    rbusGetHandlerOptions_t opts = {};
    rbusError_t result = TR104Services_TableHandler(nullptr, prop, &opts);

    EXPECT_EQ(result, RBUS_ERROR_SUCCESS);

    free(paramValues[0]);
}



//Test cases for TR104Services_GetHandler
TEST_F(CcspMtaTestFixture, TR104Services_GetHandler_ProvisionStatusFail) {
    rbusProperty_t inProperty = nullptr;
    rbusGetHandlerOptions_t opts = {};

    EXPECT_CALL(*g_mtaHALMock, mta_hal_getMtaProvisioningStatus(_))
        .WillOnce(Return(1));

    rbusError_t result = TR104Services_GetHandler(nullptr, inProperty, &opts);

    EXPECT_EQ(result, RBUS_ERROR_DESTINATION_RESPONSE_FAILURE);
}

TEST_F(CcspMtaTestFixture, TR104Services_GetHandler_NonProvisioned) {
    rbusProperty_t inProperty = nullptr;
    rbusGetHandlerOptions_t opts = {};

    EXPECT_CALL(*g_mtaHALMock, mta_hal_getMtaProvisioningStatus(_))
        .WillOnce(DoAll(SetArgPointee<0>(MTA_NON_PROVISIONED), Return(0)));

    rbusError_t result = TR104Services_GetHandler(nullptr, inProperty, &opts);

    EXPECT_EQ(result, RBUS_ERROR_ACCESS_NOT_ALLOWED);
}

TEST_F(CcspMtaTestFixture, TR104Services_GetHandler_GetBooleanValueSuccess) {
    rbusProperty_t inProperty = nullptr;
    rbusGetHandlerOptions_t opts = {};
    char* paramValues[] = { strdup("ParamName1,boolean,true") };

    EXPECT_CALL(*g_mtaHALMock, mta_hal_getMtaProvisioningStatus(_))
        .WillOnce(DoAll(SetArgPointee<0>(MTA_PROVISIONED), Return(0)));

    EXPECT_CALL(*g_rbusMock, rbusProperty_GetName(_))
        .WillRepeatedly(Return("ParamName1"));    

    EXPECT_CALL(*g_mtaHALMock, mta_hal_getTR104parameterValues(_, _, _))
        .WillOnce(DoAll(SetArgPointee<1>(1), SetArgPointee<2>(paramValues), Return(0)));

    EXPECT_CALL(*g_rbusMock, rbusValue_SetFromString(_, RBUS_BOOLEAN, StrEq("true")))
        .Times(1);

    EXPECT_CALL(*g_rbusMock, rbusValue_Init(_)).Times(1);
    EXPECT_CALL(*g_rbusMock, rbusProperty_SetValue(_, _)).Times(1);
    EXPECT_CALL(*g_rbusMock, rbusValue_Release(_)).Times(1);
    EXPECT_CALL(*g_mtaHALMock, mta_hal_freeTR104parameterValues(_, _)).Times(1);

    rbusError_t result = TR104Services_GetHandler(nullptr, inProperty, &opts);

    EXPECT_EQ(result, RBUS_ERROR_SUCCESS);

    free(paramValues[0]);
}

TEST_F(CcspMtaTestFixture, TR104Services_GetHandler_GetStringValueSuccess) {
    rbusProperty_t inProperty = nullptr;
    rbusGetHandlerOptions_t opts = {};
    char* paramValues[] = { strdup("ParamName1,string,Test_Value") };

    EXPECT_CALL(*g_mtaHALMock, mta_hal_getMtaProvisioningStatus(_))
        .WillOnce(DoAll(SetArgPointee<0>(MTA_PROVISIONED), Return(0)));

     EXPECT_CALL(*g_rbusMock, rbusProperty_GetName(_))
        .WillRepeatedly(Return("ParamName1"));    

    EXPECT_CALL(*g_mtaHALMock, mta_hal_getTR104parameterValues(_, _, _))
        .WillOnce(DoAll(SetArgPointee<1>(1), SetArgPointee<2>(paramValues), Return(0)));

    EXPECT_CALL(*g_rbusMock, rbusValue_SetFromString(_, RBUS_STRING, StrEq("Test_Value")))
        .Times(1);

    EXPECT_CALL(*g_rbusMock, rbusValue_Init(_)).Times(1);
    EXPECT_CALL(*g_rbusMock, rbusProperty_SetValue(_, _)).Times(1);
    EXPECT_CALL(*g_rbusMock, rbusValue_Release(_)).Times(1);
    EXPECT_CALL(*g_mtaHALMock, mta_hal_freeTR104parameterValues(_, _)).Times(1);

    rbusError_t result = TR104Services_GetHandler(nullptr, inProperty, &opts);

    EXPECT_EQ(result, RBUS_ERROR_SUCCESS);

    free(paramValues[0]);
}

TEST_F(CcspMtaTestFixture, TR104Services_GetHandler_HALGetFailure) {
    rbusProperty_t inProperty = nullptr;
    rbusGetHandlerOptions_t opts = {};

    EXPECT_CALL(*g_mtaHALMock, mta_hal_getMtaProvisioningStatus(_))
        .WillOnce(DoAll(SetArgPointee<0>(MTA_PROVISIONED), Return(0)));

    EXPECT_CALL(*g_rbusMock, rbusProperty_GetName(_))
        .WillRepeatedly(Return("ParamName1"));    

    EXPECT_CALL(*g_mtaHALMock, mta_hal_getTR104parameterValues(_, _, _))
        .WillOnce(Return(-1));

    EXPECT_CALL(*g_rbusMock, rbusValue_Init(_)).Times(1);    

    rbusError_t result = TR104Services_GetHandler(nullptr, inProperty, &opts);

    EXPECT_EQ(result, RBUS_ERROR_DESTINATION_RESPONSE_FAILURE);
}

TEST_F(CcspMtaTestFixture, TR104Services_GetHandler_GetDateTimeValueSuccess) {
    rbusProperty_t inProperty = nullptr;
    rbusGetHandlerOptions_t opts = {};
    char* paramValues[] = { strdup("ParamName1,dateTime,2023-10-10T12:00:00") };

    EXPECT_CALL(*g_mtaHALMock, mta_hal_getMtaProvisioningStatus(_))
        .WillOnce(DoAll(SetArgPointee<0>(MTA_PROVISIONED), Return(0)));

    EXPECT_CALL(*g_mtaHALMock, mta_hal_getTR104parameterValues(_, _, _))
        .WillOnce(DoAll(SetArgPointee<1>(1), SetArgPointee<2>(paramValues), Return(0)));

    EXPECT_CALL(*g_rbusMock, rbusProperty_GetName(_))
        .WillRepeatedly(Return("ParamName1"));    

    EXPECT_CALL(*g_rbusMock, rbusValue_SetFromString(_, RBUS_DATETIME, StrEq("2023-10-10T12:00:00")))
        .Times(1);

    EXPECT_CALL(*g_rbusMock, rbusValue_Init(_)).Times(1);
    EXPECT_CALL(*g_rbusMock, rbusProperty_SetValue(_, _)).Times(1);
    EXPECT_CALL(*g_rbusMock, rbusValue_Release(_)).Times(1);
    EXPECT_CALL(*g_mtaHALMock, mta_hal_freeTR104parameterValues(_, _)).Times(1);

    rbusError_t result = TR104Services_GetHandler(nullptr, inProperty, &opts);

    EXPECT_EQ(result, RBUS_ERROR_SUCCESS);

    free(paramValues[0]);
}

TEST_F(CcspMtaTestFixture, TR104Services_GetHandler_GetInt32ValueSuccess) {
    rbusProperty_t inProperty = nullptr;
    rbusGetHandlerOptions_t opts = {};
    char* paramValues[] = { strdup("ParamName1,int,12345") };

    EXPECT_CALL(*g_mtaHALMock, mta_hal_getMtaProvisioningStatus(_))
        .WillOnce(DoAll(SetArgPointee<0>(MTA_PROVISIONED), Return(0)));

    EXPECT_CALL(*g_mtaHALMock, mta_hal_getTR104parameterValues(_, _, _))
        .WillOnce(DoAll(SetArgPointee<1>(1), SetArgPointee<2>(paramValues), Return(0)));

    EXPECT_CALL(*g_rbusMock, rbusProperty_GetName(_))
        .WillRepeatedly(Return("ParamName1"));    

    EXPECT_CALL(*g_rbusMock, rbusValue_SetFromString(_, RBUS_INT32, StrEq("12345")))
        .Times(1);

    EXPECT_CALL(*g_rbusMock, rbusValue_Init(_)).Times(1);
    EXPECT_CALL(*g_rbusMock, rbusProperty_SetValue(_, _)).Times(1);
    EXPECT_CALL(*g_rbusMock, rbusValue_Release(_)).Times(1);
    EXPECT_CALL(*g_mtaHALMock, mta_hal_freeTR104parameterValues(_, _)).Times(1);

    rbusError_t result = TR104Services_GetHandler(nullptr, inProperty, &opts);

    EXPECT_EQ(result, RBUS_ERROR_SUCCESS);

    free(paramValues[0]);
}

TEST_F(CcspMtaTestFixture, TR104Services_GetHandler_GetUnsignedInt32ValueSuccess) {
    rbusProperty_t inProperty = nullptr;
    rbusGetHandlerOptions_t opts = {};
    char* paramValues[] = { strdup("ParamName1,unsignedint,67890") };

    EXPECT_CALL(*g_mtaHALMock, mta_hal_getMtaProvisioningStatus(_))
        .WillOnce(DoAll(SetArgPointee<0>(MTA_PROVISIONED), Return(0)));

    EXPECT_CALL(*g_mtaHALMock, mta_hal_getTR104parameterValues(_, _, _))
        .WillOnce(DoAll(SetArgPointee<1>(1), SetArgPointee<2>(paramValues), Return(0)));

    EXPECT_CALL(*g_rbusMock, rbusProperty_GetName(_))
        .WillRepeatedly(Return("ParamName1"));    

    EXPECT_CALL(*g_rbusMock, rbusValue_SetFromString(_, RBUS_UINT32, StrEq("67890")))
        .Times(1);

    EXPECT_CALL(*g_rbusMock, rbusValue_Init(_)).Times(1);
    EXPECT_CALL(*g_rbusMock, rbusProperty_SetValue(_, _)).Times(1);
    EXPECT_CALL(*g_rbusMock, rbusValue_Release(_)).Times(1);
    EXPECT_CALL(*g_mtaHALMock, mta_hal_freeTR104parameterValues(_, _)).Times(1);

    rbusError_t result = TR104Services_GetHandler(nullptr, inProperty, &opts);

    EXPECT_EQ(result, RBUS_ERROR_SUCCESS);

    free(paramValues[0]);
}

TEST_F(CcspMtaTestFixture, TR104Services_GetHandler_GetInt64ValueSuccess) {
    rbusProperty_t inProperty = nullptr;
    rbusGetHandlerOptions_t opts = {};
    char* paramValues[] = { strdup("ParamName1,long,9876543210") };

    EXPECT_CALL(*g_mtaHALMock, mta_hal_getMtaProvisioningStatus(_))
        .WillOnce(DoAll(SetArgPointee<0>(MTA_PROVISIONED), Return(0)));

    EXPECT_CALL(*g_mtaHALMock, mta_hal_getTR104parameterValues(_, _, _))
        .WillOnce(DoAll(SetArgPointee<1>(1), SetArgPointee<2>(paramValues), Return(0)));

    EXPECT_CALL(*g_rbusMock, rbusProperty_GetName(_))
        .WillRepeatedly(Return("ParamName1"));    

    EXPECT_CALL(*g_rbusMock, rbusValue_SetFromString(_, RBUS_INT64, StrEq("9876543210")))
        .Times(1);

    EXPECT_CALL(*g_rbusMock, rbusValue_Init(_)).Times(1);
    EXPECT_CALL(*g_rbusMock, rbusProperty_SetValue(_, _)).Times(1);
    EXPECT_CALL(*g_rbusMock, rbusValue_Release(_)).Times(1);
    EXPECT_CALL(*g_mtaHALMock, mta_hal_freeTR104parameterValues(_, _)).Times(1);

    rbusError_t result = TR104Services_GetHandler(nullptr, inProperty, &opts);

    EXPECT_EQ(result, RBUS_ERROR_SUCCESS);

    free(paramValues[0]);
}

TEST_F(CcspMtaTestFixture, TR104Services_GetHandler_GetUnsignedInt64ValueSuccess) {
    rbusProperty_t inProperty = nullptr;
    rbusGetHandlerOptions_t opts = {};
    char* paramValues[] = { strdup("ParamName1,unsignedLong,1234567890123") };

    EXPECT_CALL(*g_mtaHALMock, mta_hal_getMtaProvisioningStatus(_))
        .WillOnce(DoAll(SetArgPointee<0>(MTA_PROVISIONED), Return(0)));

    EXPECT_CALL(*g_mtaHALMock, mta_hal_getTR104parameterValues(_, _, _))
        .WillOnce(DoAll(SetArgPointee<1>(1), SetArgPointee<2>(paramValues), Return(0)));

    EXPECT_CALL(*g_rbusMock, rbusProperty_GetName(_))
        .WillRepeatedly(Return("ParamName1"));    

    EXPECT_CALL(*g_rbusMock, rbusValue_SetFromString(_, RBUS_UINT64, StrEq("1234567890123")))
        .Times(1);

    EXPECT_CALL(*g_rbusMock, rbusValue_Init(_)).Times(1);
    EXPECT_CALL(*g_rbusMock, rbusProperty_SetValue(_, _)).Times(1);
    EXPECT_CALL(*g_rbusMock, rbusValue_Release(_)).Times(1);
    EXPECT_CALL(*g_mtaHALMock, mta_hal_freeTR104parameterValues(_, _)).Times(1);

    rbusError_t result = TR104Services_GetHandler(nullptr, inProperty, &opts);

    EXPECT_EQ(result, RBUS_ERROR_SUCCESS);

    free(paramValues[0]);
}
//Test cases for TR104Services_SetHandler
TEST_F(CcspMtaTestFixture, TR104Services_SetHandler_Failure_ProvisioningStatus) {
    EXPECT_CALL(*g_mtaHALMock, mta_hal_getMtaProvisioningStatus(_))
        .WillOnce(Return(-1));  
    rbusProperty_t inProperty = nullptr;  
    rbusSetHandlerOptions_t opts = {};
    rbusError_t result = TR104Services_SetHandler(nullptr, inProperty, &opts);

    EXPECT_EQ(result, RBUS_ERROR_DESTINATION_RESPONSE_FAILURE);  
}

TEST_F(CcspMtaTestFixture, TR104Services_SetHandler_Failure_NotProvisioned) {
    EXPECT_CALL(*g_mtaHALMock, mta_hal_getMtaProvisioningStatus(_))
        .WillOnce(DoAll(SetArgPointee<0>(MTA_NON_PROVISIONED), Return(0)));

    rbusProperty_t inProperty = nullptr; 
    rbusSetHandlerOptions_t opts = {};
    rbusError_t result = TR104Services_SetHandler(nullptr, inProperty, &opts);

    EXPECT_EQ(result, RBUS_ERROR_ACCESS_NOT_ALLOWED); 
}

TEST_F(CcspMtaTestFixture, TR104Services_SetHandler_Success_Boolean) {
    EXPECT_CALL(*g_mtaHALMock, mta_hal_getMtaProvisioningStatus(_))
        .WillOnce(DoAll(SetArgPointee<0>(MTA_PROVISIONED), Return(0)));

    rbusValue_t value = nullptr;
    rbusProperty_t inProperty = nullptr;

    EXPECT_CALL(*g_rbusMock, rbusProperty_GetName(_))
        .WillOnce(Return("ParamName"));
    EXPECT_CALL(*g_rbusMock, rbusProperty_GetValue(_))
        .WillOnce(Return(value));
    EXPECT_CALL(*g_rbusMock, rbusValue_GetType(value))
        .WillOnce(Return(RBUS_BOOLEAN));

    const char* expectedBoolean = "true"; 
    EXPECT_CALL(*g_rbusMock, rbusValue_ToString(value, _, _))
        .WillOnce(Return(strdup(expectedBoolean)));

    EXPECT_CALL(*g_rbusMock, rbusValue_GetBoolean(value))
        .WillOnce(Return(true));     

    EXPECT_CALL(*g_mtaHALMock, mta_hal_setTR104parameterValues(_, _))
        .WillOnce(Return(0)); 
    rbusSetHandlerOptions_t opts = {};
    rbusError_t result = TR104Services_SetHandler(nullptr, inProperty, &opts);

    EXPECT_EQ(result, RBUS_ERROR_SUCCESS); 
}

TEST_F(CcspMtaTestFixture, TR104Services_SetHandler_Failure_HalSet) {
    EXPECT_CALL(*g_mtaHALMock, mta_hal_getMtaProvisioningStatus(_))
        .WillOnce(DoAll(SetArgPointee<0>(MTA_PROVISIONED), Return(0)));

    rbusValue_t value = nullptr; 
    rbusProperty_t inProperty = nullptr;  

    EXPECT_CALL(*g_rbusMock, rbusProperty_GetName(_))
        .WillOnce(Return("ParamName"));
    EXPECT_CALL(*g_rbusMock, rbusProperty_GetValue(_))
        .WillOnce(Return(value));
    EXPECT_CALL(*g_rbusMock, rbusValue_GetType(value))
        .WillOnce(Return(RBUS_BOOLEAN));
    const char* expectedFailureString = "invalid";
    EXPECT_CALL(*g_rbusMock, rbusValue_ToString(value, _, _))
        .WillOnce(Return(strdup(expectedFailureString)));

    EXPECT_CALL(*g_rbusMock, rbusValue_GetBoolean(value))
        .WillOnce(Return(false));     

    EXPECT_CALL(*g_mtaHALMock, mta_hal_setTR104parameterValues(_, _))
        .WillOnce(Return(-1));  
    rbusSetHandlerOptions_t opts = {};
    rbusError_t result = TR104Services_SetHandler(nullptr, inProperty, &opts);

    EXPECT_EQ(result, RBUS_ERROR_INVALID_INPUT);
}

TEST_F(CcspMtaTestFixture, TR104Services_SetHandler_Success_Int32) {
    EXPECT_CALL(*g_mtaHALMock, mta_hal_getMtaProvisioningStatus(_))
        .WillOnce(DoAll(SetArgPointee<0>(MTA_PROVISIONED), Return(0)));

    rbusValue_t value = nullptr; 
    rbusProperty_t inProperty = nullptr;  

    EXPECT_CALL(*g_rbusMock, rbusProperty_GetName(_))
        .WillOnce(Return("ParamName"));
    EXPECT_CALL(*g_rbusMock, rbusProperty_GetValue(_))
        .WillOnce(Return(value));
    EXPECT_CALL(*g_rbusMock, rbusValue_GetType(value))
        .WillOnce(Return(RBUS_INT32));
    EXPECT_CALL(*g_rbusMock, rbusValue_ToString(value, _, _))
        .WillOnce(Return(strdup("1234"))); 

    EXPECT_CALL(*g_mtaHALMock, mta_hal_setTR104parameterValues(_, _))
        .WillOnce(Return(0));

    rbusSetHandlerOptions_t opts = {};
    rbusError_t result = TR104Services_SetHandler(nullptr, inProperty, &opts);

    EXPECT_EQ(result, RBUS_ERROR_SUCCESS); 
}

TEST_F(CcspMtaTestFixture, TR104Services_SetHandler_Success_Uint32) {
    EXPECT_CALL(*g_mtaHALMock, mta_hal_getMtaProvisioningStatus(_))
        .WillOnce(DoAll(SetArgPointee<0>(MTA_PROVISIONED), Return(0)));


    rbusValue_t value = nullptr; 
    rbusProperty_t inProperty = nullptr; 

    EXPECT_CALL(*g_rbusMock, rbusProperty_GetName(_))
        .WillOnce(Return("ParamName"));
    EXPECT_CALL(*g_rbusMock, rbusProperty_GetValue(_))
        .WillOnce(Return(value));
    EXPECT_CALL(*g_rbusMock, rbusValue_GetType(value))
        .WillOnce(Return(RBUS_UINT32));
    EXPECT_CALL(*g_rbusMock, rbusValue_ToString(value, _, _))
        .WillOnce(Return(strdup("1234"))); 

    EXPECT_CALL(*g_mtaHALMock, mta_hal_setTR104parameterValues(_, _))
        .WillOnce(Return(0)); 

    rbusSetHandlerOptions_t opts = {};
    rbusError_t result = TR104Services_SetHandler(nullptr, inProperty, &opts);

    EXPECT_EQ(result, RBUS_ERROR_SUCCESS); 
}

TEST_F(CcspMtaTestFixture, TR104Services_SetHandler_Success_String) {
    EXPECT_CALL(*g_mtaHALMock, mta_hal_getMtaProvisioningStatus(_))
        .WillOnce(DoAll(SetArgPointee<0>(MTA_PROVISIONED), Return(0)));

    rbusValue_t value = nullptr; 
    rbusProperty_t inProperty = nullptr;

    EXPECT_CALL(*g_rbusMock, rbusProperty_GetName(_))
        .WillOnce(Return("ParamName"));
    EXPECT_CALL(*g_rbusMock, rbusProperty_GetValue(_))
        .WillOnce(Return(value));
    EXPECT_CALL(*g_rbusMock, rbusValue_GetType(value))
        .WillOnce(Return(RBUS_STRING));
    
    const char* expectedString = "SampleString";
    EXPECT_CALL(*g_rbusMock, rbusValue_ToString(value, _, _))
        .WillOnce(Return(strdup(expectedString))); 

    EXPECT_CALL(*g_mtaHALMock, mta_hal_setTR104parameterValues(_, _))
        .WillOnce(Return(0)); 

    rbusSetHandlerOptions_t opts = {};
    rbusError_t result = TR104Services_SetHandler(nullptr, inProperty, &opts);

    EXPECT_EQ(result, RBUS_ERROR_SUCCESS); 
}

TEST_F(CcspMtaTestFixture, TR104Services_SetHandler_Success_Datetime) {
    EXPECT_CALL(*g_mtaHALMock, mta_hal_getMtaProvisioningStatus(_))
        .WillOnce(DoAll(SetArgPointee<0>(MTA_PROVISIONED), Return(0)));

    rbusValue_t value = nullptr; 
    rbusProperty_t inProperty = nullptr; 

    EXPECT_CALL(*g_rbusMock, rbusProperty_GetName(_))
        .WillOnce(Return("ParamName"));
    EXPECT_CALL(*g_rbusMock, rbusProperty_GetValue(_))
        .WillOnce(Return(value));
    EXPECT_CALL(*g_rbusMock, rbusValue_GetType(value))
        .WillOnce(Return(RBUS_DATETIME));
    
    const char* expectedDatetime = "2024-10-16T12:34:56Z"; 
    EXPECT_CALL(*g_rbusMock, rbusValue_ToString(value, _, _))
        .WillOnce(Return(strdup(expectedDatetime))); 

    EXPECT_CALL(*g_mtaHALMock, mta_hal_setTR104parameterValues(_, _))
        .WillOnce(Return(0)); 

    rbusSetHandlerOptions_t opts = {};
    rbusError_t result = TR104Services_SetHandler(nullptr, inProperty, &opts);

    EXPECT_EQ(result, RBUS_ERROR_SUCCESS); 
}

TEST_F(CcspMtaTestFixture, TR104Services_SetHandler_Success_Long) {
    EXPECT_CALL(*g_mtaHALMock, mta_hal_getMtaProvisioningStatus(_))
        .WillOnce(DoAll(SetArgPointee<0>(MTA_PROVISIONED), Return(0)));

    rbusValue_t value = nullptr;
    rbusProperty_t inProperty = nullptr;

    EXPECT_CALL(*g_rbusMock, rbusProperty_GetName(_))
        .WillOnce(Return("ParamName"));
    EXPECT_CALL(*g_rbusMock, rbusProperty_GetValue(_))
        .WillOnce(Return(value));
    EXPECT_CALL(*g_rbusMock, rbusValue_GetType(value))
        .WillOnce(Return(RBUS_INT64)); 

    const char* expectedLongString = "123456789";  
    EXPECT_CALL(*g_rbusMock, rbusValue_ToString(value, _, _))
        .WillOnce(Return(strdup(expectedLongString)));

    EXPECT_CALL(*g_mtaHALMock, mta_hal_setTR104parameterValues(_, _))
        .WillOnce(Return(0)); 
    rbusSetHandlerOptions_t opts = {};
    rbusError_t result = TR104Services_SetHandler(nullptr, inProperty, &opts);

    EXPECT_EQ(result, RBUS_ERROR_SUCCESS);
}


TEST_F(CcspMtaTestFixture, TR104Services_SetHandler_Success_UnsignedLong) {
    EXPECT_CALL(*g_mtaHALMock, mta_hal_getMtaProvisioningStatus(_))
        .WillOnce(DoAll(SetArgPointee<0>(MTA_PROVISIONED), Return(0)));

    rbusValue_t value = nullptr; 
    rbusProperty_t inProperty = nullptr;

    EXPECT_CALL(*g_rbusMock, rbusProperty_GetName(_))
        .WillOnce(Return("ParamName"));
    EXPECT_CALL(*g_rbusMock, rbusProperty_GetValue(_))
        .WillOnce(Return(value));
    EXPECT_CALL(*g_rbusMock, rbusValue_GetType(value))
        .WillOnce(Return(RBUS_UINT64));

    const char* expectedUnsignedLongString = "123456789UL"; 
    EXPECT_CALL(*g_rbusMock, rbusValue_ToString(value, _, _))
        .WillOnce(Return(strdup(expectedUnsignedLongString)));

    EXPECT_CALL(*g_mtaHALMock, mta_hal_setTR104parameterValues(_, _))
        .WillOnce(Return(0)); 

    rbusSetHandlerOptions_t opts = {};
    rbusError_t result = TR104Services_SetHandler(nullptr, inProperty, &opts);

    EXPECT_EQ(result, RBUS_ERROR_SUCCESS);  
}

//Test cases for TR104_open
TEST_F(CcspMtaTestFixture, TR104_open_Failure_MTA_HAL) {
    
    EXPECT_CALL(*g_mtaHALMock, mta_hal_getTR104parameterNames(_, _))
        .WillOnce(Return(-1));

    int result = TR104_open();

    EXPECT_EQ(result, RBUS_ERROR_DESTINATION_RESPONSE_FAILURE);
}

TEST_F(CcspMtaTestFixture, TR104_open_Failure_RbusOpen) {
    char **paramList = nullptr;
    int paramCount = 0;

    EXPECT_CALL(*g_mtaHALMock, mta_hal_getTR104parameterNames(_, _))
        .WillOnce(DoAll(SetArgPointee<0>(paramList), SetArgPointee<1>(paramCount), Return(0)));

    EXPECT_CALL(*g_rbusMock, rbus_open(_, _))
        .WillOnce(Return(RBUS_ERROR_BUS_ERROR));

    int result = TR104_open();

    EXPECT_EQ(result, RBUS_ERROR_BUS_ERROR);
}

TEST_F(CcspMtaTestFixture, TR104_open_Failure_RbusRegDataElements) {
    char **paramList = new char*[2];
    paramList[0] = strdup("param1.{i}.");
    paramList[1] = strdup("param2");
    int paramCount = 2;

    EXPECT_CALL(*g_mtaHALMock, mta_hal_getTR104parameterNames(_, _))
        .WillOnce(DoAll(SetArgPointee<0>(paramList), SetArgPointee<1>(paramCount), Return(0)));

    EXPECT_CALL(*g_rbusMock, rbus_open(_, _))
        .WillOnce(Return(RBUS_ERROR_SUCCESS));

    EXPECT_CALL(*g_rbusMock, rbus_regDataElements(_, _, _))
        .WillOnce(Return(RBUS_ERROR_ELEMENT_NAME_DUPLICATE));

    EXPECT_CALL(*g_rbusMock, rbus_close(_))
        .Times(1);

    int result = TR104_open();


    EXPECT_EQ(result, RBUS_ERROR_ELEMENT_NAME_DUPLICATE);

    free(paramList[0]);
    free(paramList[1]);
    delete[] paramList;
}


TEST_F(CcspMtaTestFixture, TR104_open_Success) {
    char **paramList = new char*[2];
    paramList[0] = strdup("param1.{i}.");
    paramList[1] = strdup("param2");
    int paramCount = 2;

    // Simulate successful MTA HAL call
    EXPECT_CALL(*g_mtaHALMock, mta_hal_getTR104parameterNames(_, _))
        .WillOnce(DoAll(SetArgPointee<0>(paramList), SetArgPointee<1>(paramCount), Return(0)));

    // Simulate successful rbus_open
    EXPECT_CALL(*g_rbusMock, rbus_open(_, _))
        .WillOnce(Return(RBUS_ERROR_SUCCESS));

    // Simulate successful registration
    EXPECT_CALL(*g_rbusMock, rbus_regDataElements(_, _, _))
        .WillOnce(Return(RBUS_ERROR_SUCCESS));

     EXPECT_CALL(*g_webconfigFwMock, register_sub_docs(_, _, _, _));    

    int result = TR104_open();

    EXPECT_EQ(result, RBUS_ERROR_SUCCESS);

    // Cleanup allocated memory
    free(paramList[0]);
    free(paramList[1]);
    delete[] paramList;
}
