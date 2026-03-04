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
#include <sys/ioctl.h>
#include <net/if.h>

#define VOICE_SUPPORT_MODE_IPV4_ONLY    "IPv4_Only"
#define VOICE_SUPPORT_MODE_DUAL_STACK   "Dual_Stack"
#define  WAN_MANAGER_INTERFACE_ACTIVE_STATUS_PARAM "Device.X_RDK_WanManager.InterfaceActiveStatus"

pthread_mutex_t voiceDataProcessingMutex = PTHREAD_MUTEX_INITIALIZER;
/**
 * @brief Read the EMTA MAC address from the factory NVRAM file.
 *
 * This helper function scans the file "/tmp/factory_nvram.data" for a line
 * beginning with the literal prefix "EMTA " and, if found, copies the
 * remainder of that line (after the prefix and with the trailing newline
 * removed) into the caller-supplied buffer as a null-terminated string.
 *
 * The MAC address format is whatever textual representation is stored in
 * the file (for example, a hex string with or without separators); the
 * string is copied verbatim without validation or normalization.
 *
 * @param[in,out] pMacAddress
 *      Pointer to a character buffer that receives the MAC address string.
 *      The pointer must be non-NULL and reference a buffer of at least
 *      32 bytes in size to ensure sufficient space for the MAC address
 *
 * @note
 *      This function does not return a status; on failure to open or parse
 *      the file, it logs an error via CcspTraceError and leaves the contents
 *      of pMacAddress unchanged or partially unchanged.
 */
static void readMacAddress (char * pMacAddress)
{
	FILE *pFILE = fopen("/tmp/factory_nvram.data", "r");
	if (pFILE != NULL)
	{
		char cLine[128] = {0};
		while (fgets(cLine, sizeof(cLine), pFILE) != NULL)
		{
			if (strncmp(cLine, "EMTA ",5) == 0)
			{
				char *pMac = cLine + 5;
				pMac[strcspn(pMac, "\n")] = 0; // Remove newline character
				strncpy(pMacAddress, pMac, 32);
				break;
			}
		}
		fclose(pFILE);
	}
    else
    {
        CcspTraceError(("%s: Failed to open /tmp/factory_nvram.data\n", __FUNCTION__));
    }
}
/*
 * @brief Check if the specified network interface is already up.
 *
 * This helper function checks whether the given network interface is
 * currently in the "up" state by querying its flags via ioctl.
 *
 * @param[in] pIfaceName
 *      Pointer to a null-terminated string containing the name of the
 *      network interface to check (e.g., "mta0").
 * @return
 *      Returns true if the interface is up, false otherwise.
*/
static bool IsIfaceAlreadyUp(char *pIfaceName)
{
    bool isUp = false;
    if (NULL == pIfaceName)
    {
        CcspTraceError(("%s: NULL parameters are passed \n", __FUNCTION__));
        return false;
    }

    int iSocketFd = socket (AF_INET, SOCK_DGRAM, 0);
    if (iSocketFd < 0)
    {
        CcspTraceError(("%s: socket creation failed\n", __FUNCTION__));
        return false;
    }

    //Set the interface name
    struct ifreq ifr = {0};
    strncpy(ifr.ifr_name, pIfaceName, IFNAMSIZ-1);
    ifr.ifr_name[IFNAMSIZ-1] = '\0';

    // Check if interface exists
    if (ioctl(iSocketFd, SIOCGIFFLAGS, &ifr) < 0)
    {
        CcspTraceError(("%s: ioctl SIOCGIFFLAGS failed for interface %s\n", __FUNCTION__, pIfaceName));
        close(iSocketFd);
        return false;
    }
    // Check if interface is up
    if (ifr.ifr_flags & IFF_UP)
    {
        CcspTraceInfo(("%s: Interface %s is already up\n", __FUNCTION__, pIfaceName));
        isUp = true;
    }
    close(iSocketFd);
    return isUp;
}
/*
 * @brief Check if the specified network interface has an IP address assigned.
 *
 * This helper function checks whether the given network interface has an IP address assigned by querying its address via ioctl.
 *
 * @param[in] pIfaceName
 *      Pointer to a null-terminated string containing the name of the
 *      network interface to check (e.g., "mta0").
 * @return
 *      Returns true if the interface has an IP address assigned, false otherwise.
*/
static bool isIfaceHasIp(char *pIfaceName)
{
    bool hasIp = false;
    if (NULL == pIfaceName)
    {
        CcspTraceError(("%s: NULL parameters are passed \n", __FUNCTION__));
        return false;
    }

    int iSocketFd = socket (AF_INET, SOCK_DGRAM, 0);
    if (iSocketFd < 0)
    {
        CcspTraceError(("%s: socket creation failed\n", __FUNCTION__));
        return false;
    }

    //Set the interface name
    struct ifreq ifr = {0};
    strncpy(ifr.ifr_name, pIfaceName, IFNAMSIZ-1);
    ifr.ifr_name[IFNAMSIZ-1] = '\0';

    // Check if interface has IP address
    if (ioctl(iSocketFd, SIOCGIFADDR, &ifr) == 0)
    {
        CcspTraceInfo(("%s: Interface %s has IP address assigned\n", __FUNCTION__, pIfaceName));
        hasIp = true;
    }
    close(iSocketFd);
    return hasIp;
}
/**
 * @brief Check if EPON is active.
 *
 * This function checks the status of the EPON interface by querying
 * the WAN manager component via RBUS. It returns true if EPON is active,
 * false otherwise.
 *
 * @return
 *      Returns true if EPON is active, false otherwise.
 */
