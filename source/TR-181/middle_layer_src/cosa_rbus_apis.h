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

typedef enum
{
    BOOLEAN_PARAM = 0,      /**< bool true or false */
    STRING_PARAM,           /**< null-terminated string */
    PARAM_NONE
} paramValueType_t;

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
 *
 * @param[in] pIfaceName  Name of the MTA network interface for which
 *                        DHCPv4 should be enabled.
 */
void enableDhcpv4ForMta(const char * pIfaceName);

