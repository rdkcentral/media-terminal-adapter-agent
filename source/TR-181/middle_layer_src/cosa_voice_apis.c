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
#include "cosa_x_cisco_com_mta_internal.h"
#include "cosa_rbus_apis.h"
#include "voice_dhcp_hal.h"
#include "syscfg/syscfg.h"
#include "telemetry_busmessage_sender.h"

#define MTA_ROUTE_TABLE_NAME "mtaVoice"
#define RT_TABLES_FILE       "/etc/iproute2/rt_tables"

pthread_mutex_t voiceDataProcessingMutex = PTHREAD_MUTEX_INITIALIZER;

/*
 * @brief Convert IP mode string to enum value.
*/
static IP_MODE convertIpMode(const char *pString)
{
    IP_MODE eIpMode = -1;//unknown

    if (NULL == pString)
    {
        return eIpMode;
    }
    if (0 == strcmp(pString, "IPv4_Only"))
    {
        eIpMode = IPV4_ONLY;
    }
    else if (0 == strcmp(pString, "Dual_Stack"))
    {
        eIpMode = DUAL_MODE;
    }
    else if (0 == strcmp(pString, "IPv6_Only"))
    {
        eIpMode = IPV6_ONLY;
    }
    return eIpMode;
}
/*
 * @brief Start the voice support feature by subscribing to DHCP client events.
 *
 * This function checks the syscfg setting "VoiceSupport_Enabled" to determine
 * if voice support is enabled. If it is set to "true", the function subscribes
 * to DHCP client events to support voice-related networking behavior. If voice
 * support is disabled or not configured, the function returns without making
 * any changes.
*/
void startVoiceFeature(void)
{
    char cVoiceSupportEnabled[8] = {0};
    char cVoiceSupportMode[32] = {0};
    IP_MODE eIpModeFromHal = -1; //Unknown

    syscfg_get(NULL, "VoiceSupport_Enabled", cVoiceSupportEnabled, sizeof(cVoiceSupportEnabled));

    if (cVoiceSupportEnabled[0] == '\0' || strcmp(cVoiceSupportEnabled, "true") != 0)
    {
        CcspTraceError(("%s:%d, VoiceSupport_Enabled is false or not set, skipping voice feature initialization\n", __FUNCTION__, __LINE__));
        return;
    }
    syscfg_get(NULL, "VoiceSupport_Mode",cVoiceSupportMode, sizeof(cVoiceSupportMode));
    if (0 == strlen(cVoiceSupportMode) || (strcmp(cVoiceSupportMode, "IPv4_Only") != 0 && strcmp(cVoiceSupportMode, "Dual_Stack") != 0))
    {
        CcspTraceError(("%s:%d, VoiceSupport_Mode is not set to a valid value, skipping voice feature initialization\n", __FUNCTION__, __LINE__));
        return;
    }

    if ( 1 != voice_hal_get_ip_mode (&eIpModeFromHal))
    {
        CcspTraceError(("%s:%d, Failed to get IP mode from voice HAL\n", __FUNCTION__, __LINE__));
        /* Proceeding with syscfg value since we can still set it in voice HAL */
    }

    IP_MODE eIpModeFromSyscfg = convertIpMode(cVoiceSupportMode);
    if (eIpModeFromHal != eIpModeFromSyscfg)
    {
        CcspTraceInfo(("%s:%d, IP mode from voice HAL (%d) is different from syscfg value (%d), updating voice HAL with syscfg value\n", __FUNCTION__, __LINE__, eIpModeFromHal, eIpModeFromSyscfg));
        if (1 != voice_hal_set_ip_mode (eIpModeFromSyscfg))
        {
            CcspTraceError(("%s:%d, Failed to set IP mode in voice HAL\n", __FUNCTION__, __LINE__));
        }
    }
    subscribeDhcpClientEvents();
}

