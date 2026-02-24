/*
 * If not stated otherwise in this file or this component's LICENSE file the
 * following copyright and licenses apply:
 *
 * Copyright 2015 RDK Management
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

/**********************************************************************
   Copyright [2014] [Cisco Systems, Inc.]
 
   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at
 
       http://www.apache.org/licenses/LICENSE-2.0
 
   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
**********************************************************************/

// #ifdef CONFIG_TI_PACM
/**************************************************************************

    module: cosa_x_cisco_com_mta_internal.h

        For COSA Data Model Library Development

    -------------------------------------------------------------------

    description:

        This file defines the apis for objects to support Data Model Library.

    -------------------------------------------------------------------


    author:

        COSA XML TOOL CODE GENERATOR 1.0

    -------------------------------------------------------------------

    revision:

        01/14/2011    initial revision.

**************************************************************************/

#ifndef  _COSA_MTA_INTERNAL_H
#define  _COSA_MTA_INTERNAL_H

#include "cosa_apis.h"
#include "plugin_main_apis.h"
#include "cosa_x_cisco_com_mta_apis.h"

#define COSA_IFSTACK_ALIAS_LENGTH              64
#define MTA_REFRESH_INTERVAL                    120
#define HANDSETS_REFRESH_INTERVAL               10
#define TIME_NO_NEGATIVE(x) ((long)(x) < 0 ? 0 : (x))

#define  COSA_DATAMODEL_MTA_CLASS_CONTENT                                   \
    /* duplication of the base object class content */                      \
    COSA_BASE_CONTENT                                                       \
    /* start of Lan config security object class content */                 \
    PCOSA_MTA_LINETABLE_INFO                    pLineTable;                 \
    ULONG                                       LineTableCount;             \
    ULONG                                       ServiceClassNumber;         \
    PCOSA_MTA_SERVICE_CLASS                     pServiceClass;              \
    ULONG                                       ServiceClassUpdateTime;     \
    ULONG                                       ServiceFlowNumber;          \
    PCOSA_MTA_SERVICE_FLOW                      pServiceFlow;               \
    ULONG                                       ServiceFlowUpdateTime;      \
    PCOSA_MTA_HANDSETS_INFO                     pHandsets;                  \
    ULONG                                       HandsetsNumber;             \
    ULONG                                       HandsetsUpdateTime;         \
    COSA_MTA_PKTC                               Pktc;                       \
    COSA_MTA_DECT                               Dect;                       \
    ULONG                                       DSXLogNumber;               \
    PCOSA_MTA_DSXLOG                            pDSXLog;                    \
    ULONG                                       DSXLogUpdateTime;           \
    COSA_DML_MTA_LOG                MtaLog;                                              \
    ULONG                           MtaLogNumber;                                        \
    ULONG                           MtaLogUpdateTime;                                    \
    PCOSA_DML_MTALOG_FULL           pMtaLog;                                             \
    ULONG                           DectLogNumber;                                       \
    ULONG                           DectLogUpdateTime;                                   \
    PCOSA_DML_DECTLOG_FULL          pDectLog;                                            \
    COSA_DML_BATTERY_INFO           MtaBatteryInfo;                                      \
    PCOSA_MTA_ETHWAN_PROV_INFO      pmtaprovinfo;                                      \


typedef  struct
_COSA_DATAMODEL_MTA_CLASS_CONTENT
{
    COSA_DATAMODEL_MTA_CLASS_CONTENT
}
COSA_DATAMODEL_MTA, *PCOSA_DATAMODEL_MTA;

/*
    Standard function declaration
*/
ANSC_HANDLE
CosaMTACreate
    (
        VOID
    );

ANSC_STATUS
CosaMTAInitialize
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CosaMTARemove
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CosaSetMTAHal
    (
        PCOSA_MTA_ETHWAN_PROV_INFO  pmtaethpro
    );
    
ANSC_STATUS CosaMTALineTableInitialize
    (
        ANSC_HANDLE                 hThisObject
    );
   
    
ANSC_STATUS
CosaMTAInitializeEthWanProv
    (
        ANSC_HANDLE                 hThisObject
    );
ANSC_STATUS
CosaMTAInitializeEthWanProvDhcpOption
    (
        ANSC_HANDLE                 hThisObject
    );
ANSC_STATUS ConverStr2Hex(unsigned char buffer[]);
int getMaxCount();
int checkIfDefMtaDhcpOptionEnabled();
void WaitForDhcpOption();
void * Mta_Sysevent_thread(void *  hThisObject);
void * Mta_Sysevent_thread_Dhcp_Option( void * hThisObject);
/*
 *@brief This function get the wan interface name from sysevent
 *@param pIfname - pointer to store the wan interface name
 *@param iIfnameLen - length of the pointer
 *@return ANSC_STATUS_SUCCESS on success else ANSC_STATUS_FAILURE
*/
ANSC_STATUS getWanIfaceName(char *pIfname, int iIfnameLen);
#endif
// #endif
