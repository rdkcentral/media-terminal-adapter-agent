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
#include "syscfg/syscfg.h"
#include <sys/ioctl.h>
#include <net/if.h>

#define VOICE_SUPPORT_MODE_IPV4_ONLY    "IPv4_Only"
#define VOICE_SUPPORT_MODE_DUAL_STACK   "Dual_Stack"

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
    if (strlen(cMtaInterfaceMac) == 0 || cMtaInterfaceMac[0] == '\0') {
        CcspTraceError(("%s: readMacAddress failed to get MAC address\n", __FUNCTION__));
        return -1;
    }
    CcspTraceInfo(("%s:%d, MTA MacVlan Mac is %s\n", __FUNCTION__, __LINE__, cMtaInterfaceMac));
    getWanIfaceName(cWanIfname, sizeof(cWanIfname));
    if (cWanIfname[0] == '\0')
        snprintf(cWanIfname, sizeof(cWanIfname), "erouter0");

    if (false == IsIfaceAlreadyUp(pVoiceSupportIfaceName))
    {
        //Create the macVlan
        CcspTraceInfo(("%s:%d, Creating macVlan interface %s with mac %s\n", __FUNCTION__, __LINE__, pVoiceSupportIfaceName, cMtaInterfaceMac));
        char cCmd[2048] = {0};
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
    static bool isVoiceFeatureStarted = false;
    char cVoiceSupportEnabled[8] = {0};
    char cVoiceSupportMode[32] = {0};
    char cVoiceSupportIfaceName[32] = {0};

    if (false == isVoiceFeatureStarted)
    {
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
            CcspTraceError(("%s:%d, VoiceSupport_IfaceName not set in syscfg, using default mta0\n", __FUNCTION__, __LINE__));
            snprintf(cVoiceSupportIfaceName, sizeof(cVoiceSupportIfaceName), "mta0");
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
        isVoiceFeatureStarted = true;
    }
}
static void addIpRouteDetails(DhcpEventData_t *pDhcpEvtData)
{
    if (NULL == pDhcpEvtData)
    {
        CcspTraceError(("%s: NULL DHCP event data provided\n", __FUNCTION__));
        return;
    }
#if 0
    char cParamName[128] = {0};
    snprintf(cParamName, sizeof(cParamName), "ip route add %s dev %s", pDhcpEvtData->routeDest, pDhcpEvtData->ifName);

ip route add 21.242.78.1 dev "$mta_interface_name" # Gateway Address
ip route add 96.106.45.210 via 21.242.78.1 dev "$mta_interface_name"  # tftpserver
ip rule add from 21.242.78.16 table 21 # mta ip address
ip route add 21.242.78.0/32 dev mta0 table 21 
ip route add default via 21.242.78.1 dev mta0 table 21 
#endif
}
static void initializeVoiceSupport(DhcpEventData_t *pDhcpEvtData)
{
    if (NULL == pDhcpEvtData)
    {
        CcspTraceError(("%s: NULL DHCP event data provided\n", __FUNCTION__));
        return;
    }
#if 0
typedef struct
{ 
  char    intfName[VOICE_IFNAME_LEN];               
  uint8_t isPhyUp;                                  /* link state (1=up) */
  
  /* DHCPv4 */
  uint8_t isIpv4Up;                                 /* IPv4 address valid */
  char    ipv4Addr[VOICE_IPV4_ADDR_LEN];            /* IP or CIDR, e.g., "192.0.2.10/24" */
  char    v4NextServerIp[VOICE_IPV4_ADDR_LEN];      /* BOOTP siaddr */
  char    v4ServerHostName[VOICE_STRMAX_128];       /* BOOTP sname or opt 66 */
  char    v4BootFileName[VOICE_STRMAX_128];         /* BOOTP file or opt 67 */
  char    v4DnsServers[VOICE_IPV4_ADDR_LEN*4];      /* opt 6, comma-separated */
  char    v4LogServerIp[VOICE_IPV4_ADDR_LEN];       /* opt 7 (syslog), if present */
  char    v4HostName[VOICE_STRMAX_128];             /* opt 12 */
  char    v4DomainName[VOICE_STRMAX_128];           /* opt 15 */
  char    v4ProvServer[VOICE_STRMAX_128];           /* PacketCable v4: Opt122 subopt 3 (IPv4/FQDN) */
  
  /* DHCPv6 */
  uint8_t isIpv6Up;                                 /* at least one global IPv6 present */
  char    ipv6GlobalAddr[VOICE_IPV6_ADDR_LEN];      /* preferred global address */
  char    v6TftpServerIp[VOICE_IPV6_ADDR_LEN];      /* Opt17 subopt 32 (IPv6) */
  char    v6TftpFileName[VOICE_STRMAX_128];         /* Opt17 subopt 33 */
  char    v6SyslogServerIp[VOICE_IPV6_ADDR_LEN];    /* Opt17 subopt 34 (IPv6) */
  char    v6ProvServerIp[VOICE_STRMAX_128];         /* Opt17 subopt 2171:3 (IPv6/FQDN/IPv4 as text) */
  char    v6DnsServers[VOICE_IPV6_ADDR_LEN*4];      /* opt 23, comma-separated */
  char    v6DomainName[VOICE_STRMAX_128];           /* opt 24 (first decoded name) */
  char    v6ClientFqdn[VOICE_STRMAX_128];           /* opt 39 (decoded FQDN) */
} VoiceInterfaceInfoType;
#endif
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
    
    pthread_mutex_lock(&voiceDataProcessingMutex);
    CcspTraceInfo(("%s:%d, DHCP Client Events Handler Thread started\n", __FUNCTION__, __LINE__));
    DhcpEventData_t *pDhcpEvtData = (DhcpEventData_t *)pArg;
    pthread_detach(pthread_self());

    if (DHCP_IPv4 == pDhcpEvtData->dhcpVersion)
    {
        switch (pDhcpEvtData->dhcpMsgType)
        {
            case DHCP_CLIENT_STARTED:
            case DHCP_CLIENT_STOPPED:
            case DHCP_LEASE_RENEW:
            case DHCP_LEASE_DEL:
            case DHCP_LEASE_UPDATE:
            {
                processVoiceDhcpEvent((DhcpEventData_t *)pDhcpEvtData);
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
    pthread_mutex_unlock(&voiceDataProcessingMutex);
    pthread_exit(NULL);
    return NULL;
}