static bool checkEponIsActive(void)
{
    char cValue[128] = {0};
    getParamRetry(WAN_MANAGER_INTERFACE_ACTIVE_STATUS_PARAM, cValue, sizeof(cValue));
    if ('\0' == cValue[0])
    {
        CcspTraceError(("%s: Failed to get WAN manager interface active status\n", __FUNCTION__));
        return false;
    }
    if ((strstr(cValue, "EPON,1") != NULL) || (strstr(cValue, "WANOE,1") != NULL))
    {
       CcspTraceInfo(("%s: EPON or WANOE is active\n", __FUNCTION__));
       return true;
    }
    else
    {
       CcspTraceInfo(("%s: EPON and WANOE are not active, skipping MTA interface creation\n", __FUNCTION__));
       return false;
    }
}
/*
 * @brief Create the MTA network interface as a macvlan linked to the WAN interface.
 * This function creates a macvlan interface with the specified name,
 * assigns it the MAC address read from factory NVRAM, and brings the interface up.
 * @param[in] pVoiceSupportIfaceName
 *      Pointer to a null-terminated string containing the name of the
 *      MTA network interface to create (e.g., "mta0").
 * @return
 *      Returns 0 on success, -1 on failure.
*/
static int createMtaInterface(char * pVoiceSupportIfaceName)
{

    char cMtaInterfaceMac[32] = {0};
    char cWanIfname[32] = {0};

    if (NULL == pVoiceSupportIfaceName)
    {
        CcspTraceError(("%s: NULL parameters are passed \n", __FUNCTION__));
        return -1;
    }

    //Read the mac address from platform_hal_GetMTAMacAddress API once it is implemented
    readMacAddress(cMtaInterfaceMac);
    if (cMtaInterfaceMac[0] == '\0') {
        CcspTraceError(("%s: readMacAddress failed to get MAC address\n", __FUNCTION__));
        return -1;
    }
    CcspTraceInfo(("%s:%d, MTA MacVlan Mac is %s\n", __FUNCTION__, __LINE__, cMtaInterfaceMac));
  
    if (false == checkEponIsActive())
    {
        return -1;
    }
    getWanIfaceName(cWanIfname, sizeof(cWanIfname));
    if (cWanIfname[0] == '\0')
        snprintf(cWanIfname, sizeof(cWanIfname), "erouter0");

    if (false == IsIfaceAlreadyUp(pVoiceSupportIfaceName))
    {
        //Create the macVlan
        CcspTraceInfo(("%s:%d, Creating macVlan interface %s with mac %s\n", __FUNCTION__, __LINE__, pVoiceSupportIfaceName, cMtaInterfaceMac));
        char cCmd[128] = {0};
        snprintf(cCmd, sizeof(cCmd), "ip link add link %s name %s type macvlan mode bridge", cWanIfname, pVoiceSupportIfaceName);
        system(cCmd);
        snprintf(cCmd, sizeof(cCmd), "ip link set dev %s address %s", pVoiceSupportIfaceName, cMtaInterfaceMac);
        system(cCmd);
        snprintf(cCmd, sizeof(cCmd), "ip link set dev %s up", pVoiceSupportIfaceName);
        system(cCmd);
        CcspTraceInfo(("%s:%d, Created macVlan interface %s\n", __FUNCTION__, __LINE__, pVoiceSupportIfaceName));
    }
    else
    {
        CcspTraceInfo(("%s:%d, MTA interface %s is already up, skipping creation\n", __FUNCTION__, __LINE__, pVoiceSupportIfaceName));
    }
    return 0;
}

