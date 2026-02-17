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

#ifndef COSA_RBUS_APIS_H
#define COSA_RBUS_APIS_H

#include "ipc_msg.h"

typedef enum
{
    BOOLEAN_PARAM = 0,      /**< bool true or false */
    STRING_PARAM,           /**< null-terminated string */
    PARAM_NONE
} paramValueType_t;

typedef enum
{
    DHCP_IPv4=0,
    DHCP_IPv6,
    DHCP_BOTH,
    DHCP_NONE
}DhcpVersion;

typedef struct _DhcpEventData
{
    char              cIfaceName[32];
    DHCP_MESSAGE_TYPE dhcpMsgType;
    DhcpVersion      dhcpVersion;
    union
    {
        DHCP_MGR_IPV4_MSG dhcpV4Msg;
        DHCP_MGR_IPV6_MSG dhcpV6Msg;
    }leaseInfo;
} DhcpEventData_t;
/*
* @brief Retrieve interface index information for the MTA interface.
 *
 * This function retrieves Interface index information for the MTA interface
 * from the PSM (Persistent Storage Manager) and logs the retrieved value.
 * It reads the parameter "dmsb.voicesupport.Interface.IP.DHCPV4Interface"
 * from the PSM and stores it in a static variable for later use. If the
 * retrieval fails, it logs an error message.
 */

void getIfaceIndexInfo(void);
/**
 * @brief Initialize the RBUS handle used by this module.
 *
 * This function initializes and/or acquires the RBUS handle required
 * for subsequent RBUS operations performed by this component. It
 * should be called before any other APIs in this module that depend
 * on an active RBUS connection.
 */
void initRbusHandle(void);

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
void enableDhcpv4ForMta(const char * pIfaceName);

/**
 *@brief Disable DHCPv4 for the MTA interface.
 */
void disableDhcpv4ForMta(void);

/**
 * @brief Subscribe to DHCP client events for MTA interface.
 * This function sets up subscriptions to listen for DHCP client
 * events related to the MTA interface.
 */
void subscribeDhcpClientEvents(void);

/**
 * @brief Retrieve parameter value from a component via RBUS.
 *
 * This function retrieves the value of the specified parameter from
 * a component using RBUS. It initializes an rbus value, performs
 * the get operation, and stores the result in the provided buffer.
 * Errors during the process are logged appropriately.
 *
 * @param[in] pParamName   Name of the parameter to be retrieved.
 * @param[out] pParamValue Buffer to store the retrieved parameter value.
 * @param[in] valueSize    Size of the buffer.
 */

void getParamValue(const char * pParamName, char * pParamValue, size_t valueSize);
#endif /* COSA_RBUS_APIS_H */