/*
 *@brief Add IP route details for the MTA interface based on the DHCP event data.
 * This function processes the DHCP event data to extract relevant network information such as the assigned IP address, gateway, and TFTP server.
 * It then configures the necessary IP routes for the MTA interface.
*/
static void addIpRouteDetails(DhcpEventData_t *pDhcpEvtData)
{
    if (NULL == pDhcpEvtData)
    {
        CcspTraceError(("%s: NULL DHCP event data provided\n", __FUNCTION__));
        return;
    }

    CcspTraceInfo(("%s:%d, Adding IP route details for interface %s\n", __FUNCTION__, __LINE__, pDhcpEvtData->cIfaceName));
    CcspTraceInfo(("%s:%d, Gateway address:%s\n",__FUNCTION__,__LINE__,pDhcpEvtData->leaseInfo.dhcpV4Msg.gateway));
    CcspTraceInfo(("%s:%d, TFTP server address:%s\n",__FUNCTION__,__LINE__,pDhcpEvtData->leaseInfo.dhcpV4Msg.cTftpServer));
    CcspTraceInfo(("%s:%d, MTA IP address:%s\n",__FUNCTION__,__LINE__,pDhcpEvtData->leaseInfo.dhcpV4Msg.address));

    char cCmd[256] = {0};
    struct in_addr ipAddr = {0}, netMask = {0}, networkAddr = {0};
    int iNetRet = 0;

    iNetRet = inet_pton(AF_INET, pDhcpEvtData->leaseInfo.dhcpV4Msg.address, &ipAddr);
    if (iNetRet != 1)
    {
        CcspTraceError(("%s:%d, inet_pton failed for IP address %s with error code %d\n", __FUNCTION__, __LINE__, pDhcpEvtData->leaseInfo.dhcpV4Msg.address, iNetRet));
        return;
    }
    iNetRet = inet_pton(AF_INET, pDhcpEvtData->leaseInfo.dhcpV4Msg.netmask, &netMask);
    if (iNetRet != 1)
    {
        CcspTraceError(("%s:%d, inet_pton failed for netmask %s with error code %d\n", __FUNCTION__, __LINE__, pDhcpEvtData->leaseInfo.dhcpV4Msg.netmask, iNetRet));
        return;
    }
    networkAddr.s_addr = ipAddr.s_addr & netMask.s_addr;

    char cNetworkAddr[32] = {0};
    char cNetworkAddrWithCidr[64] = {0};
    inet_ntop(AF_INET, &networkAddr, cNetworkAddr, sizeof(cNetworkAddr));
    CcspTraceInfo(("%s:%d, Network Address:%s\n",__FUNCTION__,__LINE__,cNetworkAddr));
    snprintf(cNetworkAddrWithCidr, sizeof(cNetworkAddrWithCidr), "%s/%d", cNetworkAddr, __builtin_popcount(ntohl(netMask.s_addr)));
    CcspTraceInfo(("%s:%d, Network Address in CIDR notation:%s\n",__FUNCTION__,__LINE__,cNetworkAddrWithCidr));

    const char *pAddr    = pDhcpEvtData->leaseInfo.dhcpV4Msg.address;
    const char *pIface   = pDhcpEvtData->cIfaceName;
    const char *pGateway = pDhcpEvtData->leaseInfo.dhcpV4Msg.gateway;
    const char *pTftpSrv = pDhcpEvtData->leaseInfo.dhcpV4Msg.cTftpServer;

    // Verify named table "mtaVoice" is registered (pre-populated at build time)
    if (access(RT_TABLES_FILE, R_OK) != 0)
    {
        CcspTraceError(("%s:%d, %s not accessible: %s\n",
                        __FUNCTION__, __LINE__, RT_TABLES_FILE, strerror(errno)));
        return;
    }

    // --- Clean up old state ---

    // Flush all rules referencing table mtaVoice (handles old IP changes)
    snprintf(cCmd, sizeof(cCmd), "ip rule flush table %s 2>/dev/null", MTA_ROUTE_TABLE_NAME);
    CcspTraceInfo(("%s:%d, Executing: %s\n", __FUNCTION__, __LINE__, cCmd));
    system(cCmd);

    // Flush all routes in table mtaVoice (we own the entire table)
    snprintf(cCmd, sizeof(cCmd), "ip route flush table %s 2>/dev/null", MTA_ROUTE_TABLE_NAME);
    CcspTraceInfo(("%s:%d, Executing: %s\n", __FUNCTION__, __LINE__, cCmd));
    system(cCmd);

    // For main table: flush only proto static routes on mta interface
    // (kernel-added routes like the connected subnet are proto kernel, not affected)
    snprintf(cCmd, sizeof(cCmd), "ip route flush dev %s proto static 2>/dev/null", pIface);
    CcspTraceInfo(("%s:%d, Executing: %s\n", __FUNCTION__, __LINE__, cCmd));
    system(cCmd);

    // --- Add new state ---
    // 1. Gateway host route (main table) — ensures gateway is explicitly reachable
    snprintf(cCmd, sizeof(cCmd), "ip route add %s dev %s proto static",
             pGateway, pIface);
    CcspTraceInfo(("%s:%d, Executing: %s\n", __FUNCTION__, __LINE__, cCmd));
    system(cCmd);

    // 2. TFTP server route (main table) — gateway is now reachable via steps 1+2
    snprintf(cCmd, sizeof(cCmd), "ip route add %s via %s dev %s proto static",
             pTftpSrv, pGateway, pIface);
    CcspTraceInfo(("%s:%d, Executing: %s\n", __FUNCTION__, __LINE__, cCmd));
    system(cCmd);

    // 3. IP rule for source-based routing
    snprintf(cCmd, sizeof(cCmd), "ip rule add from %s table %s",
             pAddr, MTA_ROUTE_TABLE_NAME);
    CcspTraceInfo(("%s:%d, Executing: %s\n", __FUNCTION__, __LINE__, cCmd));
    system(cCmd);

    // 4. Network route in table mtaVoice
    snprintf(cCmd, sizeof(cCmd), "ip route add %s dev %s src %s table %s",
             cNetworkAddrWithCidr, pIface, pAddr, MTA_ROUTE_TABLE_NAME);
    CcspTraceInfo(("%s:%d, Executing: %s\n", __FUNCTION__, __LINE__, cCmd));
    system(cCmd);

    // 5. Default route in table mtaVoice
    snprintf(cCmd, sizeof(cCmd), "ip route add default via %s dev %s table %s",
             pGateway, pIface, MTA_ROUTE_TABLE_NAME);
    CcspTraceInfo(("%s:%d, Executing: %s\n", __FUNCTION__, __LINE__, cCmd));
    system(cCmd);
}

