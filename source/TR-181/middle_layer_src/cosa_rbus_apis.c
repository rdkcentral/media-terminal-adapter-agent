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
#include "cosa_voice_apis.h"
#include "syscfg/syscfg.h"


#define  DHCPv4_VOICE_SUPPORT_PARAM "dmsb.voicesupport.Interface.IP.DHCPV4Interface"
#define  DHCP_MGR_DHCPv4_TABLE "Device.DHCPv4.Client"
#define  DHCP_MGR_DHCPv6_TABLE "Device.DHCPv6.Client"

rbusHandle_t voiceRbusHandle = NULL;
extern  ANSC_HANDLE  bus_handle;
const char cSubsystem[ ]= "eRT.";
static char cBaseParam[32] = {0};

/**
* @brief Convert parameter value type to rbus value type.
*
* @param[in] paramType  Parameter value type to be converted.
* @return Corresponding rbus value type.
*/

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

/**
 * @brief Set parameter in DHCP manager via RBUS.
 *
 * This function sets the specified parameter in the DHCP manager
 * using RBUS. It constructs an rbus value from the provided parameter
 * value and type, and performs the set operation. Errors during the
 * process are logged appropriately.
 *
 * @param[in] pParamName   Name of the parameter to be set.
 * @param[in] pParamValue  Value of the parameter to be set.
 * @param[in] paramType    Type of the parameter value.
*/
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

/**
 * @brief Retrieve interface index information for the MTA interface.
 *
 * This function retrieves Interface index information for the MTA interface
 * from the PSM (Persistent Storage Manager) and logs the retrieved value.
 * It reads the parameter "dmsb.voicesupport.Interface.IP.DHCPV4Interface"
 * from the PSM and stores it in a static variable for later use. If the
 * retrieval fails, it logs an error message.
 */

void getIfaceIndexInfo(void)
{
    int iRetPsmGet = CCSP_SUCCESS;
    char *pParamValue= NULL;

    iRetPsmGet = PSM_Get_Record_Value2(bus_handle, cSubsystem, DHCPv4_VOICE_SUPPORT_PARAM, NULL, &pParamValue);
    if (iRetPsmGet != CCSP_SUCCESS)
    {
        CcspTraceError(("%s: PSM_Get_Record_Value2 failed for param %s with error code %d\n", __FUNCTION__, DHCPv4_VOICE_SUPPORT_PARAM,
            iRetPsmGet));
        /* Set a safe default base parameter to avoid constructing invalid RBUS paths later. */
        snprintf(cBaseParam, sizeof(cBaseParam), "%s", DHCP_MGR_DHCPv4_TABLE);
        return;
    }
    else
    {
        CcspTraceInfo(("%s: PSM_Get_Record_Value2 successful for param %s with value %s\n", __FUNCTION__, DHCPv4_VOICE_SUPPORT_PARAM,
            pParamValue));
        if (strlen(pParamValue) == 0)
        {
            CcspTraceWarning(("%s: Retrieved empty value for param %s, using default base parameter\n", __FUNCTION__, DHCPv4_VOICE_SUPPORT_PARAM));
            snprintf(cBaseParam, sizeof(cBaseParam), "%s", DHCP_MGR_DHCPv4_TABLE);
        }
        else
        {
            snprintf(cBaseParam, sizeof(cBaseParam), "%s",pParamValue);
        }
        ((CCSP_MESSAGE_BUS_INFO *)bus_handle)->freefunc((char *)pParamValue);
    }
}

/**
 * @brief Initialize the RBUS handle used by this module.
 *
 * This function initializes and/or acquires the RBUS handle required
 * for subsequent RBUS operations performed by this component. It
 * should be called before any other APIs in this module that depend
 * on an active RBUS connection.
 */
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

/**
 * @brief Enable or configure IPv4 DHCP for the specified MTA interface.
 *
 * This function enables DHCPv4 for the MTA (e.g., telephony) interface
 * identified by the given interface name. The interface name must be a
 * valid, null-terminated string referring to an existing network
 * interface on the device.
 * Also it updates the interface name parameter in DHCP manager via RBUS.
 *
 * @param[in] pIfaceName  Name of the MTA network interface for which
 *                        DHCPv4 should be enabled.
 */
