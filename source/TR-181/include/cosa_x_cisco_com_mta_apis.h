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

    module: cosa_x_cisco_com_mta_apis.h

        For COSA Data Model Library Development

    -------------------------------------------------------------------

    description:

        This file defines the apis for objects to support Data Model Library.

    -------------------------------------------------------------------


    author:

        COSA XML TOOL CODE GENERATOR 1.0

    -------------------------------------------------------------------

    revision:

        01/11/2011    initial revision.

**************************************************************************/


#ifndef  _COSA_MTA_APIS_H
#define  _COSA_MTA_APIS_H

#include "cosa_apis.h"
#include "plugin_main_apis.h"
#include <netinet/in.h>

#ifndef MTA_HAL_SHORT_VALUE_LEN
#define  MTA_HAL_SHORT_VALUE_LEN   16
#endif

#ifndef MTA_HAL_LONG_VALUE_LEN
#define  MTA_HAL_LONG_VALUE_LEN   64
#endif

/**********************************************************************
                STRUCTURE AND CONSTANT DEFINITIONS
**********************************************************************/
typedef  struct
_COSA_MTA_DHCP_INFO
{
    ANSC_IPV4_ADDRESS               IPAddress;
    char                            BootFileName[64];
    char                            FQDN[64];
    ANSC_IPV4_ADDRESS               SubnetMask;
    ANSC_IPV4_ADDRESS               Gateway;
    ULONG                           LeaseTimeRemaining;
    char                            RebindTimeRemaining[64];
    char                            RenewTimeRemaining[64];
    ANSC_IPV4_ADDRESS               PrimaryDNS;
    ANSC_IPV4_ADDRESS               SecondaryDNS;
    char                            DHCPOption3[64];
    char                            DHCPOption6[64];
    char                            DHCPOption7[64];
    char                            DHCPOption8[64];
    char                            PCVersion[64];
    char                            MACAddress[64];
    ANSC_IPV4_ADDRESS               PrimaryDHCPServer;
    ANSC_IPV4_ADDRESS               SecondaryDHCPServer;
}
COSA_MTA_DHCP_INFO, *PCOSA_MTA_DHCP_INFO;

typedef  struct
_COSA_MTA_DHCPv6_INFO
{
    CHAR                            IPV6Address[INET6_ADDRSTRLEN];
    CHAR                            BootFileName[64];
    CHAR                            FQDN[64];
    CHAR                            Prefix[INET6_ADDRSTRLEN];
    CHAR                            Gateway[INET6_ADDRSTRLEN];
    ULONG                           LeaseTimeRemaining;
    CHAR                            RebindTimeRemaining[64];
    CHAR                            RenewTimeRemaining[64];
    CHAR                            PrimaryDNS[INET6_ADDRSTRLEN];
    CHAR                            SecondaryDNS[INET6_ADDRSTRLEN];
    CHAR                            DHCPOption3[64];
    CHAR                            DHCPOption6[64];
    CHAR                            DHCPOption7[64];
    CHAR                            DHCPOption8[64];
    CHAR                            PCVersion[64];
    CHAR                            MACAddress[64];
    CHAR                            PrimaryDHCPv6Server[INET6_ADDRSTRLEN];
    CHAR                            SecondaryDHCPv6Server[INET6_ADDRSTRLEN];
}
COSA_MTA_DHCPv6_INFO, *PCOSA_MTA_DHCPv6_INFO;

typedef  struct
_COSA_MTA_PKTC
{
    BOOLEAN                         pktcMtaDevEnabled;
    ULONG                           pktcSigDefCallSigTos;
    ULONG                           pktcSigDefMediaStreamTos;
    ULONG                           pktcMtaDevRealmOrgName;
    ULONG                           pktcMtaDevCmsKerbRealmName;
    ULONG                           pktcMtaDevCmsIpsecCtrl;
    ULONG                           pktcMtaDevCmsSolicitedKeyTimeout;
    ULONG                           pktcMtaDevRealmPkinitGracePeriod;
}
COSA_MTA_PKTC, *PCOSA_MTA_PKTC;

typedef  struct
_COSA_MTA_SERVICE_CLASS
{
    char                            ServiceClassName[64];
}
COSA_MTA_SERVICE_CLASS, *PCOSA_MTA_SERVICE_CLASS;

