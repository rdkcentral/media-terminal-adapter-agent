/*
 * If not stated otherwise in this file or this component's LICENSE file the
 * following copyright and licenses apply:
 *
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

#include "ccsp_trace.h"
#include "ccsp_psm_helper.h"
#include "cosa_rbus_apis.h"

#define  DHCPv4_VOICE_SUPPORT_PARAM "dmsb.voicesupport.Interface.IP.DHCPV4Interface"

rbusHandle_t voiceRbusHandle = NULL;
extern  ANSC_HANDLE  bus_handle;
const char cSubsystem[ ]= "eRT.";

static rbusValueType_t convertRbusDataType(paramValueType_t paramType)
{
    rbusValueType_t rbusType = RBUS_NONE;
    switch (paramType)
    {
        case BOOLEAN_PARAM:
            rbusType = RBUS_BOOLEAN;
            break;
        case STRING_PARAM:
            rbusType = RBUS_STRING;
            break;
        default:
            CcspTraceWarning(("%s: Unsupported param type %d\n", __FUNCTION__, paramType));
            break;
    }
    return rbusType;
}

static void setParamInDhcpMgr(const char * pParamName, const char * pParamValue, paramValueType_t paramType)
{
    if (voiceRbusHandle == NULL || pParamName == NULL || pParamValue == NULL)   
    {
        CcspTraceError(("%s: Invalid rbus handle or NULL parameter\n", __FUNCTION__));
        return;
    }
    int iRet = -1;
    rbusValue_t rbusValue;
    rbusValueType_t rbusType = convertRbusDataType(paramType);
    if (rbusType == RBUS_NONE)
    {
        CcspTraceError(("%s: Unsupported param type %d for param %s\n", __FUNCTION__, paramType, pParamName));
        return;
    }

    rbusValue_Init(&rbusValue);
    if (false == rbusValue_SetFromString(rbusValue, rbusType, pParamValue))
    {
        CcspTraceError(("%s: rbusValue_SetFromString failed for param %s with value %s\n", __FUNCTION__, pParamName, pParamValue));
        rbusValue_Release(rbusValue);
        return;
    }

    iRet = rbus_set(voiceRbusHandle, pParamName, rbusValue, NULL);
    if (iRet != RBUS_ERROR_SUCCESS)
    {
        CcspTraceError(("%s: rbus_set failed for param %s with error code %d\n", __FUNCTION__, pParamName, iRet));
    }
    else
    {
        CcspTraceInfo(("%s: rbus_set successful for param %s with value %s\n", __FUNCTION__, pParamName, pParamValue));
    }
    rbusValue_Release(rbusValue);
}

void initRbusHandle(void)
{
    rbusError_t rbusReturn = RBUS_ERROR_SUCCESS;
    if (voiceRbusHandle != NULL)
    {
        CcspTraceInfo(("%s: rbus handle already initialized\n", __FUNCTION__));
        return;
    }
    rbusReturn = rbus_open(&voiceRbusHandle, "VoiceSupportMtaInterface");
    if (rbusReturn != RBUS_ERROR_SUCCESS)
    {
        CcspTraceError(("%s: rbus_open failed with error code %d\n", __FUNCTION__, rbusReturn));
        return;
    }
    CcspTraceInfo(("%s: rbus_open successful\n", __FUNCTION__));
}

void enableDhcpv4ForMta(const char * pIfaceName)
{
    int iRetPsmGet = CCSP_SUCCESS;
    char cBaseParam[32] = {0};
    char cParamName[64] = {0};
    char *pParamValue= NULL;

    iRetPsmGet = PSM_Get_Record_Value2(bus_handle, cSubsystem, DHCPv4_VOICE_SUPPORT_PARAM, NULL, &pParamValue);
    if (iRetPsmGet != CCSP_SUCCESS)
    {
        CcspTraceError(("%s: PSM_Get_Record_Value2 failed for param %s with error code %d\n", __FUNCTION__, DHCPv4_VOICE_SUPPORT_PARAM,
            iRetPsmGet));
        return;
    }
    else
    {
        CcspTraceInfo(("%s: PSM_Get_Record_Value2 successful for param %s with value %s\n", __FUNCTION__, DHCPv4_VOICE_SUPPORT_PARAM,
            pParamValue));
        snprintf(cBaseParam, sizeof(cBaseParam), "%s",pParamValue);
        ((CCSP_MESSAGE_BUS_INFO *)bus_handle)->freefunc((char *)pParamValue);
    }

    snprintf(cParamName, sizeof(cParamName), "%s.Enable", cBaseParam);
    setParamInDhcpMgr(cParamName, "true", BOOLEAN_PARAM);

    snprintf(cParamName, sizeof(cParamName), "%s.Interface", cBaseParam);
    setParamInDhcpMgr(cParamName, pIfaceName, STRING_PARAM);
}

//void subscribeDhcpClientEvents()