/*
 *@brief Convert a hexadecimal string to a byte array.
 * This helper function takes a hexadecimal string representation and converts it
 * into its corresponding byte array.
 * @param[in] pHexString
 *      Pointer to a null-terminated string containing the hexadecimal representation.
 * @param[out] pByteArray
 *      Pointer to a byte array that will receive the converted bytes.
 * @param[in] iByteArrayLen
 *      Length of the byte array to be filled.
*/
void hexStringToByteArray(const char* pHexString, uint8_t* pByteArray, int iByteArrayLen)
{
    if (NULL == pHexString || NULL == pByteArray || iByteArrayLen <= 0)
    {
        CcspTraceError(("%s: NULL parameters are passed \n", __FUNCTION__));
        return;
    }

    int iHexStringLen = strlen(pHexString);
    for (int iIndex = 0; iIndex < iByteArrayLen && (iIndex * 2 + 1) < iHexStringLen; iIndex++)
    {
        sscanf(&pHexString[iIndex * 2], "%2hhx", &pByteArray[iIndex]);
    }
}
/*
 * @brief Extract a specific sub-option from DHCP Option 122 data.
 *
 * This helper function parses the provided DHCP Option 122 data to find
 * and extract the value of a specified sub-option.
 *
 * @param[in] pOption122Data
 *      Pointer to the raw DHCP Option 122 data buffer.
 * @param[in] iOption122Len
 *      Length of the DHCP Option 122 data buffer.
 * @param[in] ui8WantedSubOption
 *      The sub-option number to extract.
 * @param[out] pOutputVal
 *      Pointer to a variable that will receive a pointer to the extracted
 *      sub-option value within the original buffer.
 * @param[out] pOutputValLen
 *      Pointer to a variable that will receive the length of the extracted
 *      sub-option value.
 *
 * @return
 *      Returns 0 on success, -1 if the sub-option is not found or on error.
*/
static int getOption122_SubOptions(uint8_t *pOption122Data, uint16_t iOption122Len, uint8_t ui8WantedSubOption, uint8_t **pOutputVal, uint8_t *pOutputValLen)
{
    if (NULL == pOption122Data || iOption122Len == 0 || NULL == pOutputVal || NULL == pOutputValLen)
    {
        CcspTraceError(("%s: NULL parameters are passed \n", __FUNCTION__));
        return -1;
    }

    uint16_t ui16Position = 0;
    while(ui16Position + 2 <= iOption122Len)
    {
        uint8_t ui8SubOption = pOption122Data[ui16Position];
        uint8_t ui8SubOptionLen = pOption122Data[ui16Position + 1];

        CcspTraceInfo(("%s:%d, SubOption:%u, SubOption Length:%u\n", __FUNCTION__, __LINE__, ui8SubOption, ui8SubOptionLen));
        CcspTraceInfo(("%s:%d, ui16Position:%d\n",__FUNCTION__, __LINE__, ui16Position));

        if (ui16Position + 2 + ui8SubOptionLen > iOption122Len)
        {
            CcspTraceError(("%s: Malformed Option 122 data\n", __FUNCTION__));
            return -1;
        }

        if (ui8SubOption == ui8WantedSubOption)
        {
            *pOutputVal = &pOption122Data[ui16Position + 2];
            *pOutputValLen = ui8SubOptionLen;
            return 0;
        }

        ui16Position += 2 + ui8SubOptionLen;
    }
    return -1; // Sub-option not found
}