typedef  struct
_COSA_MTA_SERVICE_FLOW
{
    ULONG                           SFID;
    char                            ServiceClassName[256];
    char                            Direction[16];
    ULONG                           ScheduleType;
    BOOLEAN                         DefaultFlow;
    ULONG                           NomGrantInterval;
    ULONG                           UnsolicitGrantSize;
    ULONG                           TolGrantJitter;
    ULONG                           NomPollInterval;
    ULONG                           MinReservedPkt;
    ULONG                           MaxTrafficRate;
    ULONG                           MinReservedRate;
    ULONG                           MaxTrafficBurst;
    char                            TrafficType[64];
    ULONG                           NumberOfPackets;
}
COSA_MTA_SERVICE_FLOW, *PCOSA_MTA_SERVICE_FLOW;

typedef  struct
_COSA_MTA_DECT
{
    ULONG                           RegisterDectHandset;
    ULONG                           DeregisterDectHandset;
    char                            HardwareVersion[64];
    char                            RFPI[64];
    char                            SoftwareVersion[64];
    char                            PIN[64];
}
COSA_MTA_DECT,  *PCOSA_MTA_DECT;

typedef  struct
_COSA_MTA_HANDSETS_INFO
{
    ULONG                           InstanceNumber;
    BOOLEAN                         Status;
    char                            LastActiveTime[64];
    char                            HandsetName[64];
    char                            HandsetFirmware[64];
    char                            OperatingTN[64];
    char                            SupportedTN[64];
}
COSA_MTA_HANDSETS_INFO,  *PCOSA_MTA_HANDSETS_INFO;