void enableDhcpv4ForMta(const char * pIfaceName)
{
    char cParamName[64] = {0};
    char cPartnerId[64] = { 0 };
    syscfg_get(NULL, "PartnerID", cPartnerId, sizeof(cPartnerId));
    if ('\0' != cPartnerId[0] && strcmp(cPartnerId, "comcast") == 0)
    {
        snprintf(cParamName, sizeof(cParamName), "%s.Interface", cBaseParam);
        setParamInDhcpMgr(cParamName, pIfaceName, STRING_PARAM);

        snprintf(cParamName, sizeof(cParamName), "%s.Enable", cBaseParam);
        setParamInDhcpMgr(cParamName, "true", BOOLEAN_PARAM);
    }
    else
    {
        CcspTraceInfo(("%s: Partner ID:%s is not Comcast, skipping DHCPv4 configuration\n", __FUNCTION__, cPartnerId));
    }
}
/**
 * @brief DHCP client events handler for MTA interface.
 *
 * This function handles DHCP client events received via RBUS for the
 * MTA interface. It processes events related to DHCPv4 and DHCPv6
 * clients, extracts relevant information, and logs the details.
 *
 * @param[in] voiceRbusHandle  RBUS handle used for event handling.
 * @param[in] pRbusEvent       Pointer to the received RBUS event.
 * @param[in] pRbusSubscription Pointer to the RBUS event subscription.
 */