/*
 *@brief This function initializes the voice support related parameters based on DHCP event data
 *@param pDhcpEvtData - Pointer to the DHCP event data structure
*/
static void initializeVoiceSupport(DhcpEventData_t *pDhcpEvtData)
{
    if (NULL == pDhcpEvtData)
    {
        CcspTraceError(("%s: NULL DHCP event data provided\n", __FUNCTION__));
        return;
    }
    VoiceInterfaceInfoType sVoiceInterfaceInfoType = {0};
    snprintf(sVoiceInterfaceInfoType.intfName, sizeof(sVoiceInterfaceInfoType.intfName), "%s", pDhcpEvtData->cIfaceName);
    sVoiceInterfaceInfoType.isPhyUp = 1;
    sVoiceInterfaceInfoType.isIpv4Up = 1;
    strncpy(sVoiceInterfaceInfoType.ipv4Addr, pDhcpEvtData->leaseInfo.dhcpV4Msg.address, sizeof(sVoiceInterfaceInfoType.ipv4Addr)-1);
    strncpy(sVoiceInterfaceInfoType.v4NextServerIp, pDhcpEvtData->leaseInfo.dhcpV4Msg.cTftpServer, sizeof(sVoiceInterfaceInfoType.v4NextServerIp)-1);
    strncpy(sVoiceInterfaceInfoType.v4BootFileName, pDhcpEvtData->leaseInfo.dhcpV4Msg.cOption67, sizeof(sVoiceInterfaceInfoType.v4BootFileName)-1);

    if ('\0' != sVoiceInterfaceInfoType.ipv4Addr[0])
    {
        CcspTraceInfo(("%s:%d, IPv4 address:%s assigned for %s interface\n",__FUNCTION__,__LINE__,sVoiceInterfaceInfoType.ipv4Addr, sVoiceInterfaceInfoType.intfName));
        t2_event_d("IPv4 address assigned for MTA interface", 1);
    }
    char cTmpDnsServers[VOICE_IPV4_ADDR_LEN*4] = {0};
    int iBufSize = sizeof(cTmpDnsServers);
    int iLen = 0;
    strncpy(cTmpDnsServers, pDhcpEvtData->leaseInfo.dhcpV4Msg.dnsServer, iBufSize-1);
    cTmpDnsServers[iBufSize -1] = '\0';
    iLen = strlen(cTmpDnsServers);
    if (iLen < (iBufSize -1) && strlen(pDhcpEvtData->leaseInfo.dhcpV4Msg.dnsServer1) > 0)
    {
        if (iLen > 0)
        {
            strncat(cTmpDnsServers, ",", iBufSize - strlen(cTmpDnsServers) -1);
        }
        strncat(cTmpDnsServers, pDhcpEvtData->leaseInfo.dhcpV4Msg.dnsServer1, iBufSize - strlen(cTmpDnsServers) -1);
    }

    strncpy(sVoiceInterfaceInfoType.v4DnsServers, cTmpDnsServers, sizeof(sVoiceInterfaceInfoType.v4DnsServers)-1);
    strncpy(sVoiceInterfaceInfoType.v4HostName, pDhcpEvtData->leaseInfo.dhcpV4Msg.cHostName, sizeof(sVoiceInterfaceInfoType.v4HostName)-1);
    strncpy(sVoiceInterfaceInfoType.v4DomainName, pDhcpEvtData->leaseInfo.dhcpV4Msg.cDomainName, sizeof(sVoiceInterfaceInfoType.v4DomainName)-1);
    strncpy(sVoiceInterfaceInfoType.v4LogServerIp, "(null)", sizeof(sVoiceInterfaceInfoType.v4LogServerIp)-1);
    strncpy(sVoiceInterfaceInfoType.v4ServerHostName, "(null)", sizeof(sVoiceInterfaceInfoType.v4ServerHostName)-1);

    //Retrieve Option122 SubOption 3 for Provisioning Server
    uint8_t *pSubOptData = NULL;
    uint8_t ui8SubOptionLen = 0;
    const char *pHexOption122Data = pDhcpEvtData->leaseInfo.dhcpV4Msg.cOption122;
    uint16_t iOption122Len = strlen(pHexOption122Data) / 2;
    uint8_t ui8Option122Data[iOption122Len];
    memset(ui8Option122Data, 0, sizeof(ui8Option122Data));
    hexStringToByteArray(pHexOption122Data, ui8Option122Data, iOption122Len);

    CcspTraceInfo(("%s:%d, Option122 Data: %s\n", __FUNCTION__, __LINE__, pDhcpEvtData->leaseInfo.dhcpV4Msg.cOption122));
    if (0 == getOption122_SubOptions(ui8Option122Data,
                                    iOption122Len,
                                    3,
                                    &pSubOptData,
                                    &ui8SubOptionLen))
    {
        int iCopyLen = (ui8SubOptionLen < sizeof(sVoiceInterfaceInfoType.v4ProvServer)-1) ? ui8SubOptionLen : (sizeof(sVoiceInterfaceInfoType.v4ProvServer)-1);
        memcpy(sVoiceInterfaceInfoType.v4ProvServer, pSubOptData, iCopyLen);
        sVoiceInterfaceInfoType.v4ProvServer[iCopyLen] = '\0';
        CcspTraceInfo(("%s:%d, Retrieved Option122 SubOption 3 for Provisioning Server: %s and len:%u\n", __FUNCTION__, __LINE__, sVoiceInterfaceInfoType.v4ProvServer, ui8SubOptionLen));
        char cHexBuf[256] = {0};
        long unsigned int uiPos = 0;
        for (int i = 0; i < ui8SubOptionLen && uiPos < sizeof(cHexBuf) - 3; i++) {
            uiPos += snprintf(cHexBuf + uiPos, sizeof(cHexBuf) - uiPos, "%02x ", pSubOptData[i]);
        }
        CcspTraceInfo(("%s:%d, Option122 SubOption 3 hex: %s\n", __FUNCTION__, __LINE__, cHexBuf));
    }
    else {
        CcspTraceError(("%s: Failed to get Option122 SubOption 3 for Provisioning Server\n", __FUNCTION__));
    }

    CcspTraceInfo(("%s:%d, Initializing Voice Support with following details:\n", __FUNCTION__, __LINE__));
    CcspTraceInfo(("%s:%d, Interface Name: %s\n", __FUNCTION__, __LINE__, sVoiceInterfaceInfoType.intfName));
    CcspTraceInfo(("%s:%d, IPv4 Address: %s\n", __FUNCTION__, __LINE__, sVoiceInterfaceInfoType.ipv4Addr));
    CcspTraceInfo(("%s:%d, Next Server IP: %s\n", __FUNCTION__, __LINE__, sVoiceInterfaceInfoType.v4NextServerIp));
    CcspTraceInfo(("%s:%d, Boot File Name: %s\n", __FUNCTION__, __LINE__, sVoiceInterfaceInfoType.v4BootFileName));
    CcspTraceInfo(("%s:%d, DNS Servers: %s\n", __FUNCTION__, __LINE__, sVoiceInterfaceInfoType.v4DnsServers));
    CcspTraceInfo(("%s:%d, Host Name: %s\n", __FUNCTION__, __LINE__, sVoiceInterfaceInfoType.v4HostName));
    CcspTraceInfo(("%s:%d, Domain Name: %s\n", __FUNCTION__, __LINE__, sVoiceInterfaceInfoType.v4DomainName));
    CcspTraceInfo(("%s:%d, Provisioning Server: %s\n", __FUNCTION__, __LINE__, sVoiceInterfaceInfoType.v4ProvServer));
    CcspTraceInfo(("%s:%d, Log Server IP: %s\n", __FUNCTION__, __LINE__, sVoiceInterfaceInfoType.v4LogServerIp));
    CcspTraceInfo(("%s:%d, Server Host Name: %s\n", __FUNCTION__, __LINE__, sVoiceInterfaceInfoType.v4ServerHostName));



    char cPartnerId[64] = { 0 };
    syscfg_get(NULL, "PartnerID", cPartnerId, sizeof(cPartnerId));
    if ('\0' != cPartnerId[0] && strcmp(cPartnerId, "comcast") == 0)
    {
        /*TODO: NEED TO REMOVE THE SLEEP
         * As of now broadcom snmpd process is not starting during bootup
         * Added a sleep of 10 seconds as a workaround
         * Once we get the broadcom patch, we need to remove the sleep of 10 seconds */
        if (0 == access("/nvram/add_sleep_bf_voice_hal_init", F_OK))
        {
            CcspTraceInfo(("%s:%d,Adding 10 seconds sleep before initializing voice support\n", __FUNCTION__, __LINE__));
            sleep(10);
        }
        uint8_t ui8Ret = voice_hal_interface_info_notify(&sVoiceInterfaceInfoType);
        CcspTraceInfo(("%s:%d, voice_hal_interface_info_notify returned %u\n", __FUNCTION__, __LINE__, ui8Ret));
        if (1 != ui8Ret)
        {
            CcspTraceError(("%s:%d, voice_hal_interface_info_notify failed to update voice interface info\n", __FUNCTION__, __LINE__));
            t2_event_d("Failed to initialize the voice support", 1);
            return;
        }
    }
    else
    {
        CcspTraceInfo(("%s:%d, PartnerID is not Comcast, skipping voice_hal_interface_info_notify call\n", __FUNCTION__, __LINE__));
    }
}