typedef  struct
_COSA_MTA_CALLS
{
    CHAR                            Codec[64];
    CHAR                            RemoteCodec[64];
    CHAR                            CallStartTime[64];
    CHAR                            CallEndTime[64];
    CHAR                            CWErrorRate[MTA_HAL_SHORT_VALUE_LEN];
    CHAR                            PktLossConcealment[MTA_HAL_SHORT_VALUE_LEN];
    BOOLEAN                         JitterBufferAdaptive;
    BOOLEAN                         Originator;
    ANSC_IPV4_ADDRESS               RemoteIPAddress;
    ULONG                           CallDuration;
    CHAR                            CWErrors[MTA_HAL_SHORT_VALUE_LEN];                     /* code word errors on this channel */
    CHAR                            SNR[MTA_HAL_SHORT_VALUE_LEN];                          /* signal to noise ratio * 256 */
    CHAR                            MicroReflections[MTA_HAL_SHORT_VALUE_LEN];             /* return loss measurement */
    CHAR                            DownstreamPower[MTA_HAL_SHORT_VALUE_LEN];              /* downstream power in dbmv */
    CHAR                            UpstreamPower[MTA_HAL_SHORT_VALUE_LEN];                /* upstream power in dbmv */
    CHAR                            EQIAverage[MTA_HAL_SHORT_VALUE_LEN];                   /* EQI average */    
    CHAR                            EQIMinimum[MTA_HAL_SHORT_VALUE_LEN];                   /* EQI minimum */
    CHAR                            EQIMaximum[MTA_HAL_SHORT_VALUE_LEN];                   /* EQI maximum */
    CHAR                            EQIInstantaneous[MTA_HAL_SHORT_VALUE_LEN];             /* EQI instantaneous */
    CHAR                            MOS_LQ[MTA_HAL_SHORT_VALUE_LEN];                       /* mean opinion score of listening quality, 10-50 */
    CHAR                            MOS_CQ[MTA_HAL_SHORT_VALUE_LEN];                       /* mean opinion score of conversational quality, 10-50 */
    CHAR                            EchoReturnLoss[MTA_HAL_SHORT_VALUE_LEN];               /* residual echo return loss, in db */
    CHAR                            SignalLevel[MTA_HAL_SHORT_VALUE_LEN];                  /* voice signal relative level, in db */
    CHAR                            NoiseLevel[MTA_HAL_SHORT_VALUE_LEN];                   /* noise relative level, in db */
    CHAR                            LossRate[MTA_HAL_SHORT_VALUE_LEN];                     /* fraction of RTP data packet loss * 256 */
    CHAR                            DiscardRate[MTA_HAL_SHORT_VALUE_LEN];                  /* fraction of RTP data packet discarded * 256 */
    CHAR                            BurstDensity[MTA_HAL_SHORT_VALUE_LEN];                 /* fraction of bursting data packet * 256 */
    CHAR                            GapDensity[MTA_HAL_SHORT_VALUE_LEN];                   /* fraction of packets within inter-burst gap * 256 */
    CHAR                            BurstDuration[MTA_HAL_SHORT_VALUE_LEN];                /* mean duration of bursts, in milliseconds */
    CHAR                            GapDuration[MTA_HAL_SHORT_VALUE_LEN];                  /* mean duration of gaps, in milliseconds */
    CHAR                            RoundTripDelay[MTA_HAL_SHORT_VALUE_LEN];               /* most recent measured RTD, in milliseconds */
    CHAR                            Gmin[MTA_HAL_SHORT_VALUE_LEN];                         /* local gap threshold */
    CHAR                            RFactor[MTA_HAL_SHORT_VALUE_LEN];                      /* voice quality evaluation for this RTP session */
    CHAR                            ExternalRFactor[MTA_HAL_SHORT_VALUE_LEN];              /* voice quality evaluation for segment on network external to this RTP session */
    CHAR                            JitterBufRate[MTA_HAL_SHORT_VALUE_LEN];                /* adjustment rate of jitter buffer, in milliseconds */
    CHAR                            JBNominalDelay[MTA_HAL_SHORT_VALUE_LEN];               /* nominal jitter buffer length, in milliseconds */
    CHAR                            JBMaxDelay[MTA_HAL_SHORT_VALUE_LEN];                   /* maximum jitter buffer length, in milliseconds */
    CHAR                            JBAbsMaxDelay[MTA_HAL_SHORT_VALUE_LEN];                /* absolute maximum delay, in milliseconds */
    CHAR                            TxPackets[MTA_HAL_SHORT_VALUE_LEN];                    /* count of transmitted packets */
    CHAR                            TxOctets[MTA_HAL_SHORT_VALUE_LEN];                     /* count of transmitted octet packets */
    CHAR                            RxPackets[MTA_HAL_SHORT_VALUE_LEN];                    /* count of received packets */
    CHAR                            RxOctets[MTA_HAL_SHORT_VALUE_LEN];                     /* count of received octet packets */
    CHAR                            PacketLoss[MTA_HAL_SHORT_VALUE_LEN];                   /* count of lost packets */
    CHAR                            IntervalJitter[MTA_HAL_SHORT_VALUE_LEN];               /* stat variance of packet interarrival time, in milliseconds */
    CHAR                            RemoteIntervalJitter[MTA_HAL_SHORT_VALUE_LEN];         /* remote sie IntervalJitter (see local side) */
    CHAR                            RemoteMOS_LQ[MTA_HAL_SHORT_VALUE_LEN];                 /* remote side MOS_LQ (see local side) */
    CHAR                            RemoteMOS_CQ[MTA_HAL_SHORT_VALUE_LEN];                 /* remote side MOS_CQ (see local side) */
    CHAR                            RemoteEchoReturnLoss[MTA_HAL_SHORT_VALUE_LEN];         /* remote side EchoReturnLoss (see local side) */
    CHAR                            RemoteSignalLevel[MTA_HAL_SHORT_VALUE_LEN];            /* remote side SignalLevel (see local side) */
    CHAR                            RemoteNoiseLevel[MTA_HAL_SHORT_VALUE_LEN];             /* remote side NoiseLevel (see local side) */
    CHAR                            RemoteLossRate[MTA_HAL_SHORT_VALUE_LEN];               /* remote side LossRate (see local side) */
    CHAR                            RemotePktLossConcealment[MTA_HAL_SHORT_VALUE_LEN];     /* remote side PktLossConcealment (see local side) */
    CHAR                            RemoteDiscardRate[MTA_HAL_SHORT_VALUE_LEN];            /* remote side DiscardRate (see local side) */
    CHAR                            RemoteBurstDensity[MTA_HAL_SHORT_VALUE_LEN];           /* remote side BurstDensity (see local side) */
    CHAR                            RemoteGapDensity[MTA_HAL_SHORT_VALUE_LEN];             /* remote side GapDensity (see local side) */
    CHAR                            RemoteBurstDuration[MTA_HAL_SHORT_VALUE_LEN];          /* remote side BurstDuration (see local side) */
    CHAR                            RemoteGapDuration[MTA_HAL_SHORT_VALUE_LEN];            /* remote side GapDuration (see local side) */
    CHAR                            RemoteRoundTripDelay[MTA_HAL_SHORT_VALUE_LEN];         /* remote side RoundTripDelay (see local side) */
    CHAR                            RemoteGmin[MTA_HAL_SHORT_VALUE_LEN];                   /* remote side Gmin (see local side) */
    CHAR                            RemoteRFactor[MTA_HAL_SHORT_VALUE_LEN];                /* remote side RFactore (see local side) */
    CHAR                            RemoteExternalRFactor[MTA_HAL_SHORT_VALUE_LEN];        /* remote side ExternalRFactor (see local side) */
    BOOLEAN                         RemoteJitterBufferAdaptive;   	  /* remote side JitterBufferAdaptive (see local side) */
    CHAR                            RemoteJitterBufRate[MTA_HAL_SHORT_VALUE_LEN];          /* remote side JitterBufRate (see local side) */
    CHAR                            RemoteJBNominalDelay[MTA_HAL_SHORT_VALUE_LEN];         /* remote side JBNominalDelay (see local side) */
    CHAR                            RemoteJBMaxDelay[MTA_HAL_SHORT_VALUE_LEN];             /* remote side JBMaxDelay (see local side) */
    CHAR                            RemoteJBAbsMaxDelay[MTA_HAL_SHORT_VALUE_LEN];          /* remote side JBAbsMaxDelay (see local side) */
}
COSA_MTA_CALLS, *PCOSA_MTA_CALLS;