static void dhcpClientEventsHandler(rbusHandle_t voiceRbusHandle, rbusEvent_t const* pRbusEvent, rbusEventSubscription_t* pRbusSubscription)
{
    (void)voiceRbusHandle;
    (void)pRbusSubscription;
    if (pRbusEvent == NULL)
    {
        CcspTraceError(("%s: Received NULL event\n", __FUNCTION__));
        return;
    }
    CcspTraceInfo(("%s: Received event %s\n", __FUNCTION__, pRbusEvent->name));

    pthread_t dhcpEventThreadId;

    if(strstr(pRbusEvent->name, DHCP_MGR_DHCPv4_TABLE) || strstr(pRbusEvent->name, DHCP_MGR_DHCPv6_TABLE))
    {
        if (0 == access("/tmp/dumpDHCPevent.txt", F_OK))
        {
            FILE *fp = fopen("/tmp/dhcp_event_dump.txt", "a");
            if(fp)
            {
                CcspTraceError(("Writing DHCP event data to /tmp/dhcp_event_dump.txt\n"));
                fprintf(fp, "Event Name: %s\n", pRbusEvent->name);
                fprintf(fp, "Event Data:\n");
                rbusObject_fwrite(pRbusEvent->data, 0, fp);
                fprintf(fp, "\n-----------------------\n");
                fclose(fp);
            }
        }
        // Unwrap the data - check if it's wrapped in "initialValue" (from publishOnSubscribe)
        rbusObject_t dataObj = pRbusEvent->data;
        rbusValue_t initialValue = rbusObject_GetValue(dataObj, "initialValue");
        if (NULL != initialValue)
        {
            // Check the type before unwrapping
            rbusValueType_t valueType = rbusValue_GetType(initialValue);

            if (valueType == RBUS_OBJECT)
            {
                // Valid lease data wrapped in object
                dataObj = rbusValue_GetObject(initialValue);
                CcspTraceInfo(("%s: Unwrapped initialValue object\n", __FUNCTION__));
            }
            else if (valueType == RBUS_STRING)
            {
                // Empty string (no lease available yet)
                const char *pStrVal = rbusValue_GetString(initialValue, NULL);
                if (NULL == pStrVal || strlen(pStrVal) == 0)
                {
                    CcspTraceInfo(("%s: Empty initialValue - no lease available yet\n", __FUNCTION__));
                    return;
                }
            }
            else
            {
                CcspTraceWarning(("%s: Unexpected initialValue type %d\n", __FUNCTION__, valueType));
                return;
            }
        }

        DhcpEventData_t *pDhcpEvtData = (DhcpEventData_t *)malloc(sizeof(DhcpEventData_t));
        if (pDhcpEvtData == NULL)
        {
            CcspTraceError(("%s: Memory allocation failed for DhcpEventData_t\n", __FUNCTION__));
            return;
        }
        memset(pDhcpEvtData, 0, sizeof(DhcpEventData_t));
        pDhcpEvtData->dhcpVersion = strstr(pRbusEvent->name, DHCP_MGR_DHCPv4_TABLE) ? DHCP_IPv4 : DHCP_IPv6;

        rbusValue_t rbusValue = rbusObject_GetValue(dataObj, "IfName");
        if (NULL == rbusValue)
        {
            CcspTraceInfo(("%s: No IfName in event (empty data)\n", __FUNCTION__));
            free(pDhcpEvtData);
            return;
        }
        const char *pIfaceName = rbusValue_GetString(rbusValue, NULL);
        if (NULL == pIfaceName || strlen(pIfaceName) == 0)
        {
            CcspTraceError(("%s: Invalid interface name in DHCP event\n", __FUNCTION__));
            free(pDhcpEvtData);
            return;
        }
        snprintf(pDhcpEvtData->cIfaceName, sizeof(pDhcpEvtData->cIfaceName), "%s", pIfaceName);
        CcspTraceInfo(("%s: DHCP %s event for interface %s\n", __FUNCTION__,
            (pDhcpEvtData->dhcpVersion == DHCP_IPv4) ? "IPv4" : "IPv6", pDhcpEvtData->cIfaceName));

        rbusValue = rbusObject_GetValue(dataObj, "MsgType");
        if (NULL == rbusValue)
        {
            CcspTraceInfo(("%s: No MsgType in event (empty data)\n", __FUNCTION__));
            free(pDhcpEvtData);
            return;
        }
        pDhcpEvtData->dhcpMsgType = (DHCP_MESSAGE_TYPE)rbusValue_GetUInt32(rbusValue);

        CcspTraceInfo(("%s: DHCP Message Type %d\n", __FUNCTION__, pDhcpEvtData->dhcpMsgType));

        if (DHCP_LEASE_UPDATE == pDhcpEvtData->dhcpMsgType || DHCP_LEASE_RENEW == pDhcpEvtData->dhcpMsgType || DHCP_LEASE_DEL == pDhcpEvtData->dhcpMsgType || \
            DHCP_CLIENT_STARTED == pDhcpEvtData->dhcpMsgType || DHCP_CLIENT_STOPPED == pDhcpEvtData->dhcpMsgType || DHCP_CLIENT_FAILED == pDhcpEvtData->dhcpMsgType)
        {
            int iByteLen = 0;
            rbusValue = rbusObject_GetValue(dataObj, "LeaseInfo");
            if (NULL == rbusValue)
            {
                CcspTraceInfo(("%s: No LeaseInfo in event for DHCP Message Type %d\n", __FUNCTION__, pDhcpEvtData->dhcpMsgType));
                free(pDhcpEvtData);
                return;
            }
            const uint8_t* pLeaseInfo = rbusValue_GetBytes(rbusValue, &iByteLen);
            if (pLeaseInfo != NULL && iByteLen > 0)
            {
                if (DHCP_IPv4 == pDhcpEvtData->dhcpVersion)
                {
                    CcspTraceInfo(("%s: Processing DHCPv4 LeaseInfo of size %d\n", __FUNCTION__, iByteLen));
                    CcspTraceInfo(("%s:%d, DHCP_MGR_IPV4_MSG size: %ld\n", __FUNCTION__, __LINE__, sizeof(DHCP_MGR_IPV4_MSG)));
                    if (sizeof(DHCP_MGR_IPV4_MSG) == iByteLen)
                    {
                        memcpy(&pDhcpEvtData->leaseInfo.dhcpV4Msg, pLeaseInfo, sizeof(DHCP_MGR_IPV4_MSG));
                        CcspTraceInfo(("%s:%d, DHCPv4 Lease Info - Ifname: %s, Address: %s, Netmask: %s, Gateway: %s\n",
                            __FUNCTION__, __LINE__,
                            pDhcpEvtData->leaseInfo.dhcpV4Msg.ifname,
                            pDhcpEvtData->leaseInfo.dhcpV4Msg.address,
                            pDhcpEvtData->leaseInfo.dhcpV4Msg.netmask,
                            pDhcpEvtData->leaseInfo.dhcpV4Msg.gateway));
                        CcspTraceInfo(("%s:%d, DHCPv4 Lease Info - DNS Server: %s, DNS Server1: %s, TimeZone: %s\n",
                            __FUNCTION__, __LINE__,
                            pDhcpEvtData->leaseInfo.dhcpV4Msg.dnsServer,
                            pDhcpEvtData->leaseInfo.dhcpV4Msg.dnsServer1,
                            pDhcpEvtData->leaseInfo.dhcpV4Msg.timeZone));
                        CcspTraceInfo(("%s:%d, DHCPv4 Lease Info - MTU Size: %u, Time Offset: %d, IsTimeOffsetAssigned: %d\n",
                            __FUNCTION__, __LINE__,
                            pDhcpEvtData->leaseInfo.dhcpV4Msg.mtuSize,
                            pDhcpEvtData->leaseInfo.dhcpV4Msg.timeOffset,
                            pDhcpEvtData->leaseInfo.dhcpV4Msg.isTimeOffsetAssigned));
                        CcspTraceInfo(("%s:%d, DHCPv4 Lease Info - Upstream Rate: %u, Downstream Rate: %u\n",
                            __FUNCTION__, __LINE__,
                            pDhcpEvtData->leaseInfo.dhcpV4Msg.upstreamCurrRate,
                            pDhcpEvtData->leaseInfo.dhcpV4Msg.downstreamCurrRate));
                        CcspTraceInfo(("%s:%d, DHCPv4 Lease Info - MTA Option 122: %s, MTA Option 67: %s, TFTP Server: %s\n",
                            __FUNCTION__, __LINE__,
                            pDhcpEvtData->leaseInfo.dhcpV4Msg.cOption122,
                            pDhcpEvtData->leaseInfo.dhcpV4Msg.cOption67,
                            pDhcpEvtData->leaseInfo.dhcpV4Msg.cTftpServer));
                        CcspTraceInfo(("%s:%d, DHCPv4 Lease Info - HostName: %s, DomainName: %s\n",
                            __FUNCTION__, __LINE__,
                            pDhcpEvtData->leaseInfo.dhcpV4Msg.cHostName,
                            pDhcpEvtData->leaseInfo.dhcpV4Msg.cDomainName));
                    }
                    else
                    {
                        CcspTraceError(("%s: Invalid DHCPv4 LeaseInfo size %d\n", __FUNCTION__, iByteLen));
                    }
                }
            }
            else
            {
                CcspTraceError(("%s: NULL or empty LeaseInfo in DHCP event\n", __FUNCTION__));
                free(pDhcpEvtData);
                return;
            }
        }
        else
        {
            CcspTraceInfo(("%s: No LeaseInfo for DHCP Message Type %d\n", __FUNCTION__, pDhcpEvtData->dhcpMsgType));
            free(pDhcpEvtData);
            return;
        }
        if (pthread_create(&dhcpEventThreadId, NULL, dhcpClientEventsHandlerThread, (void *)pDhcpEvtData) != 0)
        {
            CcspTraceError(("%s: Failed to create DHCP event processing thread\n", __FUNCTION__));
            free(pDhcpEvtData);
            return;
        }
        pthread_detach(dhcpEventThreadId);
        usleep(10000); // Sleep for 10ms to allow thread to start
    }
    else
    {
        CcspTraceWarning(("%s: Unrecognized event %s\n", __FUNCTION__, pRbusEvent->name));
    }
}

