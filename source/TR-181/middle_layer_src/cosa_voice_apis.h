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

#ifndef _COSA_VOICE_APIS_H_
#define _COSA_VOICE_APIS_H_

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
void startVoiceFeature(void);

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
void * dhcpClientEventsHandlerThread(void * pArg);


#endif