typedef  struct
_COSA_MTA_LINETABLE_INFO
{
    ULONG                           InstanceNumber;

    ULONG                           LineNumber;
    ULONG                           Status; /* 1 = OnHook; 2 = OffHook */
    char                            HazardousPotential[128];
    char                            ForeignEMF[128];
    char                            ResistiveFaults[128];
    char                            ReceiverOffHook[128];
    char                            RingerEquivalency[64];
    char                            CAName[64];
    ULONG                           CAPort;
    ULONG                           MWD;
    ULONG                           CallsNumber;
    PCOSA_MTA_CALLS                 pCalls;
    ULONG                           CallsUpdateTime;
    ULONG                           OverCurrentFault; /* 1 = Normal,2 = Fault */	
}
COSA_MTA_LINETABLE_INFO, *PCOSA_MTA_LINETABLE_INFO;

typedef  struct
_COSA_MTA_CAPPL
{
    char                            LCState[64];
    char                            CallPState[64];
    char                            LoopCurrent[64];
}
COSA_MTA_CAPPL,  *PCOSA_MTA_CAPPL;
    
typedef  struct
_COSA_MTA_DSXLOG
{
    char                            Time[64];
    char                            Description[128];
    ULONG                           ID;
    ULONG                           Level;
}
COSA_MTA_DSXLOG,  *PCOSA_MTA_DSXLOG;

typedef enum _MTA_RESET_TYPE
{
	MTA_RESET=1,
	LINE_RESET
}MTA_RESET_TYPE;


typedef enum
{
    COSA_MTA_INIT = 0,
    COSA_MTA_START,
    COSA_MTA_COMPLETE,
    COSA_MTA_ERROR,
    COSA_MTA_REJECTED,
}COSA_MTA_STATUS;


/**********************************************************************
                FUNCTION PROTOTYPES
**********************************************************************/
//Commend out the broadcom patch changes for now, will be requried to test basic voice functionality supported by broadcom.
#if 0 //defined (VOICE_MTA_SUPPORT)
void
CosaDmlNotifyIf(char *pIpAddr);
#endif

ANSC_STATUS
CosaDmlMTAInit
    (
        ANSC_HANDLE                 hDml,
        PANSC_HANDLE                phContext
    );

ANSC_STATUS
CosaDmlMTAGetDHCPInfo
    (
        ANSC_HANDLE                 hContext,
        PCOSA_MTA_DHCP_INFO         pInfo
    );