/*
 * @brief Start the voice support feature by creating the MTA interface
 *        and enabling DHCPv4 if necessary.
 *
 * This function checks syscfg settings to determine if voice support is
 * enabled and, if so, creates the MTA network interface and subscribes
 * to DHCP client events. If the voice support mode includes IPv4, it
 * enables DHCPv4 on the MTA interface if it does not already have an IP
 * address assigned.
 */
void startVoiceFeature(void)
{

    char cVoiceSupportEnabled[8] = {0};
    char cVoiceSupportMode[32] = {0};
    char cVoiceSupportIfaceName[32] = {0};


    syscfg_get(NULL, "VoiceSupport_IfaceName",cVoiceSupportIfaceName, sizeof(cVoiceSupportIfaceName));
    syscfg_get(NULL, "VoiceSupport_Mode",cVoiceSupportMode, sizeof(cVoiceSupportMode));
    syscfg_get(NULL, "VoiceSupport_Enabled", cVoiceSupportEnabled, sizeof(cVoiceSupportEnabled));

    if (cVoiceSupportEnabled[0] == '\0' || strcmp(cVoiceSupportEnabled, "true") != 0)
    {
        CcspTraceError(("%s:%d, VoiceSupport_Enabled is false or not set, skipping MTA interface creation\n", __FUNCTION__, __LINE__));
        return;
    }

    if (cVoiceSupportIfaceName[0] == '\0')
    {
        CcspTraceWarning(("%s:%d, VoiceSupport_IfaceName not set in syscfg, using default mta0\n", __FUNCTION__, __LINE__));
        snprintf(cVoiceSupportIfaceName, sizeof(cVoiceSupportIfaceName), "mta0");
    }

    if (cVoiceSupportMode[0] == '\0')
    {
        CcspTraceWarning(("%s:%d, VoiceSupport_Mode not set in syscfg, using default Dual_Stack\n", __FUNCTION__, __LINE__));
        snprintf(cVoiceSupportMode, sizeof(cVoiceSupportMode), VOICE_SUPPORT_MODE_DUAL_STACK);
    }

    if (createMtaInterface(cVoiceSupportIfaceName) == 0)
    {
        CcspTraceInfo(("%s:%d, MTA interface created successfully\n", __FUNCTION__, __LINE__));
    } else {
        CcspTraceError(("%s:%d, Failed to create MTA interface\n", __FUNCTION__, __LINE__));
        return;
    }
    subscribeDhcpClientEvents();

    if (0 == strcmp(cVoiceSupportMode, VOICE_SUPPORT_MODE_IPV4_ONLY) || 0 == strcmp(cVoiceSupportMode, VOICE_SUPPORT_MODE_DUAL_STACK))
    {
        CcspTraceInfo(("%s:%d, Starting udhcpc on MTA interface\n", __FUNCTION__, __LINE__));
        if (false == isIfaceHasIp(cVoiceSupportIfaceName))
            enableDhcpv4ForMta(cVoiceSupportIfaceName);
    } else {
        CcspTraceInfo(("%s:%d, VoiceSupport_Mode: %s is not set to %s or %s, skipping udhcpc start\n",__FUNCTION__, __LINE__, cVoiceSupportMode, VOICE_SUPPORT_MODE_IPV4_ONLY, VOICE_SUPPORT_MODE_DUAL_STACK));
    }
}
/**
 * @brief stop the voice support feature by deleting the MTA interface and disabling DHCPv4 if necessary.
 */