void * eventSubscriptionThread(void * pArg)
{
    (void)pArg; // Unused parameter
    char cEventName[64] = {0};

    snprintf(cEventName, sizeof(cEventName), "%s.Events", cBaseParam);
    CcspTraceInfo(("%s:%d, Subscribing to DHCP client events for MTA interface with event name %s\n", __FUNCTION__, __LINE__, cEventName));

    rbusEventSubscription_t rbusEventSubscription = {
        .eventName = cEventName,
        .filter = NULL,
        .interval = 0,
        .duration = 0,
        .handler = dhcpClientEventsHandler,
        .userData = NULL,
        .handle = NULL,
        .asyncHandler = NULL,
        .publishOnSubscribe = true
    };

    if ('\0' == cEventName[0])
    {
        CcspTraceError(("%s: Event name is empty, cannot subscribe\n", __FUNCTION__));
        return NULL;
    }
    CcspTraceInfo(("%s:%d, Subscribing to event %s in event subscription thread\n", __FUNCTION__, __LINE__, rbusEventSubscription.eventName));
    rbusError_t rbusRet = rbusEvent_SubscribeEx (voiceRbusHandle, &rbusEventSubscription, 1/* Number of subscriptions */, 5 /* retry interval in seconds */);
    if (rbusRet != RBUS_ERROR_SUCCESS)
    {
        CcspTraceError(("%s: rbus_event_subscribe failed for event %s with error code %d\n", __FUNCTION__, rbusEventSubscription.eventName, rbusRet));
    }
    else
    {
        CcspTraceInfo(("%s: rbus_event_subscribe successful for event %s\n", __FUNCTION__, rbusEventSubscription.eventName));
    }
    pthread_exit(NULL);
}
/**
 * @brief Subscribe to DHCP client events for MTA interface.
 * This function sets up subscriptions to listen for DHCP client
 * events related to the MTA interface.
 */

void subscribeDhcpClientEvents(void)
{
    if (NULL == voiceRbusHandle)
    {
        CcspTraceError(("%s: rbus handle is NULL, cannot subscribe to events\n", __FUNCTION__));
        return;
    }

    pthread_t eventSubscriptionThreadId = -1;
    if (pthread_create(&eventSubscriptionThreadId, NULL, (void *)eventSubscriptionThread, NULL) != 0)
    {
        CcspTraceError(("%s: Failed to create event subscription thread\n", __FUNCTION__));
        return;
    }
    pthread_detach(eventSubscriptionThreadId);
}