ANSC_STATUS
CosaDmlMTAGetDHCPV6Info
    (
        ANSC_HANDLE                 hContext,
        PCOSA_MTA_DHCPv6_INFO       pInfo
    );

ANSC_STATUS
CosaDmlMTAGetPktc
    (
        ANSC_HANDLE                 hContext,
        PCOSA_MTA_PKTC              pPktc
    );

ANSC_STATUS
CosaDmlMTASetPktc
    (
        ANSC_HANDLE                 hContext,
        PCOSA_MTA_PKTC              pPktc
    );

ULONG
CosaDmlMTALineTableGetNumberOfEntries
    (
        ANSC_HANDLE                 hContext
    );

ANSC_STATUS
CosaDmlMTALineTableGetEntry
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulIndex,
        PCOSA_MTA_LINETABLE_INFO    pEntry
    );

ANSC_STATUS
CosaDmlMTATriggerDiagnostics
    (
        //ANSC_HANDLE               hContext
        ULONG                       nIndex
    );

ANSC_STATUS
CosaDmlMTAGetServiceClass
    (
        ANSC_HANDLE                 hContext,
        PULONG                      pulCount,
        PCOSA_MTA_SERVICE_CLASS     *ppCfg
    );

ANSC_STATUS
CosaDmlMTAGetServiceFlow
    (
        ANSC_HANDLE                 hContext,
        PULONG                      pulCount,
        PCOSA_MTA_SERVICE_FLOW      *ppCfg
    );

ANSC_STATUS
CosaDmlMTADectGetEnable
    (
        ANSC_HANDLE                 hContext,
        BOOLEAN                     *pBool
    );
        
ANSC_STATUS
CosaDmlMTADectSetEnable
    (
        ANSC_HANDLE                 hContext,
        BOOLEAN                     bBool
    );

ANSC_STATUS
CosaDmlMTADectGetRegistrationMode
    (
        ANSC_HANDLE                 hContext,
        BOOLEAN                     *pBool
    );

ANSC_STATUS
CosaDmlMTADectSetRegistrationMode
    (
        ANSC_HANDLE                 hContext,
        BOOLEAN                     bBool
    );

ANSC_STATUS
CosaDmlMTAGetDect
    (
        ANSC_HANDLE                 hContext,
        PCOSA_MTA_DECT              pDect
    );

ANSC_STATUS
CosaDmlMTASetDect
    (
        ANSC_HANDLE                 hContext,
        PCOSA_MTA_DECT              pDect
    );

ULONG
CosaDmlMTAHandsetsGetNumberOfEntries
    (
        ANSC_HANDLE                 hContext
    );

ANSC_STATUS
CosaDmlMTAGetDectPIN
    (
        ANSC_HANDLE                 hContext,
        char                        *pPINString
    );

ANSC_STATUS
CosaDmlMTASetDectPIN
    (
        ANSC_HANDLE                 hContext,
        char                        *pPINString
    );

ANSC_STATUS
CosaDmlMTAGetHandsets
    (
        ANSC_HANDLE                 hContext,
        PULONG                      pulCount,
        PCOSA_MTA_HANDSETS_INFO     *ppHandsets
    );

ANSC_STATUS
CosaDmlMTASetHandsets
    (
        ANSC_HANDLE                 hContext,
        PCOSA_MTA_HANDSETS_INFO     pHandsets
    );

ANSC_STATUS
CosaDmlMTAVQMResetStats
    (
        ANSC_HANDLE                 hContext
    );

ANSC_STATUS
CosaDmlMTAGetCalls
    (
        ANSC_HANDLE                 hContext,
        ULONG                       InstanceNumber, /* LineTable's instance number */
        PULONG                      pulCount,
        PCOSA_MTA_CALLS             *ppCfg
    );

ANSC_STATUS
CosaDmlMTAGetCALLP
    (
        ANSC_HANDLE                 hContext,
        PCOSA_MTA_CAPPL             pCallp
    );

ANSC_STATUS
CosaDmlMTAGetDSXLogs
    (
        ANSC_HANDLE                 hContext,
        PULONG                      pulCount,
        PCOSA_MTA_DSXLOG            *ppDSXLog
    );

ANSC_STATUS
CosaDmlMTAGetDSXLogEnable
    (
        ANSC_HANDLE                 hContext,
        BOOLEAN                     *pBool
    );