void stopVoiceFeature(void)
{
    char cVoiceSupportIfaceName[32] = {0};

    disableDhcpv4ForMta();
    syscfg_get(NULL, "VoiceSupport_IfaceName",cVoiceSupportIfaceName, sizeof(cVoiceSupportIfaceName));
    if (cVoiceSupportIfaceName[0] == '\0')
    {
        CcspTraceWarning(("%s:%d, VoiceSupport_IfaceName not set in syscfg, using default mta0\n", __FUNCTION__, __LINE__));
        snprintf(cVoiceSupportIfaceName, sizeof(cVoiceSupportIfaceName), "mta0");
    }
    char cCmd[128] = {0};
    snprintf(cCmd, sizeof(cCmd), "ip link delete %s", cVoiceSupportIfaceName);
    system(cCmd);
    CcspTraceInfo(("%s:%d, Deleted MTA interface %s\n", __FUNCTION__, __LINE__, cVoiceSupportIfaceName));
    CcspTraceInfo(("%s:%d, Stopped voice support feature\n", __FUNCTION__, __LINE__));
}

/*
 *@brief Add IP route details for the MTA interface based on DHCP event data
 *@param pDhcpEvtData - Pointer to the DHCP event data structure
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


    char cParamName[256] = {0};
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
    //calculate CIDR notation
    CcspTraceInfo(("%s:%d, Network Address:%s\n",__FUNCTION__,__LINE__,cNetworkAddr));
    snprintf(cNetworkAddrWithCidr, sizeof(cNetworkAddrWithCidr), "%s/%d", cNetworkAddr, __builtin_popcount(ntohl(netMask.s_addr)));
    CcspTraceInfo(("%s:%d, Network Address in CIDR notation:%s\n",__FUNCTION__,__LINE__,cNetworkAddrWithCidr));

    snprintf(cParamName, sizeof(cParamName), "ip route add %s dev %s", pDhcpEvtData->leaseInfo.dhcpV4Msg.address, pDhcpEvtData->cIfaceName);
    system(cParamName);
    snprintf(cParamName, sizeof(cParamName), "ip route add %s via %s dev %s", pDhcpEvtData->leaseInfo.dhcpV4Msg.cTftpServer, pDhcpEvtData->leaseInfo.dhcpV4Msg.gateway, pDhcpEvtData->cIfaceName);
    system(cParamName);
    snprintf(cParamName, sizeof(cParamName), "ip rule add from %s table 21", pDhcpEvtData->leaseInfo.dhcpV4Msg.address);
    system(cParamName);
    snprintf(cParamName, sizeof(cParamName), "ip route add %s dev %s table 21", cNetworkAddrWithCidr, pDhcpEvtData->cIfaceName);
    system(cParamName);
    snprintf(cParamName, sizeof(cParamName), "ip route add default via %s dev %s table 21", pDhcpEvtData->leaseInfo.dhcpV4Msg.gateway, pDhcpEvtData->cIfaceName);
    system(cParamName);
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

    voice_hal_interface_info_notify(&sVoiceInterfaceInfoType);
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