static void processVoiceDhcpEvent(DhcpEventData_t *pDhcpEvtData)
{
    if (NULL == pDhcpEvtData)
    {
        CcspTraceError(("%s: NULL DHCP event data provided\n", __FUNCTION__));
        return;
    }
    addIpRouteDetails(pDhcpEvtData);
    initializeVoiceSupport(pDhcpEvtData);
    
}
/*
 * @brief Thread function to handle DHCP client events for voice support.
 * This function processes DHCP events in a separate thread to avoid blocking
 * the main event handling loop. It takes a pointer to DhcpEventData_t as an
 * argument, which contains the DHCP event details.
 * @param[in] pArg
 *      Pointer to a DhcpEventData_t structure containing the DHCP event data.
 * @return
 *      Returns NULL upon completion.
*/
void * dhcpClientEventsHandlerThread(void * pArg)
{
    if (NULL == pArg)
    {
        CcspTraceError(("%s: NULL argument provided\n", __FUNCTION__));
        return NULL;
    }
    
    CcspTraceInfo(("%s:%d, DHCP Client Events Handler Thread started\n", __FUNCTION__, __LINE__));
    DhcpEventData_t *pDhcpEvtData = (DhcpEventData_t *)pArg;

    if (DHCP_IPv4 == pDhcpEvtData->dhcpVersion)
    {
        switch (pDhcpEvtData->dhcpMsgType)
        {
            case DHCP_CLIENT_STARTED:
            case DHCP_CLIENT_STOPPED:
            case DHCP_LEASE_RENEW:
            case DHCP_LEASE_DEL:
            case DHCP_LEASE_UPDATE:
            case DHCP_CLIENT_FAILED:
            {
                pthread_mutex_lock(&voiceDataProcessingMutex);
                processVoiceDhcpEvent((DhcpEventData_t *)pDhcpEvtData);
                pthread_mutex_unlock(&voiceDataProcessingMutex);
                break;
            }

            default:
                CcspTraceWarning(("%s: Unrecognized DHCPv4 message type %d\n", __FUNCTION__, pDhcpEvtData->dhcpMsgType));
                break;
        }
    }
    else
    {
        CcspTraceError(("%s: Unsupported DHCP version %d\n", __FUNCTION__, pDhcpEvtData->dhcpVersion));
    }
    free(pDhcpEvtData);
    pthread_exit(NULL);
}