ANSC_STATUS
CosaDmlMTASetDSXLogEnable
    (
        ANSC_HANDLE                 hContext,
        BOOLEAN                     Bool
    );

ANSC_STATUS
CosaDmlMTAClearDSXLog
    (
        ANSC_HANDLE                 hContext,
		BOOLEAN                     ClearLog
    );

ANSC_STATUS
CosaDmlMTAGetCallSignallingLogEnable
	(
        ANSC_HANDLE                 hContext,
        BOOLEAN                     *pBool
    );

ANSC_STATUS
CosaDmlMTASetCallSignallingLogEnable
    (
        ANSC_HANDLE                 hContext,
        BOOLEAN                     Bool
    );

ANSC_STATUS
CosaDmlMTAClearCallSignallingLog
    (
        ANSC_HANDLE                 hContext,
		BOOLEAN                     ClearLog
    );

ANSC_STATUS
CosaDmlMtaGetResetCount
    (
        ANSC_HANDLE                 hContext,
		MTA_RESET_TYPE              type,
        ULONG                       *pValue
    );

ANSC_STATUS
CosaDmlMtaClearCalls
    (
		ULONG InstanceNumber
    );

typedef  struct
_COSA_DML_MTA_LOG
{
    BOOLEAN                         EnableDECTLog;
    BOOLEAN                         EnableMTALog;
}
COSA_DML_MTA_LOG,  *PCOSA_DML_MTA_LOG;

typedef  struct
_COSA_DML_MTALOG_FULL
{
    ULONG                           Index;
    ULONG                           EventID;
    CHAR                            EventLevel[64];
    CHAR                            Time[64];
    PCHAR                           pDescription;
}
COSA_DML_MTALOG_FULL,  *PCOSA_DML_MTALOG_FULL;

typedef  struct
_COSA_DML_DECTLOG_FULL
{
    ULONG                           Index;
    ULONG                           EventID;
    ULONG                           EventLevel;
    CHAR                            Time[64];
    CHAR                            Description[256];
}
COSA_DML_DECTLOG_FULL,  *PCOSA_DML_DECTLOG_FULL;

ANSC_STATUS
CosaDmlMtaGetMtaLog
    (
        ANSC_HANDLE                 hContext,
        PULONG                      pulCount,
        PCOSA_DML_MTALOG_FULL       *ppCfg        
    );

ANSC_STATUS
CosaDmlMtaGetDectLog
    (
        ANSC_HANDLE                 hContext,
        PULONG                      pulCount,
        PCOSA_DML_DECTLOG_FULL      *ppCfg        
    );


// #ifdef CONFIG_TI_BBU

typedef  struct
_COSA_DML_BATTERY_INFO
{
    CHAR                            ModelNumber[32];
    CHAR                            SerialNumber[32];
    CHAR                            PartNumber[32];
    CHAR                            ChargerFirmwareRevision[32];
}
COSA_DML_BATTERY_INFO,  *PCOSA_DML_BATTERY_INFO;

ANSC_STATUS
CosaDmlMtaBatteryGetInstalled
    (
        ANSC_HANDLE                 hContext,
        PBOOL                       pValue
    );

ANSC_STATUS
CosaDmlMtaBatteryGetTotalCapacity
    (
        ANSC_HANDLE                 hContext,
        PULONG                      pValue
    );

ANSC_STATUS
CosaDmlMtaBatteryGetActualCapacity
    (
        ANSC_HANDLE                 hContext,
        PULONG                      pValue
    );

ANSC_STATUS
CosaDmlMtaBatteryGetRemainingCharge
    (
        ANSC_HANDLE                 hContext,
        PULONG                      pValue
    );

ANSC_STATUS
CosaDmlMtaBatteryGetRemainingTime
    (
        ANSC_HANDLE                 hContext,
        PULONG                      pValue
    );

ANSC_STATUS
CosaDmlMtaBatteryGetNumberofCycles
    (
        ANSC_HANDLE                 hContext,
        PULONG                      pValue
    );

ANSC_STATUS
CosaDmlMtaBatteryGetPowerStatus
    (
        ANSC_HANDLE                 hContext,
        PCHAR                       pValue,
        PULONG                      pSize
    );

ANSC_STATUS
CosaDmlMtaBatteryGetCondition
    (
        ANSC_HANDLE                 hContext,
        PCHAR                       pValue,
        PULONG                      pSize
    );

ANSC_STATUS
CosaDmlMtaBatteryGetStatus
    (
        ANSC_HANDLE                 hContext,
        PCHAR                       pValue,
        PULONG                      pSize
    );

ANSC_STATUS
CosaDmlMtaBatteryGetLife
    (
        ANSC_HANDLE                 hContext,
        PCHAR                       pValue,
        PULONG                      pSize
    );

ANSC_STATUS
CosaDmlMtaBatteryGetInfo
    (
        ANSC_HANDLE                 hContext,
        PCOSA_DML_BATTERY_INFO      pInfo
    );

// #endif /* CONFIG_TI_BBU */

ANSC_STATUS
CosaDmlMtaGetDhcpStatus
    (
		PULONG pIpv4DhcpStatus,
		PULONG pIpv6DhcpStatus
    );

ANSC_STATUS
CosaDmlMtaGetConfigFileStatus
    (
		PULONG pConfigFileStatus
    );

ANSC_STATUS
CosaDmlMtaGetLineRegisterStatus
    (
		PCHAR pcLineRegisterStatus
    );


typedef  struct
_COSA_BOOTSTRAP_STR
{
    CHAR                    ActiveValue[65];
    CHAR		    UpdateSource[16];
}
COSA_BOOTSTRAP_STR;

typedef  struct
_COSA_BOOTSTRAP_INT
{
    INT                     ActiveValue;
    CHAR		    UpdateSource[16];
}
COSA_BOOTSTRAP_INT;

typedef  struct
_COSA_MTA_ETHWAN_PROV_INFO
{
    COSA_BOOTSTRAP_INT	 			  StartupIPMode;
    COSA_BOOTSTRAP_STR                            IPv4PrimaryDhcpServerOptions;
    COSA_BOOTSTRAP_STR	        		  IPv4SecondaryDhcpServerOptions;	
    COSA_BOOTSTRAP_STR                            IPv6PrimaryDhcpServerOptions;
    COSA_BOOTSTRAP_STR                            IPv6SecondaryDhcpServerOptions;
}
COSA_MTA_ETHWAN_PROV_INFO,  *PCOSA_MTA_ETHWAN_PROV_INFO;

typedef enum _MTA_IP_TYPE
{
        MTA_IPV4_TR=0,
        MTA_IPV6_TR,
        MTA_DUAL_STACK_TR
}MTA_IP_TYPE_TR;


ANSC_STATUS
CosaDmlMTASetStartUpIpMode
    (
        PCOSA_MTA_ETHWAN_PROV_INFO  pmtaethpro,
        INT                         bInt
    );

ANSC_STATUS
CosaDmlMTASetPrimaryDhcpServerOptions
    (
        PCOSA_MTA_ETHWAN_PROV_INFO  pmtaethpro,
        char                       *buf,
        MTA_IP_TYPE_TR              type
    );

ANSC_STATUS
CosaDmlMTASetSecondaryDhcpServerOptions
    (
        PCOSA_MTA_ETHWAN_PROV_INFO  pmtaethpro,
        char                       *buf,
        MTA_IP_TYPE_TR              type
    );

ANSC_STATUS
CosaDmlMtaResetNow
    (
		BOOLEAN  bValue
    );

void 
CosaDmlMtaProvisioningStatusGet();

ANSC_STATUS
CosaMTAInitializeEthWanProvJournal
    (
        PCOSA_MTA_ETHWAN_PROV_INFO  pmtaethpro
    );

ANSC_STATUS UpdateJsonParam
        (
                char*                       pKey,
                char*                   PartnerId,
                char*                   pValue,
                char*                   pSource,
                char*                   pCurrentTime
    );

ANSC_STATUS
CosaDmlMTADectRegisterDectHandset
    (
        ANSC_HANDLE                 hContext,
        ULONG                       uValue
    );

ANSC_STATUS
CosaDmlMTADectDeregisterDectHandset
    (
        ANSC_HANDLE                 hContext,
        ULONG                       uValue
    );

#endif /* _COSA_MTA_APIS_H */

//#endif /* CONFIG_TI_PACM */
