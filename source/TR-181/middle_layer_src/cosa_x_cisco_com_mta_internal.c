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

    module: cosa_x_cisco_com_mta_internal.c

        For COSA Data Model Library Development

    -------------------------------------------------------------------

    description:

        This file implementes back-end apis for the COSA Data Model Library

        *  CosaMTACreate
        *  CosaMTAInitialize
        *  CosaMTARemove
    -------------------------------------------------------------------

    environment:

        platform independent

    -------------------------------------------------------------------

    author:

        COSA XML TOOL CODE GENERATOR 1.0

    -------------------------------------------------------------------

    revision:

        01/11/2011    initial revision.

**************************************************************************/

#include "plugin_main_apis.h"
#include "safec_lib_common.h"
#include "cosa_x_cisco_com_mta_apis.h"
#include "cosa_x_cisco_com_mta_dml.h"
#include "cosa_x_cisco_com_mta_internal.h"
#include "mta_hal.h"
#include <sysevent/sysevent.h>
#include "syscfg/syscfg.h"
#include "voice_dhcp_hal.h"
#include "ccsp/autoconf.h"
#include "ccsp/platform_hal.h"

#define MAX_BUFF_SIZE 128
#define MAX_IP_PREF_VAL 6
#define MAX_IPV4_HEX_VAL 16
#define MAX_IPV6_HEX_VAL 65

static int sysevent_fd;
static token_t sysevent_token;
/**********************************************************************

    caller:     owner of the object

    prototype:

        ANSC_HANDLE
        CosaMTACreate
            (
            );

    description:

        This function constructs cosa device info object and return handle.

    argument:  

    return:     newly created device info object.

**********************************************************************/
/* Coverity Fix CID 66908 - NonStd_void_param_list */
ANSC_HANDLE
CosaMTACreate
    (
     void
    )
{
    PCOSA_DATAMODEL_MTA          pMyObject    = (PCOSA_DATAMODEL_MTA)NULL;

    /*
     * We create object by first allocating memory for holding the variables and member functions.
     */
    pMyObject = (PCOSA_DATAMODEL_MTA)AnscAllocateMemory(sizeof(COSA_DATAMODEL_MTA));

    if ( !pMyObject )
    {
        return  (ANSC_HANDLE)NULL;
    }

    /*
     * Initialize the common variables and functions for a container object.
     */
    pMyObject->Oid               = COSA_DATAMODEL_MTA_OID;
    pMyObject->Create            = CosaMTACreate;
    pMyObject->Remove            = CosaMTARemove;
    pMyObject->Initialize        = CosaMTAInitialize;

    pMyObject->Initialize   ((ANSC_HANDLE)pMyObject);

    return  (ANSC_HANDLE)pMyObject;
}

ANSC_STATUS ConverStr2Hex(unsigned char buffer[])
{
       /* Coverity Fix :CID 70174 Declare and Never Used */
	int i = 0, len = 0;
	char 	tbuffer [ MAX_IPV6_HEX_VAL ] = { 0 };
        errno_t rc = -1;
			len = strlen((const char*)buffer);
                        rc = strcpy_s(tbuffer,sizeof(tbuffer), (const char*)buffer);
                        if(rc != EOK)
                        {
                            ERR_CHK(rc);
                            return ANSC_STATUS_FAILURE;
                        }
			printf("len = %d\n",len);
			for(i = 0; i<len; i++)
			{
				if((buffer[i] >= 48) && (buffer[i] <= 57))
				{
					buffer[i] = buffer[i]-48;
				}
				else if((buffer[i] >= 65) && (buffer[i] <= 70))
				{
					buffer[i] = buffer[i]-55;
				}
				else if((buffer[i] >= 97) && (buffer[i] <= 102))
				{
					buffer[i] = buffer[i]-87;
				}
				else
				{
					printf("buffer = %s is not correct\n",tbuffer);
					CcspTraceError(("Unsupported format %s %s\n", tbuffer,__FUNCTION__));
					return ANSC_STATUS_FAILURE;
				}
			}
	return ANSC_STATUS_SUCCESS;

}

#if defined (SCXF10)
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
				strcpy_s(pMacAddress, 32, pMac);
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

static void getDhcpOption43RawData (dhcpOption43RawData_t * pDhcpOption43RawData)
{
    if (NULL == pDhcpOption43RawData)
    {
        CcspTraceError(("%s: Invalid NULL pointer\n", __FUNCTION__));
        return;
    }

    snprintf(pDhcpOption43RawData->cVendorName, sizeof(pDhcpOption43RawData->cVendorName), CONFIG_VENDOR_NAME);
    snprintf(pDhcpOption43RawData->cOUID,sizeof(pDhcpOption43RawData->cOUID), CONFIG_VENDOR_ID);
    if (RETURN_OK != platform_hal_GetModelName(pDhcpOption43RawData->cModelNumber))
        CcspTraceError(("%s: platform_hal_GetModelName failed\n", __FUNCTION__));
    if (RETURN_OK != platform_hal_GetSerialNumber(pDhcpOption43RawData->cSerialNumber))
        CcspTraceError(("%s: platform_hal_GetSerialNumber failed\n", __FUNCTION__));
    if (RETURN_OK != platform_hal_GetHardwareVersion(pDhcpOption43RawData->cHardwareVersion))
        CcspTraceError(("%s: platform_hal_GetHardwareVersion failed\n", __FUNCTION__));
    if (RETURN_OK != platform_hal_GetFirmwareName(pDhcpOption43RawData->cSoftwareVersion, sizeof(pDhcpOption43RawData->cSoftwareVersion)))
        CcspTraceError(("%s: platform_hal_GetFirmwareName failed\n", __FUNCTION__));
    if (RETURN_OK != platform_hal_GetBootloaderVersion(pDhcpOption43RawData->cBootLoaderVersion, sizeof(pDhcpOption43RawData->cBootLoaderVersion)))
        CcspTraceError(("%s: platform_hal_GetBootloaderVersion failed\n", __FUNCTION__));
    readMacAddress(pDhcpOption43RawData->cMtaMacAddress);

    if (strlen(pDhcpOption43RawData->cMtaMacAddress) == 0)
    {
        CcspTraceError(("%s: readMacAddress failed to get MAC address\n", __FUNCTION__));
        snprintf(pDhcpOption43RawData->cMtaMacAddress, sizeof(pDhcpOption43RawData->cMtaMacAddress), "78:B3:9F:8F:F2:25");
    }

    CcspTraceInfo(("%s: Serial Number = %s\n", __FUNCTION__, pDhcpOption43RawData->cSerialNumber));
    CcspTraceInfo(("%s: Hardware Version = %s\n", __FUNCTION__, pDhcpOption43RawData->cHardwareVersion));
    CcspTraceInfo(("%s: Software Version = %s\n", __FUNCTION__, pDhcpOption43RawData->cSoftwareVersion));
    CcspTraceInfo(("%s: Bootloader Version = %s\n", __FUNCTION__, pDhcpOption43RawData->cBootLoaderVersion));
    CcspTraceInfo(("%s: OUI = %s\n", __FUNCTION__, pDhcpOption43RawData->cOUID));
    CcspTraceInfo(("%s: Model Number = %s\n", __FUNCTION__, pDhcpOption43RawData->cModelNumber));
    CcspTraceInfo(("%s: Vendor Name = %s\n", __FUNCTION__, pDhcpOption43RawData->cVendorName));
    CcspTraceInfo(("%s: MTA MAC Address = %s\n", __FUNCTION__, pDhcpOption43RawData->cMtaMacAddress));

    return;
}

static int prepareDhcpOption43(const dhcpOption43RawData_t * pDhcpOption43RawData,char *pOutbuf, int iOutbufLen)
{
    if ((NULL == pDhcpOption43RawData) || (NULL == pOutbuf) || (iOutbufLen < 512)) {
        return -1;
    }
    srand((unsigned int)time(NULL));
    unsigned char cHexBuf[512] = {0};
    int iIndex = 0;

 // Type 02: EDVA identifier (fixed)
    cHexBuf[iIndex++] = 0x02;  // Type
    cHexBuf[iIndex++] = 0x04;  // Length = 4
    cHexBuf[iIndex++] = 0x45;  // 'E'
    cHexBuf[iIndex++] = 0x44;  // 'D'
    cHexBuf[iIndex++] = 0x56;  // 'V'
    cHexBuf[iIndex++] = 0x41;  // 'A'

    CcspTraceInfo(("%s: EDVA Identifier:%02x %02x %02x %02x\n", __FUNCTION__,
        cHexBuf[iIndex - 4], cHexBuf[iIndex - 3], cHexBuf[iIndex - 2], cHexBuf[iIndex - 1]));
    // Type 04: Serial Number
    size_t iSerialLen = strlen(pDhcpOption43RawData->cSerialNumber);
    cHexBuf[iIndex++] = 0x04;  // Type
    cHexBuf[iIndex++] = (unsigned char)iSerialLen;  // Length
    memcpy(&cHexBuf[iIndex], pDhcpOption43RawData->cSerialNumber, iSerialLen);
    iIndex += iSerialLen;

    // Type 05: Hardware Version
    size_t iHardwareVerLen = strlen(pDhcpOption43RawData->cHardwareVersion);
    cHexBuf[iIndex++] = 0x05;  // Type
    cHexBuf[iIndex++] = (unsigned char)iHardwareVerLen;  // Length
    memcpy(&cHexBuf[iIndex], pDhcpOption43RawData->cHardwareVersion, iHardwareVerLen);
    iIndex += iHardwareVerLen;

    // Check for override file
    FILE *pOverride = fopen("/tmp/mtaDhcpOption43.txt", "r");
    int isOverride = (pOverride != NULL);
    if (pOverride)
        fclose(pOverride);

    CcspTraceInfo(("%s: Software and Bootloader Version Override %s\n", __FUNCTION__, isOverride ? "ENABLED" : "DISABLED"));
    // Type 06: software version
    const char *pSoftwareVersion = isOverride ? "Prod_23_2_231009" : pDhcpOption43RawData->cSoftwareVersion;
    size_t iSoftwareVerLen = strlen(pSoftwareVersion);
    cHexBuf[iIndex++] = 0x06;  // Type
    cHexBuf[iIndex++] = (unsigned char)iSoftwareVerLen;  // Length
    memcpy(&cHexBuf[iIndex], pSoftwareVersion, iSoftwareVerLen);
    iIndex += iSoftwareVerLen;

    // Type 07: Bootloader Version
    const char *pBootLoaderVersion = isOverride ? "S1TC-3.63.20.104" : pDhcpOption43RawData->cBootLoaderVersion;
    size_t uiProductLen = strlen(pBootLoaderVersion);
    cHexBuf[iIndex++] = 0x07;  // Type
    cHexBuf[iIndex++] = (unsigned char)uiProductLen;  // Length
    memcpy(&cHexBuf[iIndex], pBootLoaderVersion, uiProductLen);
    iIndex += uiProductLen;

    // Type 08: OUI
    int iOuiLen = 3;//As per standard OUI length is 3 bytes
#if 0
    unsigned char cOui[3] = {0};
    // Try to parse as hex string (e.g., "0030F4" or "00:30:F4")
    if ((sscanf(pDhcpOption43RawData->cOUID, "%2hhx%2hhx%2hhx", &cOui[0], &cOui[1], &cOui[2]) != iOuiLen) &&
       (sscanf(pDhcpOption43RawData->cOUID, "%2hhx:%2hhx:%2hhx", &cOui[0], &cOui[1], &cOui[2]) != iOuiLen)) {
        CcspTraceError(("%s: Invalid OUI format from hal: %s\n", __FUNCTION__, pDhcpOption43RawData->cOUID));
        //Hardcode default OUI which we got from internet for Sercomm Devices
        cOui[0] = 0x00;
        cOui[1] = 0x30;
        cOui[2] = 0xF4;
    }
#endif
    cHexBuf[iIndex++] = 0x08;  // Type
    cHexBuf[iIndex++] = (unsigned char)iOuiLen;  // Length
    memcpy(&cHexBuf[iIndex], pDhcpOption43RawData->cOUID, iOuiLen);
    iIndex += iOuiLen;

    // Type 09: Model Number
    size_t iModelLen = strlen(pDhcpOption43RawData->cModelNumber);
    cHexBuf[iIndex++] = 0x09;  // Type
    cHexBuf[iIndex++] = (unsigned char)iModelLen;  // Length
    memcpy(&cHexBuf[iIndex], pDhcpOption43RawData->cModelNumber, iModelLen);
    iIndex += iModelLen;

    // Type 0A: Vendor Name
    size_t iVendorLen = strlen(pDhcpOption43RawData->cVendorName);
    cHexBuf[iIndex++] = 0x0a;  // Type
    cHexBuf[iIndex++] = (unsigned char)iVendorLen;  // Length
    memcpy(&cHexBuf[iIndex], pDhcpOption43RawData->cVendorName, iVendorLen);
    iIndex += iVendorLen;

    // Type 1F: MAC Address (6 bytes, binary format)
    cHexBuf[iIndex++] = 0x1f;  // Type
    cHexBuf[iIndex++] = 0x06;  // Length = 6 bytes
    // Parse MAC address from string (e.g., "78:B3:9F:8F:F2:25" or "78b39f8ff225")
    unsigned char mac[6];
    if (sscanf(pDhcpOption43RawData->cMtaMacAddress, "%02x:%02x:%02x:%02x:%02x:%02x",
               (unsigned int*)&mac[0], (unsigned int*)&mac[1], (unsigned int*)&mac[2],
               (unsigned int*)&mac[3], (unsigned int*)&mac[4], (unsigned int*)&mac[5]) == 6) {
        memcpy(&cHexBuf[iIndex], mac, 6);
        iIndex += 6;
    } else {
        // Try without colons
        if (sscanf(pDhcpOption43RawData->cMtaMacAddress, "%02x%02x%02x%02x%02x%02x",
                   (unsigned int*)&mac[0], (unsigned int*)&mac[1], (unsigned int*)&mac[2],
                   (unsigned int*)&mac[3], (unsigned int*)&mac[4], (unsigned int*)&mac[5]) == 6) {
            memcpy(&cHexBuf[iIndex], mac, 6);
            iIndex += 6;
        }
    }

    // Type 20: Correlation ID (4 bytes, binary)
    // Generate random correlation ID
    uint32_t ui32CorrelationId = ((uint32_t)rand() << 16) | ((uint32_t)rand() & 0xFFFF);

    cHexBuf[iIndex++] = 0x20;  // Type
    cHexBuf[iIndex++] = 0x04;  // Length = 4 bytes
    cHexBuf[iIndex++] = (unsigned char)((ui32CorrelationId >> 24) & 0xFF);
    cHexBuf[iIndex++] = (unsigned char)((ui32CorrelationId >> 16) & 0xFF);
    cHexBuf[iIndex++] = (unsigned char)((ui32CorrelationId >> 8) & 0xFF);
    cHexBuf[iIndex++] = (unsigned char)(ui32CorrelationId & 0xFF);

    CcspTraceInfo(("%s: Correlation ID: %08x\n", __FUNCTION__, ui32CorrelationId));

    /* Convert to hex string with "0x2b:" prefix */
    int iWritten = snprintf(pOutbuf, iOutbufLen, "0x2b:");
    if (iWritten < 0 || iWritten >= iOutbufLen) {
        return -1;
    }

    for (int i = 0; i < iIndex; i++) {
        int iRet = snprintf(pOutbuf + iWritten, iOutbufLen - iWritten, "%02x", cHexBuf[i]);
        if (iRet < 0 || (iWritten + iRet) >= iOutbufLen) {
            return -1;
        }
        iWritten += iRet;
    }
    return iWritten;
}
static int prepareDhcpOption60(const VoicePktcCapabilitiesType *pVoicePktCap,char *pOutbuf, int iOutbufLen)
{
    if ((NULL == pVoicePktCap) || (NULL == pOutbuf) || (iOutbufLen < 200)) {
        return -1;
	}

    unsigned char cHexBuf[512] = {0};
    int iIndex = 0;
    /* Vendor prefix (Technicolor/Comcast specific) */
    cHexBuf[iIndex++] = 0x05;
    cHexBuf[iIndex++] = 0x49;

    /* Subopt 1: pktcblVersion */
    cHexBuf[iIndex++] = 0x01;
    cHexBuf[iIndex++] = 0x01;
    cHexBuf[iIndex++] = pVoicePktCap->pktcblVersion;

    /* Subopt 2: numEndpoints */
    cHexBuf[iIndex++] = 0x02;
    cHexBuf[iIndex++] = 0x01;
    cHexBuf[iIndex++] = pVoicePktCap->numEndpoints;

    /* Subopt 3: tgtSupport */
    cHexBuf[iIndex++] = 0x03;
    cHexBuf[iIndex++] = 0x01;
    cHexBuf[iIndex++] = pVoicePktCap->tgtSupport;

    /* Subopt 4: httpDownload */
    cHexBuf[iIndex++] = 0x04;
    cHexBuf[iIndex++] = 0x01;
    cHexBuf[iIndex++] = pVoicePktCap->httpDownload;

    /* Subopt 9: nvramInfoStorage */
    cHexBuf[iIndex++] = 0x09;
    cHexBuf[iIndex++] = 0x01;
    cHexBuf[iIndex++] = pVoicePktCap->nvramInfoStorage;

    /* Subopt 11: supportedCodecs */
    cHexBuf[iIndex++] = 0x0b;
    //cHexBuf[iIndex++] = 0x0b;  // Length = 11 bytes
    cHexBuf[iIndex++] = 0x03;  // Length = 3 bytes
    cHexBuf[iIndex++] = pVoicePktCap->supportedCodecs[0];
    cHexBuf[iIndex++] = pVoicePktCap->supportedCodecs[1];
    cHexBuf[iIndex++] = pVoicePktCap->supportedCodecs[2];
     /*cHexBuf[iIndex++] = pVoicePktCap->supportedCodecs[3];
    cHexBuf[iIndex++] = pVoicePktCap->supportedCodecs[4];
    cHexBuf[iIndex++] = pVoicePktCap->supportedCodecs[5];
    cHexBuf[iIndex++] = pVoicePktCap->supportedCodecs[6];
    cHexBuf[iIndex++] = pVoicePktCap->supportedCodecs[7];
    cHexBuf[iIndex++] = pVoicePktCap->supportedCodecs[8];
    cHexBuf[iIndex++] = pVoicePktCap->supportedCodecs[9];
    cHexBuf[iIndex++] = pVoicePktCap->supportedCodecs[10];*/

    /* Subopt 12: silenceSuppression (mapped to subopt 0x0c) */
    cHexBuf[iIndex++] = 0x0c;
    cHexBuf[iIndex++] = 0x01;
    cHexBuf[iIndex++] = pVoicePktCap->silenceSuppression;

    /* Subopt 13: echoCancellation (mapped to subopt 0x0d) */
    cHexBuf[iIndex++] = 0x0d;
    cHexBuf[iIndex++] = 0x01;
    cHexBuf[iIndex++] = pVoicePktCap->echoCancellation;

    /* Subopt 15: ugsAd (mapped to subopt 0x0f) */
    cHexBuf[iIndex++] = 0x0f;
    cHexBuf[iIndex++] = 0x01;
    cHexBuf[iIndex++] = pVoicePktCap->ugsAd;

    /* Subopt 16: ifIndexStart (mapped to subopt 0x10) */
    cHexBuf[iIndex++] = 0x10;
    cHexBuf[iIndex++] = 0x01;
    cHexBuf[iIndex++] = pVoicePktCap->ifIndexStart;

    /* Subopt 18: supportedProvFlow (mapped to subopt 0x12) */
    cHexBuf[iIndex++] = 0x12;
    cHexBuf[iIndex++] = 0x02;  // Length = 2 bytes
    cHexBuf[iIndex++] = 0x00;
    cHexBuf[iIndex++] = pVoicePktCap->supportedProvFlow;

    /* Subopt 19: t38Version (mapped to subopt 0x13) */
    cHexBuf[iIndex++] = 0x13;
    cHexBuf[iIndex++] = 0x01;
    cHexBuf[iIndex++] = pVoicePktCap->t38Version;

    /* Subopt 20: t38ErrorCorrection (mapped to subopt 0x14) */
    cHexBuf[iIndex++] = 0x14;
    cHexBuf[iIndex++] = 0x01;
    cHexBuf[iIndex++] = pVoicePktCap->t38ErrorCorrection;

    /* Subopt 21: rfc2833 (mapped to subopt 0x15) */
    cHexBuf[iIndex++] = 0x15;
    cHexBuf[iIndex++] = 0x01;
    cHexBuf[iIndex++] = pVoicePktCap->rfc2833;

    /* Subopt 22: voiceMetrics (mapped to subopt 0x16) */
    cHexBuf[iIndex++] = 0x16;
    cHexBuf[iIndex++] = 0x01;
    cHexBuf[iIndex++] = pVoicePktCap->voiceMetrics;

    /* Subopt 23: supportedMibs (mapped to subopt 0x17) */
    cHexBuf[iIndex++] = 0x17;
    cHexBuf[iIndex++] = 0x03;  // Length = 3 bytes
    cHexBuf[iIndex++] = pVoicePktCap->supportedMibs[0];
    cHexBuf[iIndex++] = pVoicePktCap->supportedMibs[1];
    cHexBuf[iIndex++] = pVoicePktCap->supportedMibs[2];

    /* Subopt 24: multiGrants (mapped to subopt 0x18) */
    cHexBuf[iIndex++] = 0x18;
    cHexBuf[iIndex++] = 0x01;
    cHexBuf[iIndex++] = pVoicePktCap->multiGrants;

    /* Subopt 25: v_152 (mapped to subopt 0x19) */
    cHexBuf[iIndex++] = 0x19;
    cHexBuf[iIndex++] = 0x01;
    cHexBuf[iIndex++] = pVoicePktCap->v_152;

    /* Subopt 26: certBootstrapping (mapped to subopt 0x1a) */
    cHexBuf[iIndex++] = 0x1a;
    cHexBuf[iIndex++] = 0x01;
    cHexBuf[iIndex++] = pVoicePktCap->certBootstrapping;

    /* Subopt 38: ipAddrProvCap (mapped to subopt 0x26) */
    cHexBuf[iIndex++] = 0x26;
    cHexBuf[iIndex++] = 0x01;
    cHexBuf[iIndex++] = pVoicePktCap->ipAddrProvCap;

    int iWritten = snprintf(pOutbuf, iOutbufLen, "pktc2.0:");
    if (iWritten < 0 || iWritten >= iOutbufLen) {
        return -1;
    }

    for (int iVar = 0; iVar < iIndex; iVar++) {
        int iRet = snprintf(pOutbuf + iWritten, iOutbufLen - iWritten, "%02x", cHexBuf[iVar]);
        if (iRet < 0 || iWritten + iRet >= iOutbufLen) {
            return -1;
        }
        iWritten += iRet;
    }
    return iWritten;
}

static void readDhcpOptionsFromHal(char * pDhcpOption43, int iDhcpOption43Len, char *pDhcpOption60, int iDhcpOption60Len)
{
    if (NULL == pDhcpOption43 || NULL == pDhcpOption60 || iDhcpOption43Len < 512 || iDhcpOption60Len < 256)
    {
        CcspTraceError(("%s: Invalid NULL pointer or insufficient buffer length\n", __FUNCTION__));
        return;
    }
    VoicePktcCapabilitiesType sVoicePktcCapabilities = {0};
    CcspTraceInfo(("%s:<--->Sizeof(VoicePktcCapabilitiesType) = %zu\n", __FUNCTION__, sizeof(VoicePktcCapabilitiesType)));
    uint8_t ui8Ret = voice_hal_get_pktc_capabilities(&sVoicePktcCapabilities);
    CcspTraceInfo(("%s ui8Ret = %d\n", __FUNCTION__, ui8Ret));
    if(ui8Ret == 1)
    {
        CcspTraceInfo(("Original: voice_hal_get_pktc_capabilities Values '%s'\n", __FUNCTION__));
        CcspTraceInfo(("%s: pktcblVersion = %d\n", __FUNCTION__, sVoicePktcCapabilities.pktcblVersion));
        CcspTraceInfo(("%s: numEndpoints = %d\n", __FUNCTION__, sVoicePktcCapabilities.numEndpoints));
        CcspTraceInfo(("%s: tgtSupport = %d\n", __FUNCTION__, sVoicePktcCapabilities.tgtSupport));
        CcspTraceInfo(("%s: httpDownload = %d\n", __FUNCTION__, sVoicePktcCapabilities.httpDownload));
        CcspTraceInfo(("%s: nvramInfoStorage = %d\n", __FUNCTION__, sVoicePktcCapabilities.nvramInfoStorage));
        /*CcspTraceInfo(("%s: supportedCodecs = %02x %02x %02x %2x %02x %02x %02x %02x %02x %02x %02x\n", __FUNCTION__,
                    sVoicePktcCapabilities.supportedCodecs[0], sVoicePktcCapabilities.supportedCodecs[1], sVoicePktcCapabilities.supportedCodecs[2],
                    sVoicePktcCapabilities.supportedCodecs[3], sVoicePktcCapabilities.supportedCodecs[4], sVoicePktcCapabilities.supportedCodecs[5],
                    sVoicePktcCapabilities.supportedCodecs[6], sVoicePktcCapabilities.supportedCodecs[7], sVoicePktcCapabilities.supportedCodecs[8],
                    sVoicePktcCapabilities.supportedCodecs[9], sVoicePktcCapabilities.supportedCodecs[10]));*/
        CcspTraceInfo(("%s: supportedCodecs = %02x %02x %02x\n", __FUNCTION__,
                    sVoicePktcCapabilities.supportedCodecs[0], sVoicePktcCapabilities.supportedCodecs[1], sVoicePktcCapabilities.supportedCodecs[2]));
        CcspTraceInfo(("%s: silenceSuppression = %d\n", __FUNCTION__, sVoicePktcCapabilities.silenceSuppression));
        CcspTraceInfo(("%s: echoCancellation = %d\n", __FUNCTION__, sVoicePktcCapabilities.echoCancellation));
        CcspTraceInfo(("%s: ugsAd = %d\n", __FUNCTION__, sVoicePktcCapabilities.ugsAd));
        CcspTraceInfo(("%s: ifIndexStart = %d\n", __FUNCTION__, sVoicePktcCapabilities.ifIndexStart));
        CcspTraceInfo(("%s: supportedProvFlow = %d\n", __FUNCTION__, sVoicePktcCapabilities.supportedProvFlow));
        CcspTraceInfo(("%s: t38Version = %d\n", __FUNCTION__, sVoicePktcCapabilities.t38Version));
        CcspTraceInfo(("%s: t38ErrorCorrection = %d\n", __FUNCTION__, sVoicePktcCapabilities.t38ErrorCorrection));
        CcspTraceInfo(("%s: rfc2833 = %d\n", __FUNCTION__, sVoicePktcCapabilities.rfc2833));
        CcspTraceInfo(("%s: voiceMetrics = %d\n", __FUNCTION__, sVoicePktcCapabilities.voiceMetrics));
        CcspTraceInfo(("%s: supportedMibs = %02x %02x %02x\n", __FUNCTION__, sVoicePktcCapabilities.supportedMibs[0], sVoicePktcCapabilities.supportedMibs[1], sVoicePktcCapabilities.supportedMibs[2]));
        CcspTraceInfo(("%s: multiGrants = %d\n", __FUNCTION__, sVoicePktcCapabilities.multiGrants));
        CcspTraceInfo(("%s: v_152 = %d\n", __FUNCTION__, sVoicePktcCapabilities.v_152));
        CcspTraceInfo(("%s: certBootstrapping = %d\n", __FUNCTION__, sVoicePktcCapabilities.certBootstrapping));
        CcspTraceInfo(("%s: ipAddrProvCap = %d\n", __FUNCTION__, sVoicePktcCapabilities.ipAddrProvCap));
        // Using above data prepare the DHCP options value 43 and 125 in hex format
        char cHexBuf[512] = {0};
        dhcpOption43RawData_t dhcpOption43RawData = {0};
        getDhcpOption43RawData(&dhcpOption43RawData);

        int iLen43 = prepareDhcpOption43(&dhcpOption43RawData, cHexBuf, sizeof(cHexBuf));
        CcspTraceInfo(("%s: DHCP Option 43 length = %d\n", __FUNCTION__, iLen43));
        CcspTraceInfo(("%s: DHCP Option 43 hex = %s\n", __FUNCTION__, cHexBuf));

        char cBufOption60[256] = {0};
        int iLen60 = prepareDhcpOption60(&sVoicePktcCapabilities,cBufOption60, sizeof(cBufOption60));
        CcspTraceInfo(("%s: DHCP Option 60 length = %d\n", __FUNCTION__, iLen60));
        CcspTraceInfo(("%s: DHCP Option 60 hex = %s\n", __FUNCTION__, cBufOption60));
        if (pDhcpOption43 != NULL) {
            snprintf(pDhcpOption43, iDhcpOption43Len, "%s", cHexBuf);
        }
        if (pDhcpOption60 != NULL) {
            snprintf(pDhcpOption60, iDhcpOption60Len, "%s", cBufOption60);
        }
    }
    else
    {
        CcspTraceError(("voice_hal_get_pktc_capabilities failed '%s'\n", __FUNCTION__));
    }
}
#if 0
static bool isXf10OrXer10Model(void)
{
    char cModelNum[32] = {0};
    if (RETURN_OK != platform_hal_GetModelName(cModelNum))
    {
        CcspTraceError(("%s: platform_hal_GetModelName failed\n", __FUNCTION__));
        return false;
    }
    CcspTraceInfo(("%s: MODEL_NUM = %s\n", __FUNCTION__, cModelNum));
#if 0
    FILE *pFILE = fopen("/etc/device.properties", "r");
    if (pFILE != NULL)
    {
        char cLine[128] = {0};
        while (fgets(cLine, sizeof(cLine), pFILE) != NULL)
        {
            if (strncmp(cLine, "MODEL_NUM=", 10) == 0)
            {
                char *pModel = cLine + 10;
                pModel[strcspn(pModel, "\n")] = 0; // Remove newline character
                strcpy_s(cModelNum, sizeof(cModelNum), pModel);
                break;
            }
        }
        fclose(pFILE);
    }
#endif
    if (strcmp(cModelNum, "SCER11BEL") == 0 || strcmp(cModelNum, "SCXF11BFL") == 0)
    {
        return true;
    }
    return false;
}
#endif
static void prepareToStartUdhcpc(void)
{
    char cConfigMacVlanWithUdhcpc[8] = {0};
    syscfg_get(NULL, "ConfigMacVlanWithUdhcpc", cConfigMacVlanWithUdhcpc, sizeof(cConfigMacVlanWithUdhcpc));

    if (cConfigMacVlanWithUdhcpc[0] == '\0' || strcmp(cConfigMacVlanWithUdhcpc, "true") != 0)
    {
        CcspTraceError(("%s:%d, ConfigMacVlanWithUdhcpc is false or not set, skipping udhcpc start\n", __FUNCTION__, __LINE__));
        return;
    }
    char cDhcpOption43[512] = {0};
    char cDhcpOption60[512] = {0};
    CcspTraceInfo(("%s:%d, Reading DHCP Options from HAL\n", __FUNCTION__, __LINE__));
    readDhcpOptionsFromHal(cDhcpOption43, sizeof(cDhcpOption43), cDhcpOption60, sizeof(cDhcpOption60));

    if ((cDhcpOption43[0] != '\0') || (cDhcpOption60[0] != '\0'))
    {
        //read mtaInterfaceName from syscfg and macVlan interface mac (EMTA 78:B3:9F:8F:F2:25) from /tmp/factory_nvram.data and create the macVlan interface using ip link add command
        char cMtaInterfaceName[32] = {0};
        syscfg_get(NULL, "mtaInterfaceName", cMtaInterfaceName, sizeof(cMtaInterfaceName));
        if (cMtaInterfaceName[0] == '\0')
        {
            strcpy_s(cMtaInterfaceName, sizeof(cMtaInterfaceName), "voipIfname");
        }
        CcspTraceError(("%s:%d, MTA Interface Name is %s\n", __FUNCTION__, __LINE__, cMtaInterfaceName));
        char cMacVlanMac[32] = {0};
        readMacAddress(cMacVlanMac);
        if (strlen(cMacVlanMac) == 0)
        {
            CcspTraceError(("%s: readMacAddress failed to get MAC address\n", __FUNCTION__));
            snprintf(cMacVlanMac, sizeof(cMacVlanMac), "78:B3:9F:8F:F2:25");
        }
        CcspTraceError(("%s:%d, MTA MacVlan Mac is %s\n", __FUNCTION__, __LINE__, cMacVlanMac));
        if (cMacVlanMac[0] != '\0')
        {
            char cWanIfname[32] = {0};
            syscfg_get(NULL, "wan_physical_ifname", cWanIfname, sizeof(cWanIfname));
            CcspTraceInfo(("%s:%d, WAN Physical Ifname is %s\n", __FUNCTION__, __LINE__, cWanIfname));
            if (cWanIfname[0] == '\0')
            {
                strcpy_s(cWanIfname, sizeof(cWanIfname), "erouter0");
            }
            //Create the macVlan
            CcspTraceInfo(("%s:%d, Creating macVlan interface %s with mac %s\n", __FUNCTION__, __LINE__, cMtaInterfaceName, cMacVlanMac));
            char cCmd[2048] = {0};
            snprintf(cCmd, sizeof(cCmd), "ip link add link %s name %s type macvlan mode bridge", cWanIfname, cMtaInterfaceName);
            system(cCmd);
            snprintf(cCmd, sizeof(cCmd), "ip link set dev %s address %s", cMtaInterfaceName, cMacVlanMac);
            system(cCmd);
            snprintf(cCmd, sizeof(cCmd), "ip link set dev %s up", cMtaInterfaceName);
            system(cCmd);
            //Now run the udhcpc with dhcp option 43 and 125 like below
            //udhcpc -O 23 -O 125 -i mtaIface -p /tmp/udhcpc.mtaIface.pid -x 0x7D:00000DE90101020201020301000401000901010B0306090F0C01010D01010F010110010912020004130101140101150101160101170302003F1801001901001A0100260101 -s /var/tmp/service_udhcpc
            snprintf(cCmd, sizeof(cCmd), "udhcpc -O 2 -O 122 -O 4 -O 7 -O 43 -O 54 -O 99 -O 123 -O 125 -O timezone -V eRouter1.0 -x %s -i %s -p /tmp/udhcpc.%s.pid -V %s -s /var/tmp/service_udhcpc &", cDhcpOption43, cMtaInterfaceName, cMtaInterfaceName, cDhcpOption60);
            system(cCmd);
            CcspTraceInfo(("%s:%d, Started udhcpc for MTA on interface %s with cmd: %s\n", __FUNCTION__, __LINE__, cMtaInterfaceName, cCmd));
        }
        else
        {
            CcspTraceWarning(("%s:%d, mtaMacAddress not found in /tmp/factory_nvram.data\n", __FUNCTION__, __LINE__));
        }
    }
}
#endif
ANSC_STATUS
CosaMTAInitializeEthWanProvDhcpOption
    (
        ANSC_HANDLE                 hThisObject
    )
{
	 MTA_IP_TYPE_TR ip_type;
	 char 	buffer [ MAX_BUFF_SIZE ] = { 0 };
	 int	MtaIPMode = 0,IP_Pref_Mode_Received=0;
	 int i = 0, j =0; int len = 0;
         errno_t rc = -1;
         unsigned char x,y;
	 PMTAMGMT_MTA_PROVISIONING_PARAMS pMtaProv = NULL;
	 pMtaProv = (PMTAMGMT_MTA_PROVISIONING_PARAMS)malloc(sizeof(MTAMGMT_PROVISIONING_PARAMS));
         
        if(pMtaProv == NULL)
        {
		printf("Memory Alloction Failed '%s'\n", __FUNCTION__);
		CcspTraceError(("Memory Alloction Failed '%s'\n", __FUNCTION__));
		return ANSC_STATUS_FAILURE;
	} 

	 PCOSA_DATAMODEL_MTA      pMyObject    = (PCOSA_DATAMODEL_MTA)hThisObject;
         /*Coverity Fix CID 120992  NULL Check */
	 if((pMyObject->pmtaprovinfo = (PCOSA_MTA_ETHWAN_PROV_INFO)AnscAllocateMemory(sizeof(COSA_MTA_ETHWAN_PROV_INFO))) == NULL)
         {
             CcspTraceWarning(("%s:pMyObject->pmtaprovinfo attained NULL\n",__FUNCTION__));
              free(pMtaProv);
             return ANSC_STATUS_FAILURE;
         }    

	 char Ip_Pref [MAX_IP_PREF_VAL] = { 0 }, Ipv4_Primary[MAX_IPV4_HEX_VAL] = {0}, Ipv4_Secondary[MAX_IPV4_HEX_VAL] = {0}, Ipv6_Primary[MAX_IPV6_HEX_VAL] = {0} , Ipv6_Secondary[MAX_IPV6_HEX_VAL] = {0};

         CosaMTAInitializeEthWanProvJournal(pMyObject->pmtaprovinfo);

	sysevent_get(sysevent_fd, sysevent_token, "MTA_IP_PREF", Ip_Pref, sizeof(Ip_Pref));
	sysevent_get(sysevent_fd, sysevent_token, "MTA_DHCPv4_PrimaryAddress", Ipv4_Primary, sizeof(Ipv4_Primary));
	sysevent_get(sysevent_fd, sysevent_token, "MTA_DHCPv4_SecondaryAddress", Ipv4_Secondary, sizeof(Ipv4_Secondary));
	sysevent_get(sysevent_fd, sysevent_token, "MTA_DHCPv6_PrimaryAddress", Ipv6_Primary, sizeof(Ipv6_Primary));
	sysevent_get(sysevent_fd, sysevent_token, "MTA_DHCPv6_SecondaryAddress", Ipv6_Secondary, sizeof(Ipv6_Secondary));

    	if ( Ip_Pref[MAX_IP_PREF_VAL-1] != '\0' )
        	Ip_Pref[MAX_IP_PREF_VAL-1] = '\0' ;

    	if ( Ipv4_Primary[MAX_IPV4_HEX_VAL-1] != '\0' )
        	Ipv4_Primary[MAX_IPV4_HEX_VAL-1] = '\0' ;

    	if ( Ipv4_Secondary[MAX_IPV4_HEX_VAL-1] != '\0' )
        	Ipv4_Secondary[MAX_IPV4_HEX_VAL-1] = '\0' ;

    	if ( Ipv6_Primary[MAX_IPV6_HEX_VAL-1] != '\0' )
        	Ipv6_Primary[MAX_IPV6_HEX_VAL-1] = '\0' ;

    	if ( Ipv6_Secondary[MAX_IPV6_HEX_VAL-1] != '\0' )
        	Ipv6_Secondary[MAX_IPV6_HEX_VAL-1] = '\0' ;

    	CcspTraceInfo(("%s MTA values returned from dhcp server are \n",__FUNCTION__));

    	CcspTraceInfo(("%s MTA_IP_PREF = %s \n",__FUNCTION__,Ip_Pref));
    	CcspTraceInfo(("%s MTA_DHCPv4_PrimaryAddress = %s,MTA_DHCPv4_SecondaryAddress = %s  \n",__FUNCTION__,Ipv4_Primary,Ipv4_Secondary));
    	CcspTraceInfo(("%s MTA_DHCPv6_PrimaryAddress = %s,MTA_DHCPv6_SecondaryAddress = %s  \n",__FUNCTION__,Ipv6_Primary,Ipv6_Secondary));

	if ( ( 0 == strncmp(Ipv4_Primary,"00000000",8) ) || ( 0 == strncmp(Ipv6_Primary,"00000000",8) ) )
	{
	    CcspTraceWarning(("%s Received 0's from dhcp sever ,not initializing MTA \n",__FUNCTION__));
	    /* CID 120995 Resource leak */	
	    free(pMtaProv);
	    return ANSC_STATUS_FAILURE;
	} 
      
	if(Ip_Pref[0] != '\0') {
			sscanf( Ip_Pref, "%d", &IP_Pref_Mode_Received );
			if ( IP_Pref_Mode_Received == 01 )
				MtaIPMode = MTA_IPV4;
			else if ( IP_Pref_Mode_Received == 02 )
				MtaIPMode = MTA_IPV6;
			else if ( ( IP_Pref_Mode_Received == 05 ) || ( IP_Pref_Mode_Received == 06 ) )
				MtaIPMode = MTA_DUAL_STACK;
			else
			{
				CcspTraceWarning(("%s Value received from server is invalid , using default value \n",__FUNCTION__));

				if( 0 == syscfg_get( NULL, "StartupIPMode", buffer, sizeof(buffer)))
				{
				   if(buffer[0] != '\0')
				   {
						sscanf( buffer, "%d", &MtaIPMode );
	     				CcspTraceInfo(("%s Default MtaIPMode is %d \n",__FUNCTION__,MtaIPMode));
				   }
				}
			}

			CcspTraceInfo(("MtaIPMode = %d\n",MtaIPMode));
		}
		else
		{
			// deduce MtaIPMode value from dhcp server  if one of the ip is received , if both are received use default, if not received then use default
			if ( ( (Ipv4_Primary[0] != '\0')  && (Ipv6_Primary[0] != '\0') ) || ( (Ipv4_Primary[0] == '\0')  && (Ipv6_Primary[0] == '\0') ) )
			{

				if( 0 == syscfg_get( NULL, "StartupIPMode", buffer, sizeof(buffer)))
				{
				   if(buffer[0] != '\0')
				   {
						sscanf( buffer, "%d", &MtaIPMode );
	     				CcspTraceInfo(("%s Default MtaIPMode is %d \n",__FUNCTION__,MtaIPMode));
				   }
				}
		       }

			else if ( Ipv4_Primary[0] != '\0')
			{
	     		CcspTraceInfo(("%s Received only Ipv4_Primary from dhcp server , setting IP Preference mode to ipv4 \n",__FUNCTION__));
				MtaIPMode=MTA_IPV4;

			}
			else if ( Ipv6_Primary[0] != '\0')
			{
	     		CcspTraceInfo(("%s Received only Ipv6_Primary from dhcp server , setting IP Preference mode to ipv6  \n",__FUNCTION__));
				MtaIPMode=MTA_IPV6;
			}

		}

		pMtaProv->MtaIPMode = MtaIPMode;
		CosaDmlMTASetStartUpIpMode(pMyObject->pmtaprovinfo,MtaIPMode);
		ip_type = MTA_IPV4_TR;

		rc = memset_s(pMtaProv->DhcpOption122Suboption1, sizeof(pMtaProv->DhcpOption122Suboption1), 0, sizeof(pMtaProv->DhcpOption122Suboption1));
                ERR_CHK(rc);
		if (Ipv4_Primary[0] != '\0' )
			{
                                char* pIpv4Primary = Ipv4_Primary;
                                rc = strcpy_s(buffer, sizeof(buffer), pIpv4Primary);
                                if(rc != EOK)
                                {
                                    ERR_CHK(rc);
                                    /* Coverity Fix CID : 120995 RESOURCE_LEAK */
                                    if (pMtaProv)
                                    {
                                        free(pMtaProv);
                                    }
                                    return ANSC_STATUS_FAILURE;
                                }
			}
			else 
			{
				syscfg_get( NULL, "IPv4PrimaryDhcpServerOptions", buffer, sizeof(buffer));
			}	
				
				   if(buffer[0] != '\0')
				   {
					len = strlen(buffer);
					CosaDmlMTASetPrimaryDhcpServerOptions(pMyObject->pmtaprovinfo, buffer, ip_type);
					if((MtaIPMode ==  MTA_IPV4) || (MtaIPMode == MTA_DUAL_STACK))
				        {       
                                                /* Coverity Fix CID:121023 Incompatible type */
						if(ConverStr2Hex((unsigned char *)buffer) == ANSC_STATUS_SUCCESS)
                                                {
                                                        for(i = 0,j= 0;i<len; i++,j++)
							{
								if(j<MTA_DHCPOPTION122SUBOPTION1_MAX)
								{
                                                                      x = buffer[i]<<4;
                                                                      y = buffer[++i];
									pMtaProv->DhcpOption122Suboption1[j] |= x + y;
								}
								else
									break;
							}
	
							printf("pMtaProv->DhcpOption122Suboption1[0] = %X %d\n",pMtaProv->DhcpOption122Suboption1[0],pMtaProv->DhcpOption122Suboption1[0]);
							printf("pMtaProv->DhcpOption122Suboption1[1] = %X %d\n",pMtaProv->DhcpOption122Suboption1[1],pMtaProv->DhcpOption122Suboption1[1]);
							printf("pMtaProv->DhcpOption122Suboption1[2] = %X %d\n",pMtaProv->DhcpOption122Suboption1[2],pMtaProv->DhcpOption122Suboption1[2]);
							printf("pMtaProv->DhcpOption122Suboption1[3] = %X %d\n",pMtaProv->DhcpOption122Suboption1[3],pMtaProv->DhcpOption122Suboption1[3]);
						}
					}
				   }


			rc = memset_s(pMtaProv->DhcpOption122Suboption2, sizeof(pMtaProv->DhcpOption122Suboption2), 0, sizeof(pMtaProv->DhcpOption122Suboption2));
                        ERR_CHK(rc);
			if (Ipv4_Secondary[0] != '\0' )
			{
                               char* pIpv4Secondary = Ipv4_Secondary;
                               rc = strcpy_s(buffer, sizeof(buffer), pIpv4Secondary);
                               if(rc != EOK)
                               {
                                   ERR_CHK(rc);
                                   /* Coverity Fix CID : 120995 RESOURCE_LEAK */
                                   if (pMtaProv)
                                   {
                                       free(pMtaProv);
                                   }
                                   return ANSC_STATUS_FAILURE;
                               }
                        /* Coverity  fix CID : 340667 Unused Value */
                        //x=0;
                        //y=0;
			}
			else
			{
				syscfg_get( NULL, "IPv4SecondaryDhcpServerOptions", buffer, sizeof(buffer));
			}
				   
				   if(buffer[0] != '\0')
				   {
					len = strlen(buffer);
					CosaDmlMTASetSecondaryDhcpServerOptions(pMyObject->pmtaprovinfo, buffer, ip_type);
					if((MtaIPMode ==  MTA_IPV4) || (MtaIPMode == MTA_DUAL_STACK))
					{       /* Coverity Fix CID:121023 Incompatible type */
						if(ConverStr2Hex((unsigned char *)buffer) == ANSC_STATUS_SUCCESS)
						{
					   		for(i = 0,j= 0;i<len; i++,j++)
							{
								if(j<MTA_DHCPOPTION122SUBOPTION2_MAX)
								{
                                                                        x= buffer[i]<<4;
                                                                        y = buffer[++i];
									pMtaProv->DhcpOption122Suboption2[j] |= x + y;
								}
								else
									break;
							}
							printf("pMtaProv->DhcpOption122Suboption2[0] = %X %d\n",pMtaProv->DhcpOption122Suboption2[0],pMtaProv->DhcpOption122Suboption2[0]);
							printf("pMtaProv->DhcpOption122Suboption2[1] = %X %d\n",pMtaProv->DhcpOption122Suboption2[1],pMtaProv->DhcpOption122Suboption2[1]);
							printf("pMtaProv->DhcpOption122Suboption2[2] = %X %d\n",pMtaProv->DhcpOption122Suboption2[2],pMtaProv->DhcpOption122Suboption2[2]);
							printf("pMtaProv->DhcpOption122Suboption2[3] = %X %d\n",pMtaProv->DhcpOption122Suboption2[3],pMtaProv->DhcpOption122Suboption2[3]);
						}
					}

				   }
	    ip_type = MTA_IPV6_TR;

				rc = memset_s(pMtaProv->DhcpOption2171CccV6DssID1, sizeof(pMtaProv->DhcpOption2171CccV6DssID1), 0, sizeof(pMtaProv->DhcpOption2171CccV6DssID1));
                                ERR_CHK(rc);
            pMtaProv->DhcpOption2171CccV6DssID1Len = 0;
			if ( Ipv6_Primary[0] != '\0' ) 
			{
                                char* pIpv6Primary = Ipv6_Primary;
                                rc = strcpy_s(buffer, sizeof(buffer), pIpv6Primary);
                                if(rc != EOK)
                                {
                                    ERR_CHK(rc);
                                    /* Coverity Fix CID : 120995 RESOURCE_LEAK */
                                    if (pMtaProv)
                                    {
                                       free(pMtaProv);
                                    }
                                    return ANSC_STATUS_FAILURE;
                                }
                                /* Coverity  fix CID : 340667 Unused Value */
                                //x=0;
                                //y=0;
				memset(pMtaProv->DhcpOption2171CccV6DssID1,0,MTA_DHCPOPTION122CCCV6DSSID1_MAX);
			}

			else
			{
				syscfg_get( NULL, "IPv6PrimaryDhcpServerOptions", buffer, sizeof(buffer));
			}
				
				   if(buffer[0] != '\0')
				   {
					len = strlen(buffer);
					CosaDmlMTASetPrimaryDhcpServerOptions(pMyObject->pmtaprovinfo, buffer, ip_type);
					if((MtaIPMode == MTA_IPV6) || (MtaIPMode == MTA_DUAL_STACK))
					{	/* Coverity Fix CID:121023 Incompatible type */
						if(ConverStr2Hex((unsigned char *)buffer) == ANSC_STATUS_SUCCESS)
						{
							printf("Buffer is %s\n",buffer);
								for(i = 0,j= 0;i<len; i++,j++)
										{
								if(j<MTA_DHCPOPTION122CCCV6DSSID1_MAX)
							        {       /*Coverity Fix CID:120994 EVALUTION_ORDER */
                                                                        x = buffer[i]<<4;
                                                                         y = buffer[++i];
									pMtaProv->DhcpOption2171CccV6DssID1[j] |= x + y;
								}
								else
									break;
							}
	
							printf("pMtaProv->DhcpOption2171CccV6DssID1[0] = %X %d\n",pMtaProv->DhcpOption2171CccV6DssID1[0],pMtaProv->DhcpOption2171CccV6DssID1[0]);
							printf("pMtaProv->DhcpOption2171CccV6DssID1[1] = %X %d\n",pMtaProv->DhcpOption2171CccV6DssID1[1],pMtaProv->DhcpOption2171CccV6DssID1[1]);
							printf("pMtaProv->DhcpOption2171CccV6DssID1[2] = %X %d\n",pMtaProv->DhcpOption2171CccV6DssID1[2],pMtaProv->DhcpOption2171CccV6DssID1[2]);
							printf("pMtaProv->DhcpOption2171CccV6DssID1[3] = %X %d\n",pMtaProv->DhcpOption2171CccV6DssID1[3],pMtaProv->DhcpOption2171CccV6DssID1[3]);
							pMtaProv->DhcpOption2171CccV6DssID1Len = j;
							printf("pMtaProv->DhcpOption2171CccV6DssID1Len = %d\n",pMtaProv->DhcpOption2171CccV6DssID1Len);
						}
					}

				   }

				rc = memset_s(pMtaProv->DhcpOption2171CccV6DssID2, sizeof(pMtaProv->DhcpOption2171CccV6DssID2), 0, sizeof(pMtaProv->DhcpOption2171CccV6DssID2));
                                ERR_CHK(rc);
            pMtaProv->DhcpOption2171CccV6DssID2Len = 0;
			if ( Ipv6_Secondary[0] != '\0' )   
			{
                                char* pIpv6Secondary = Ipv6_Secondary;
                                rc = strcpy_s(buffer, sizeof(buffer), pIpv6Secondary);
                                if(rc != EOK)
                                {
                                    ERR_CHK(rc);
                                    /* Coverity Fix CID : 120995 RESOURCE_LEAK */
                                    if (pMtaProv)
                                    {
                                       free(pMtaProv);
                                    }
                                    return ANSC_STATUS_FAILURE;
                                }
                                /* Coverity  fix CID : 340667 Unused Value */
                                //x =0;
                                //y =0;
			}
			else
			{
				syscfg_get( NULL, "IPv6SecondaryDhcpServerOptions", buffer, sizeof(buffer));
			}   
				   if(buffer[0] != '\0')
				   {
					len = strlen(buffer);
					CosaDmlMTASetSecondaryDhcpServerOptions(pMyObject->pmtaprovinfo, buffer, ip_type);
					if((MtaIPMode == MTA_IPV6) || (MtaIPMode == MTA_DUAL_STACK))
	                                {	        /* Coverity Fix CID:121023 Incompatible type */
						if(ConverStr2Hex((unsigned char *)buffer) == ANSC_STATUS_SUCCESS)
						{
							printf("Buffer is %s\n",buffer);
					   		for(i = 0,j= 0;i<len; i++,j++)
							{
								if(j<MTA_DHCPOPTION122CCCV6DSSID2_MAX)
								{
                                                                       x =buffer[i]<<4;
                                                                        y = buffer[++i];
									pMtaProv->DhcpOption2171CccV6DssID2[j] |= x + y;
							}
								else
									break;
							}
							printf("pMtaProv->DhcpOption2171CccV6DssID2[0] = %X %d\n",pMtaProv->DhcpOption2171CccV6DssID2[0],pMtaProv->DhcpOption2171CccV6DssID2[0]);
							printf("pMtaProv->DhcpOption2171CccV6DssID2[1] = %X %d\n",pMtaProv->DhcpOption2171CccV6DssID2[1],pMtaProv->DhcpOption2171CccV6DssID2[1]);
							printf("pMtaProv->DhcpOption2171CccV6DssID2[2] = %X %d\n",pMtaProv->DhcpOption2171CccV6DssID2[2],pMtaProv->DhcpOption2171CccV6DssID2[2]);
							printf("pMtaProv->DhcpOption2171CccV6DssID2[3] = %X %d\n",pMtaProv->DhcpOption2171CccV6DssID2[3],pMtaProv->DhcpOption2171CccV6DssID2[3]);
							pMtaProv->DhcpOption2171CccV6DssID2Len = j;
							printf("pMtaProv->DhcpOption2171CccV6DssID2Len = %d\n",pMtaProv->DhcpOption2171CccV6DssID2Len);
						}
					}

				   }
    #if defined (ENABLE_ETH_WAN)
		// call hal to start provisioning
		if(mta_hal_start_provisioning(pMtaProv) == RETURN_OK)
		{
			CcspTraceError(("mta_hal_start_provisioning succeded '%s'\n", __FUNCTION__));
            if (syscfg_set(NULL, "MTA_PROVISION","true") != 0)
            {
                CcspTraceWarning(("%s: syscfg_set failed\n", __FUNCTION__));
            }
            else
            {
                CcspTraceInfo(("%s: syscfg MTA_PROVISION successfully set to true\n", __FUNCTION__));
            }
                        /*Coverity Fix  CID:120996 RESOURCE_LEAK */
                        free(pMtaProv);
			return ANSC_STATUS_SUCCESS;
		}
		else
		{
			CcspTraceError(("mta_hal_start_provisioning Failed '%s'\n", __FUNCTION__));
                        /*Coverity Fix  CID:120995 RESOURCE_LEAK */
                        free(pMtaProv);
			return ANSC_STATUS_FAILURE;
		}
     #endif
     /* CID 121017 Structurally dead code fix */
     return ANSC_STATUS_SUCCESS;
}

#define MTA_DHCP_ENABLED 0
#define MTA_DHCP_DISABLED 1
//MAX_TIMEOUT_MTA_DHCP_DISABLED is 1 and MAX_TIMEOUT_MTA_DHCP_ENABLED is 2 when unitTestDockerSupport is enabled
//MAX_TIMEOUT_MTA_DHCP_DISABLED is 300 and MAX_TIMEOUT_MTA_DHCP_ENABLED is 60 when unitTestDockerSupport is not enabled
int checkIfDefMtaDhcpOptionEnabled()
{
	char ipv4Primary[16] , ipv6Primary[16],ip4Sec[16] , ipv6Sec[16] ; 
	memset(ipv4Primary,0,sizeof(ipv4Primary));
	memset(ip4Sec,0,sizeof(ip4Sec));
	memset(ipv6Primary,0,sizeof(ipv6Primary));
	memset(ipv6Sec,0,sizeof(ipv6Sec));

    	syscfg_get( NULL, "IPv4PrimaryDhcpServerOptions", ipv4Primary, sizeof(ipv4Primary));
    	syscfg_get( NULL, "IPv4SecondaryDhcpServerOptions", ip4Sec, sizeof(ip4Sec));
    	syscfg_get( NULL, "IPv6PrimaryDhcpServerOptions", ipv6Primary, sizeof(ipv6Primary));
    	syscfg_get( NULL, "IPv6SecondaryDhcpServerOptions", ipv6Sec, sizeof(ipv6Sec));

    	if ( (strcmp(ipv4Primary,"FFFFFFFF") == 0) || (strcmp(ipv4Primary,"0A0A0A0A") == 0) )
    		return MTA_DHCP_ENABLED;
    	else if ( (strcmp(ip4Sec,"FFFFFFFF") == 0) || (strcmp(ip4Sec,"0A0A0A0A") == 0) )
    		return MTA_DHCP_ENABLED;
    	else if ( (strcmp(ipv6Primary,"FFFFFFFF") == 0) || (strcmp(ipv6Primary,"0A0A0A0A") == 0) )
    		return MTA_DHCP_ENABLED;
    	else if ( (strcmp(ipv6Sec,"FFFFFFFF") == 0) || (strcmp(ipv6Sec,"0A0A0A0A") == 0) )
    		return MTA_DHCP_ENABLED;
    	else
		return MTA_DHCP_DISABLED ;

	return MTA_DHCP_DISABLED ;
}
int getMaxCount()
{
	int maxCount=MAX_TIMEOUT_MTA_DHCP_DISABLED;
	if ( MTA_DHCP_ENABLED == checkIfDefMtaDhcpOptionEnabled() )
	{
		CcspTraceInfo(("%s dhcp_option's enabled in partner defaults,wait for 60sec to receive dhcp options  \n",__FUNCTION__));
		maxCount=MAX_TIMEOUT_MTA_DHCP_ENABLED ;
	}
	return maxCount;
}

void WaitForDhcpOption()
{
	char dhcp_option[10] = {0};
 	int count=0;
 	errno_t rc = -1;
 	int ind = -1;

	int maxCount=getMaxCount() ;
	//  wait for maxCount sec to receive MTA options/Offer , otherwise time out 
 	while ( maxCount >= count )
	{
		rc = memset_s(dhcp_option, sizeof(dhcp_option), 0, sizeof(dhcp_option));
    		ERR_CHK(rc);
		sysevent_get(sysevent_fd, sysevent_token, "dhcp_mta_option", dhcp_option, sizeof(dhcp_option));
    		rc  = strcmp_s((const char*)dhcp_option, sizeof(dhcp_option), "received", &ind);
    		ERR_CHK(rc);
		if ((ind == 0) && (rc == EOK))
		{
			CcspTraceInfo(("%s dhcp_option's received,breaking the loop  \n",__FUNCTION__));
			break;
		}
		sleep(5);
		count+=5;
 	}
 	CcspTraceInfo(("%s Didn't receive dhcp options in %d sec, initializing mta with default values \n",__FUNCTION__,maxCount));
}

/*Coverity Fix CID 121026 Arg Type MisMatch */
void * Mta_Sysevent_thread_Dhcp_Option( void * hThisObject)
{

 PCOSA_DATAMODEL_MTA      pMyObject    = (PCOSA_DATAMODEL_MTA)hThisObject;

 char current_wan_state[10] = {0}, dhcp_option[10] = {0};

 int err;
 char name[25]={0}, val[10]={0};
 errno_t rc = -1;
 int ind = -1;
 async_id_t getnotification_asyncid;
 async_id_t wan_state_asyncid;

 int retValue=0,  namelen=0, vallen =0 ;
 sysevent_set_options(sysevent_fd, sysevent_token, "current_wan_state", TUPLE_FLAG_EVENT);
 retValue=sysevent_setnotification(sysevent_fd, sysevent_token, "current_wan_state",  &wan_state_asyncid);
 CcspTraceWarning(("%s Return value is   %d\n",__FUNCTION__,retValue));

 sysevent_get(sysevent_fd, sysevent_token, "current_wan_state", current_wan_state, sizeof(current_wan_state));
 sysevent_get(sysevent_fd, sysevent_token, "dhcp_mta_option", dhcp_option, sizeof(dhcp_option));

 char value[16] = {'\0'};
 bool tr104ApplySuccess = false;

 char ethWanSyscfg[64]={'\0'};
 bool mtaInEthernetMode = false;
 if( (0 == syscfg_get( NULL, "eth_wan_enabled", ethWanSyscfg, sizeof(ethWanSyscfg))) && ((ethWanSyscfg[0] != '\0') && (strncmp(ethWanSyscfg, "true", strlen("true")) == 0)))
 {
    mtaInEthernetMode = true;
 }

 CcspTraceError(("%s:%d MTA started in %s mode. \n",__FUNCTION__,__LINE__, mtaInEthernetMode?"Ethernet":"DOCSIS"));

 rc = strcmp_s((const char*)current_wan_state, sizeof(current_wan_state), "up", &ind);
 ERR_CHK(rc);
 if((rc == EOK) && (ind == 0))
 {
     if(mtaInEthernetMode)
     {
         rc = strcmp_s((const char*)dhcp_option, sizeof(dhcp_option), "received", &ind);
         ERR_CHK(rc);
         if((rc == EOK) && (ind == 0))
         {
             CcspTraceWarning(("%s current_wan_state up, Initializing MTA \n",__FUNCTION__));
             CosaMTAInitializeEthWanProvDhcpOption(pMyObject);
         }
         else if((rc == EOK) && (ind != 0))
         {
             CcspTraceWarning(("%s current_wan_state up, but dhcp_option's not received.  \n",__FUNCTION__));
             WaitForDhcpOption();
             CosaMTAInitializeEthWanProvDhcpOption(pMyObject);
         }
     }
 }

  do
  {

    namelen = sizeof(name);
    vallen  = sizeof(val);

    rc = memset_s(name, sizeof(name), 0, sizeof(name));
    ERR_CHK(rc);
    rc = memset_s(val, sizeof(val), 0, sizeof(val));
    ERR_CHK(rc);

    err = sysevent_getnotification(sysevent_fd, sysevent_token, name, &namelen, val, &vallen, &getnotification_asyncid);

    if (!err)
    {
        CcspTraceWarning(("%s Recieved notification event  %s, state %s\n",__FUNCTION__,name,val));
        rc = strcmp_s((const char*)name, sizeof(name), "current_wan_state", &ind);
        ERR_CHK(rc);
        if((rc == EOK) && (ind == 0))
        {
            rc = strcmp_s((const char*)val, sizeof(val), "up", &ind);
            ERR_CHK(rc);
            if((rc == EOK) && (ind == 0))
            {
                bool isEthEnabled = false;
                bool tr104Enable = false;
                if( (0 == syscfg_get( NULL, "eth_wan_enabled", ethWanSyscfg, sizeof(ethWanSyscfg))) && ((ethWanSyscfg[0] != '\0') && (strncmp(ethWanSyscfg, "true", strlen("true")) == 0)))
                {
                    isEthEnabled = true;
                }
              
                if((syscfg_get(NULL,"TR104enable", value, sizeof(value)) == 0) && (strcmp(value, "true") == 0))
                {  
    		    tr104Enable = true;
 		}
              
  		CcspTraceWarning((" %s:: tr104Enable: %s \n",__FUNCTION__, value ));
   		memset_s(value,sizeof(value),0,sizeof(value));
        
                if(sysevent_get(sysevent_fd, sysevent_token, "tr104_applied", value, sizeof(value)) == 0)
                {
   		    tr104ApplySuccess = (strcmp(value, "true") == 0);
   		    CcspTraceWarning((" %s:: tr104_applied: %s tr104ApplySuccess=%d \n",__FUNCTION__, value ,tr104ApplySuccess ));
 		} 
                else
                {
   		    CcspTraceError(("%s:%d tr104 not applied :value =%s \n",__FUNCTION__,__LINE__,value));
   		    tr104ApplySuccess = false;
                }
                
                CcspTraceWarning((" %s:: mtaInEthernetMode: %s tr104Enable: %s tr104ApplySuccess: %s \n",__FUNCTION__, mtaInEthernetMode ?"true":"false" ,tr104Enable ?"true":"false" ,tr104ApplySuccess ?"true":"false" ));

                if(mtaInEthernetMode != isEthEnabled)
                {
                    CcspTraceError(("%s:%d MTA is in incorrect WAN state. MTA started in %s, but selected WAN mode %s . MTA agent will be restarted.\n",__FUNCTION__,__LINE__, mtaInEthernetMode?"Ethernet":"DOCSIS", isEthEnabled?"Ethernet":"DOCSIS"));
                    exit(0);
                }
                else if( (mtaInEthernetMode) && (tr104Enable) && (tr104ApplySuccess))
                {
                    CcspTraceWarning(("%s TR104 is Enabled in EthWan Mode, skipping Default config apply. \n",__FUNCTION__));
                }
                else if( ((mtaInEthernetMode) && (!tr104Enable)) ||  ((mtaInEthernetMode) && (tr104Enable) && (!tr104ApplySuccess)) )
                {
                    WaitForDhcpOption();
                    CosaMTAInitializeEthWanProvDhcpOption(pMyObject);
                }
            }
        }
    }
  }while(FOREVER); //FOREVER is 0 when unitTestDockerSupport is enabled, else FOREVER is 1.
}

ANSC_STATUS CosaMTALineTableInitialize
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCOSA_DATAMODEL_MTA      pMyObject    = (PCOSA_DATAMODEL_MTA)hThisObject;
    PCOSA_MTA_LINETABLE_INFO pLineTable    = (PCOSA_MTA_LINETABLE_INFO)pMyObject->pLineTable;
    ULONG ulCount = CosaDmlMTALineTableGetNumberOfEntries(NULL);

    CcspTraceWarning(("%s %lu\n",__FUNCTION__, ulCount));
    pMyObject->LineTableCount = 0;
    
    if ( ulCount != 0 )
    {
        pLineTable = AnscAllocateMemory(ulCount * sizeof(COSA_MTA_LINETABLE_INFO));
        
        if(pLineTable != NULL)
        {
            ULONG ul=0;
            for (ul=0; ul<ulCount; ul++)
            {
                pLineTable[ul].InstanceNumber = ul + 1;
            }
            pMyObject->pLineTable = pLineTable;
            pMyObject->LineTableCount = ulCount;
        } 
    }
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaMTAInitializeEthWanProv
    (
        ANSC_HANDLE                 hThisObject
    )
{

 MTA_IP_TYPE_TR ip_type;
 char 	buffer [ 128 ] = { 0 };
 int	MtaIPMode = 0;
 int i = 0, j =0; int len = 0;
 errno_t rc = -1;
 unsigned char x,y;
 PMTAMGMT_MTA_PROVISIONING_PARAMS pMtaProv = NULL;
 pMtaProv = (PMTAMGMT_MTA_PROVISIONING_PARAMS)malloc(sizeof(MTAMGMT_PROVISIONING_PARAMS));

 PCOSA_DATAMODEL_MTA      pMyObject    = (PCOSA_DATAMODEL_MTA)hThisObject;
 pMyObject->pmtaprovinfo = (PCOSA_MTA_ETHWAN_PROV_INFO)AnscAllocateMemory(sizeof(COSA_MTA_ETHWAN_PROV_INFO));

/* Coverity Fix : NULL_RETURNS */
if(pMtaProv && pMyObject->pmtaprovinfo)
{
    CosaMTAInitializeEthWanProvJournal(pMyObject->pmtaprovinfo);
	/* CID  173808  Uninitialized scalar variable */
	pMtaProv->MtaIPMode = MTA_IPV4;

	if( 0 == syscfg_get( NULL, "StartupIPMode", buffer, sizeof(buffer)))
	{
	   if(buffer[0] != '\0')
	   {
		sscanf( buffer, "%d", &MtaIPMode );
		printf("MtaIPMode = %d\n",MtaIPMode);
	   }
	}
	/* CID 173808: Uninitialized scalar variable*/
	pMtaProv->MtaIPMode = MtaIPMode;
        CosaDmlMTASetStartUpIpMode(pMyObject->pmtaprovinfo,MtaIPMode);
        ip_type = MTA_IPV4_TR;
		rc = memset_s(buffer, sizeof(buffer), 0, sizeof(buffer));
                ERR_CHK(rc);
		rc = memset_s(pMtaProv->DhcpOption122Suboption1, sizeof(pMtaProv->DhcpOption122Suboption1), 0, sizeof(pMtaProv->DhcpOption122Suboption1));
                ERR_CHK(rc);
		if( 0 == syscfg_get( NULL, "IPv4PrimaryDhcpServerOptions", buffer, sizeof(buffer)))
		{
		   if(buffer[0] != '\0')
		   {
			len = strlen(buffer);
			CosaDmlMTASetPrimaryDhcpServerOptions(pMyObject->pmtaprovinfo, buffer, ip_type);
			if((MtaIPMode ==  MTA_IPV4) || (MtaIPMode == MTA_DUAL_STACK))
			{
				if(ConverStr2Hex((unsigned char*)buffer) == ANSC_STATUS_SUCCESS)
				{

					for(i = 0,j= 0;i<len; i++,j++)
					{
						if(j<MTA_DHCPOPTION122SUBOPTION1_MAX)
						{
                                                        /*Coverity Fix CID:63437 EVALUTION_ORDER */
                                                        x = buffer[i]<<4;
                                                        y = buffer[++i];
							pMtaProv->DhcpOption122Suboption1[j] |= x + y;
						}
						else
							break;
					}
	
					printf("pMtaProv->DhcpOption122Suboption1[0] = %X %d\n",pMtaProv->DhcpOption122Suboption1[0],pMtaProv->DhcpOption122Suboption1[0]);
					printf("pMtaProv->DhcpOption122Suboption1[1] = %X %d\n",pMtaProv->DhcpOption122Suboption1[1],pMtaProv->DhcpOption122Suboption1[1]);
					printf("pMtaProv->DhcpOption122Suboption1[2] = %X %d\n",pMtaProv->DhcpOption122Suboption1[2],pMtaProv->DhcpOption122Suboption1[2]);
					printf("pMtaProv->DhcpOption122Suboption1[3] = %X %d\n",pMtaProv->DhcpOption122Suboption1[3],pMtaProv->DhcpOption122Suboption1[3]);
				}
			}

		   }
		}
		rc = memset_s(buffer,sizeof(buffer), 0, sizeof(buffer));
                ERR_CHK(rc);
		rc = memset_s(pMtaProv->DhcpOption122Suboption2, sizeof(pMtaProv->DhcpOption122Suboption2), 0, sizeof(pMtaProv->DhcpOption122Suboption2));
                ERR_CHK(rc);
		if( 0 == syscfg_get( NULL, "IPv4SecondaryDhcpServerOptions", buffer, sizeof(buffer)))
		{
		   
		   if(buffer[0] != '\0')
		   {
			len = strlen(buffer);
			CosaDmlMTASetSecondaryDhcpServerOptions(pMyObject->pmtaprovinfo, buffer, ip_type);
			if((MtaIPMode ==  MTA_IPV4) || (MtaIPMode == MTA_DUAL_STACK))
			{
				if(ConverStr2Hex((unsigned char*)buffer) == ANSC_STATUS_SUCCESS)
				{
			   		for(i = 0,j= 0;i<len; i++,j++)
					{
						if(j<MTA_DHCPOPTION122SUBOPTION2_MAX)
						{
							unsigned char tmp = buffer[i];
                                                        ++i;
                                                        pMtaProv->DhcpOption122Suboption2[j] |= (unsigned char)((tmp << 4) + (buffer[i]));
						}
						else
							break;
					}
					printf("pMtaProv->DhcpOption122Suboption2[0] = %X %d\n",pMtaProv->DhcpOption122Suboption2[0],pMtaProv->DhcpOption122Suboption2[0]);
					printf("pMtaProv->DhcpOption122Suboption2[1] = %X %d\n",pMtaProv->DhcpOption122Suboption2[1],pMtaProv->DhcpOption122Suboption2[1]);
					printf("pMtaProv->DhcpOption122Suboption2[2] = %X %d\n",pMtaProv->DhcpOption122Suboption2[2],pMtaProv->DhcpOption122Suboption2[2]);
					printf("pMtaProv->DhcpOption122Suboption2[3] = %X %d\n",pMtaProv->DhcpOption122Suboption2[3],pMtaProv->DhcpOption122Suboption2[3]);
				}
			}

		   }
		}
  
        ip_type = MTA_IPV6_TR;
		rc = memset_s(buffer, sizeof(buffer), 0, sizeof(buffer));
                ERR_CHK(rc);
		rc = memset_s(pMtaProv->DhcpOption2171CccV6DssID1, sizeof(pMtaProv->DhcpOption2171CccV6DssID1), 0, sizeof(pMtaProv->DhcpOption2171CccV6DssID1));
                ERR_CHK(rc);
                pMtaProv->DhcpOption2171CccV6DssID1Len = 0;
		if( 0 == syscfg_get( NULL, "IPv6PrimaryDhcpServerOptions", buffer, sizeof(buffer)))
		{
		   if(buffer[0] != '\0')
		   {
			len = strlen(buffer);
			CosaDmlMTASetPrimaryDhcpServerOptions(pMyObject->pmtaprovinfo, buffer, ip_type);
			if((MtaIPMode == MTA_IPV6) || (MtaIPMode == MTA_DUAL_STACK))
			{
				if(ConverStr2Hex((unsigned char*)buffer) == ANSC_STATUS_SUCCESS)
				{

					for(i = 0,j= 0;i<len; i++,j++)
					{
						if(j<MTA_DHCPOPTION122CCCV6DSSID1_MAX)
						{
							unsigned char tmp = buffer[i];
                                                        ++i;
                                                        pMtaProv->DhcpOption2171CccV6DssID1[j] |= (unsigned char)((tmp << 4) + (buffer[i]));
						}
						else
							break;
					}
	
					printf("pMtaProv->DhcpOption2171CccV6DssID1[0] = %X %d\n",pMtaProv->DhcpOption2171CccV6DssID1[0],pMtaProv->DhcpOption2171CccV6DssID1[0]);
					printf("pMtaProv->DhcpOption2171CccV6DssID1[1] = %X %d\n",pMtaProv->DhcpOption2171CccV6DssID1[1],pMtaProv->DhcpOption2171CccV6DssID1[1]);
					printf("pMtaProv->DhcpOption2171CccV6DssID1[2] = %X %d\n",pMtaProv->DhcpOption2171CccV6DssID1[2],pMtaProv->DhcpOption2171CccV6DssID1[2]);
					printf("pMtaProv->DhcpOption2171CccV6DssID1[3] = %X %d\n",pMtaProv->DhcpOption2171CccV6DssID1[3],pMtaProv->DhcpOption2171CccV6DssID1[3]);
					pMtaProv->DhcpOption2171CccV6DssID1Len = j;
					printf("pMtaProv->DhcpOption2171CccV6DssID1Len = %d\n",pMtaProv->DhcpOption2171CccV6DssID1Len);
				}

			}
		   }
		}
		rc = memset_s(buffer, sizeof(buffer), 0, sizeof(buffer));
                ERR_CHK(rc);
		rc = memset_s(pMtaProv->DhcpOption2171CccV6DssID2, sizeof(pMtaProv->DhcpOption2171CccV6DssID2), 0, sizeof(pMtaProv->DhcpOption2171CccV6DssID2));
                ERR_CHK(rc);
		pMtaProv->DhcpOption2171CccV6DssID2Len = 0;
		if( 0 == syscfg_get( NULL, "IPv6SecondaryDhcpServerOptions", buffer, sizeof(buffer)))
		{
		   
		   if(buffer[0] != '\0')
		   {
			len = strlen(buffer);
			CosaDmlMTASetSecondaryDhcpServerOptions(pMyObject->pmtaprovinfo, buffer, ip_type);
			if((MtaIPMode == MTA_IPV6) || (MtaIPMode == MTA_DUAL_STACK))
			{
				if(ConverStr2Hex((unsigned char*)buffer) == ANSC_STATUS_SUCCESS)
				{
			   		for(i = 0,j= 0;i<len; i++,j++)
					{
						if(j<MTA_DHCPOPTION122CCCV6DSSID2_MAX)
						{
							unsigned char tmp = buffer[i];
                                                        ++i;
                                                        pMtaProv->DhcpOption2171CccV6DssID2[j] |= (unsigned char)((tmp << 4) + (buffer[i]));
						}
						else
							break;
					}
					printf("pMtaProv->DhcpOption2171CccV6DssID2[0] = %X %d\n",pMtaProv->DhcpOption2171CccV6DssID2[0],pMtaProv->DhcpOption2171CccV6DssID2[0]);
					printf("pMtaProv->DhcpOption2171CccV6DssID2[1] = %X %d\n",pMtaProv->DhcpOption2171CccV6DssID2[1],pMtaProv->DhcpOption2171CccV6DssID2[1]);
					printf("pMtaProv->DhcpOption2171CccV6DssID2[2] = %X %d\n",pMtaProv->DhcpOption2171CccV6DssID2[2],pMtaProv->DhcpOption2171CccV6DssID2[2]);
					printf("pMtaProv->DhcpOption2171CccV6DssID2[3] = %X %d\n",pMtaProv->DhcpOption2171CccV6DssID2[3],pMtaProv->DhcpOption2171CccV6DssID2[3]);
					pMtaProv->DhcpOption2171CccV6DssID2Len = j;
					printf("pMtaProv->DhcpOption2171CccV6DssID2Len = %d\n",pMtaProv->DhcpOption2171CccV6DssID2Len);
				}

			}

		   }
		}
    #ifdef ENABLE_ETH_WAN	
		// call hal to start provisioning
		if(mta_hal_start_provisioning(pMtaProv) == RETURN_OK)
		{
            if (syscfg_set(NULL, "MTA_PROVISION","true") != 0)
            {
                CcspTraceWarning(("%s: syscfg_set failed\n", __FUNCTION__));
            }
            else
            {
                CcspTraceInfo(("%s: syscfg MTA_PROVISION successfully set to true\n", __FUNCTION__));
                #if defined (SCXF10)
                prepareToStartUdhcpc();
                #endif
            }
                        /* Coverity Fix CID:74083 RESOURCE_LEAK */
                        free(pMtaProv);
			return ANSC_STATUS_SUCCESS;
		}
		else
		{
			CcspTraceError(("mta_hal_start_provisioning Failed '%s'\n", __FUNCTION__));
                         free(pMtaProv);
			return ANSC_STATUS_FAILURE;
		}
               
     #endif
}
else
	{
		printf("Memory Alloction Failed '%s'\n", __FUNCTION__);
		CcspTraceError(("Memory Alloction Failed '%s'\n", __FUNCTION__));
        if (pMtaProv) 
            free(pMtaProv);
		return ANSC_STATUS_FAILURE;
	}  

/* CID 92066 Structurally dead code fix */
free(pMtaProv);
return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaSetMTAHal
    (
        PCOSA_MTA_ETHWAN_PROV_INFO  pmtaethpro
    )
{
 char 	buffer [ 128 ] = { 0 };
 int	MtaIPMode = 0;
 int i = 0, j =0; int len = 0;
 errno_t rc = -1;
 ANSC_STATUS status = ANSC_STATUS_SUCCESS;
 unsigned char x,y;
 PMTAMGMT_MTA_PROVISIONING_PARAMS pMtaProv = NULL;
 pMtaProv = (PMTAMGMT_MTA_PROVISIONING_PARAMS)malloc(sizeof(MTAMGMT_PROVISIONING_PARAMS));

if(pMtaProv)
{

	MtaIPMode = pmtaethpro->StartupIPMode.ActiveValue;
	pMtaProv->MtaIPMode = MtaIPMode;
	printf("pMtaProv->MtaIPMode = %d\n", pMtaProv->MtaIPMode);
	rc = memset_s(pMtaProv->DhcpOption122Suboption1, sizeof(pMtaProv->DhcpOption122Suboption1), 0, sizeof(pMtaProv->DhcpOption122Suboption1));
        ERR_CHK(rc);
		if( 0 != strlen(pmtaethpro->IPv4PrimaryDhcpServerOptions.ActiveValue))
		{
                   rc = strcpy_s(buffer, sizeof(buffer), pmtaethpro->IPv4PrimaryDhcpServerOptions.ActiveValue);
                   if(rc != EOK)
                   {
                       ERR_CHK(rc);
                       status = ANSC_STATUS_FAILURE;
                       goto EXIT;
                   }
		   if(buffer[0] != '\0')
		   {
			len = strlen(buffer);
			if((MtaIPMode ==  MTA_IPV4) || (MtaIPMode == MTA_DUAL_STACK))
			{
				if(ConverStr2Hex((unsigned char*)buffer) == ANSC_STATUS_SUCCESS)
				{

					for(i = 0,j= 0;i<len; i++,j++)
					{
						if(j<MTA_DHCPOPTION122SUBOPTION1_MAX)
						{
                                                       x= buffer[i]<<4;
                                                        y= buffer[++i];
							pMtaProv->DhcpOption122Suboption1[j] |= x + y;
						}
						else
							break;
					}
	
					printf("pMtaProv->DhcpOption122Suboption1[0] = %X %d\n",pMtaProv->DhcpOption122Suboption1[0],pMtaProv->DhcpOption122Suboption1[0]);
					printf("pMtaProv->DhcpOption122Suboption1[1] = %X %d\n",pMtaProv->DhcpOption122Suboption1[1],pMtaProv->DhcpOption122Suboption1[1]);
					printf("pMtaProv->DhcpOption122Suboption1[2] = %X %d\n",pMtaProv->DhcpOption122Suboption1[2],pMtaProv->DhcpOption122Suboption1[2]);
					printf("pMtaProv->DhcpOption122Suboption1[3] = %X %d\n",pMtaProv->DhcpOption122Suboption1[3],pMtaProv->DhcpOption122Suboption1[3]);
				}
			}

		   }
		}
                rc = memset_s(pMtaProv->DhcpOption122Suboption2, sizeof(pMtaProv->DhcpOption122Suboption2), 0, sizeof(pMtaProv->DhcpOption122Suboption2));
                ERR_CHK(rc);
		if( 0 != strlen(pmtaethpro->IPv4SecondaryDhcpServerOptions.ActiveValue))
		{
                   rc = strcpy_s(buffer, sizeof(buffer), pmtaethpro->IPv4SecondaryDhcpServerOptions.ActiveValue);
                   if(rc != EOK)
	           {
                        ERR_CHK(rc);
                        status = ANSC_STATUS_FAILURE;
                        goto EXIT;
                   }
                x=0;
                y =0;
		   if(buffer[0] != '\0')
		   {
			len = strlen(buffer);
			if((MtaIPMode ==  MTA_IPV4) || (MtaIPMode == MTA_DUAL_STACK))
			{
				if(ConverStr2Hex((unsigned char*)buffer) == ANSC_STATUS_SUCCESS)
				{
			   		for(i = 0,j= 0;i<len; i++,j++)
					{
						if(j<MTA_DHCPOPTION122SUBOPTION2_MAX)

						{     
                                                        x=buffer[i]<<4;
                                                          y =buffer[++i];

							pMtaProv->DhcpOption122Suboption2[j] |= x + y;
						}
						else
							break;
					}
					printf("pMtaProv->DhcpOption122Suboption2[0] = %X %d\n",pMtaProv->DhcpOption122Suboption2[0],pMtaProv->DhcpOption122Suboption2[0]);
					printf("pMtaProv->DhcpOption122Suboption2[1] = %X %d\n",pMtaProv->DhcpOption122Suboption2[1],pMtaProv->DhcpOption122Suboption2[1]);
					printf("pMtaProv->DhcpOption122Suboption2[2] = %X %d\n",pMtaProv->DhcpOption122Suboption2[2],pMtaProv->DhcpOption122Suboption2[2]);
					printf("pMtaProv->DhcpOption122Suboption2[3] = %X %d\n",pMtaProv->DhcpOption122Suboption2[3],pMtaProv->DhcpOption122Suboption2[3]);
				}
			}

		   }
		}
  
		rc = memset_s(pMtaProv->DhcpOption2171CccV6DssID1, sizeof(pMtaProv->DhcpOption2171CccV6DssID1), 0, sizeof(pMtaProv->DhcpOption2171CccV6DssID1));
                ERR_CHK(rc);
		pMtaProv->DhcpOption2171CccV6DssID1Len = 0;
		if(0 != strlen(pmtaethpro->IPv6PrimaryDhcpServerOptions.ActiveValue))
		{
                   rc = strcpy_s(buffer, sizeof(buffer), pmtaethpro->IPv6PrimaryDhcpServerOptions.ActiveValue);
                   if(rc != EOK)
                   {
                        ERR_CHK(rc);
                        status = ANSC_STATUS_FAILURE;
                        goto EXIT;
                   }
                x =0;
                 y=0; 
		   if(buffer[0] != '\0')
		   {
			len = strlen(buffer);
			if((MtaIPMode == MTA_IPV6) || (MtaIPMode == MTA_DUAL_STACK))
			{
				if(ConverStr2Hex((unsigned char*)buffer) == ANSC_STATUS_SUCCESS)
				{

					for(i = 0,j= 0;i<len; i++,j++)
					{
						if(j<MTA_DHCPOPTION122CCCV6DSSID1_MAX)
						{
                                                        x=buffer[i]<<4;
                                                        y = buffer[++i];
							pMtaProv->DhcpOption2171CccV6DssID1[j] |= x + y;
						}
						else
							break;
					}
	
					printf("pMtaProv->DhcpOption2171CccV6DssID1[0] = %X %d\n",pMtaProv->DhcpOption2171CccV6DssID1[0],pMtaProv->DhcpOption2171CccV6DssID1[0]);
					printf("pMtaProv->DhcpOption2171CccV6DssID1[1] = %X %d\n",pMtaProv->DhcpOption2171CccV6DssID1[1],pMtaProv->DhcpOption2171CccV6DssID1[1]);
					printf("pMtaProv->DhcpOption2171CccV6DssID1[2] = %X %d\n",pMtaProv->DhcpOption2171CccV6DssID1[2],pMtaProv->DhcpOption2171CccV6DssID1[2]);
					printf("pMtaProv->DhcpOption2171CccV6DssID1[3] = %X %d\n",pMtaProv->DhcpOption2171CccV6DssID1[3],pMtaProv->DhcpOption2171CccV6DssID1[3]);
					pMtaProv->DhcpOption2171CccV6DssID1Len = j;
					printf("pMtaProv->DhcpOption2171CccV6DssID1Len = %d\n",pMtaProv->DhcpOption2171CccV6DssID1Len);
				}
			}
		   }
		}
		rc = memset_s(pMtaProv->DhcpOption2171CccV6DssID2, sizeof(pMtaProv->DhcpOption2171CccV6DssID2), 0, sizeof(pMtaProv->DhcpOption2171CccV6DssID2));
                ERR_CHK(rc);
		pMtaProv->DhcpOption2171CccV6DssID2Len = 0;
		if(0 != strlen(pmtaethpro->IPv6SecondaryDhcpServerOptions.ActiveValue))
		{
                   rc = strcpy_s(buffer, sizeof(buffer), pmtaethpro->IPv6SecondaryDhcpServerOptions.ActiveValue);
                   if(rc != EOK)
                   {
                        ERR_CHK(rc);
                        status = ANSC_STATUS_FAILURE;
                        goto EXIT;
                   }
                x=0;
                y =0;
		   if(buffer[0] != '\0')
		   {
			len = strlen(buffer);
			if((MtaIPMode == MTA_IPV6) || (MtaIPMode == MTA_DUAL_STACK))
			{
				if(ConverStr2Hex((unsigned char*)buffer) == ANSC_STATUS_SUCCESS)
				{
			   		for(i = 0,j= 0;i<len; i++,j++)
					{
						if(j<MTA_DHCPOPTION122CCCV6DSSID2_MAX)
						{
                                                        x= buffer[i]<<4;
                                                        y =buffer[++i];
							pMtaProv->DhcpOption2171CccV6DssID2[j] |= x + y;
                                                }
						else
							break;
					}
					printf("pMtaProv->DhcpOption2171CccV6DssID2[0] = %X %d\n",pMtaProv->DhcpOption2171CccV6DssID2[0],pMtaProv->DhcpOption2171CccV6DssID2[0]);
					printf("pMtaProv->DhcpOption2171CccV6DssID2[1] = %X %d\n",pMtaProv->DhcpOption2171CccV6DssID2[1],pMtaProv->DhcpOption2171CccV6DssID2[1]);
					printf("pMtaProv->DhcpOption2171CccV6DssID2[2] = %X %d\n",pMtaProv->DhcpOption2171CccV6DssID2[2],pMtaProv->DhcpOption2171CccV6DssID2[2]);
					printf("pMtaProv->DhcpOption2171CccV6DssID2[3] = %X %d\n",pMtaProv->DhcpOption2171CccV6DssID2[3],pMtaProv->DhcpOption2171CccV6DssID2[3]);
					pMtaProv->DhcpOption2171CccV6DssID2Len = j;
					printf("pMtaProv->DhcpOption2171CccV6DssID2Len = %d\n",pMtaProv->DhcpOption2171CccV6DssID2Len);
				}
			}

		   }
		}
    #ifdef ENABLE_ETH_WAN	
		// call hal to start provisioning
		if(mta_hal_start_provisioning(pMtaProv) == RETURN_OK)
		{
            if (syscfg_set(NULL, "MTA_PROVISION","true") != 0)
            {
                CcspTraceWarning(("%s: syscfg_set failed\n", __FUNCTION__));
            }
            else
            {
                CcspTraceInfo(("%s: syscfg MTA_PROVISION successfully set to true\n", __FUNCTION__));
            }
			status = ANSC_STATUS_SUCCESS;
                   
		}
		else
		{
			status = ANSC_STATUS_FAILURE;
		}
                free(pMtaProv);
                pMtaProv = NULL;              
#endif
}
else
	{
		printf("Memory Alloction Failed '%s'\n", __FUNCTION__);
		CcspTraceError(("Memory Alloction Failed '%s'\n", __FUNCTION__));
		status = ANSC_STATUS_FAILURE;
	}

EXIT:
      if(pMtaProv)
      {
          free(pMtaProv);
      }
      return status;
}
/*CID 121026 Argument Type mismatch*/
void * Mta_Sysevent_thread(void *  hThisObject)
{

 PCOSA_DATAMODEL_MTA      pMyObject    = (PCOSA_DATAMODEL_MTA)hThisObject;
#if 0
 static int sysevent_fd;
         static token_t sysevent_token;
         sysevent_fd = sysevent_open("127.0.0.1", SE_SERVER_WELL_KNOWN_PORT, SE_VERSION, "WAN State", &sysevent_token);
#endif

#if defined(INTEL_PUMA7)
         //Intel SDK 7.2 Proposed Bug Fix: Prevent CCSP MTA Crash when erouter is in IPv6 mode
         char wan_status[10] = {0};
#else
         char current_wan_state[10] = {0};
#endif

        async_id_t getnotification_asyncid;
          int err;
          char name[25]={0}, val[10]={0};
          int namelen=0, vallen=0;
          errno_t rc = -1;
          int ind = -1;
        char ethWanSyscfg[64]={'\0'};
        bool mtaInEthernetMode = false;
        if( (0 == syscfg_get( NULL, "eth_wan_enabled", ethWanSyscfg, sizeof(ethWanSyscfg))) && ((ethWanSyscfg[0] != '\0') && (strncmp(ethWanSyscfg, "true", strlen("true")) == 0)))
        {
            mtaInEthernetMode = true;
        }

#if defined(INTEL_PUMA7)
         //Intel SDK 7.2 Proposed Bug Fix: Prevent CCSP MTA Crash when erouter is in IPv6 mode
         async_id_t wan_status_asyncid;
         sysevent_set_options(sysevent_fd, sysevent_token, "wan-status", TUPLE_FLAG_EVENT);
         sysevent_setnotification(sysevent_fd, sysevent_token, "wan-status",  &wan_status_asyncid);
 

         sysevent_get(sysevent_fd, sysevent_token, "wan-status", wan_status, sizeof(wan_status));
         rc = strcmp_s((const char*)wan_status, sizeof(wan_status), "started", &ind);
         ERR_CHK(rc);
         if((rc == EOK) && (ind == 0))
         {
             if(mtaInEthernetMode)
             {
                 CcspTraceWarning(("%s wan-status started, Initializing MTA \n",__FUNCTION__));
                 CosaMTAInitializeEthWanProv(pMyObject);
             }
         }
#else


         async_id_t wan_state_asyncid;
         sysevent_set_options(sysevent_fd, sysevent_token, "current_wan_state", TUPLE_FLAG_EVENT);
         sysevent_setnotification(sysevent_fd, sysevent_token, "current_wan_state",  &wan_state_asyncid);

         sysevent_get(sysevent_fd, sysevent_token, "current_wan_state", current_wan_state, sizeof(current_wan_state));
         rc = strcmp_s((const char*)current_wan_state, sizeof(current_wan_state), "up", &ind);
         ERR_CHK(rc);
         if((rc == EOK) && (ind == 0))
         {
             if(mtaInEthernetMode)
             {
                CcspTraceWarning(("%s current_wan_state up, Initializing MTA \n",__FUNCTION__));
      	        CosaMTAInitializeEthWanProv(pMyObject);
             }
         }
#endif

        do
        {

          namelen = sizeof(name);
          vallen  = sizeof(val);
	  rc = memset_s(name, sizeof(name), 0, sizeof(name));
          ERR_CHK(rc);
	  rc = memset_s(val, sizeof(val), 0, sizeof(val));
          ERR_CHK(rc);
          err = sysevent_getnotification(sysevent_fd, sysevent_token, name, &namelen, val, &vallen, &getnotification_asyncid);

                if (!err)
                {
                    bool isEthEnabled = false;
                    if( (0 == syscfg_get( NULL, "eth_wan_enabled", ethWanSyscfg, sizeof(ethWanSyscfg))) && ((ethWanSyscfg[0] != '\0') && (strncmp(ethWanSyscfg, "true", strlen("true")) == 0)))
                    {
                        isEthEnabled = true;
                    }
#if defined(INTEL_PUMA7)
                        //Intel SDK 7.2 Proposed Bug Fix: Prevent CCSP MTA Crash when erouter is in IPv6 mode
                        CcspTraceWarning(("%s Recieved notification event  %s, status %s\n",__FUNCTION__,name,val));
                        rc = strcmp_s((const char*)name, sizeof(name), "wan_status", &ind);
                        ERR_CHK(rc);
                        if((rc == EOK) && ( ind == 0))
                        {
                            rc = strcmp_s((const char*)val, sizeof(val), "started", &ind);
                            ERR_CHK(rc);
                        }
#else
                        CcspTraceWarning(("%s Recieved notification event  %s, state %s\n",__FUNCTION__,name,val));
                        rc = strcmp_s((const char*)name, sizeof(name), "current_wan_state", &ind);
                        ERR_CHK(rc);
                        if((rc == EOK) && ( ind == 0))
                        {
                            rc = strcmp_s((const char*)val, sizeof(val), "up", &ind);
                            ERR_CHK(rc);
                        }
                            
#endif
                        if((rc == EOK) && (ind == 0))
                        {
                            if(mtaInEthernetMode != isEthEnabled)
                            {
                                CcspTraceError(("%s:%d MTA is in incorrect WAN state. MTA started in %s, but selected WAN mode %s . MTA agent will be restarted.\n",__FUNCTION__,__LINE__, mtaInEthernetMode?"Ethernet":"DOCSIS", isEthEnabled?"Ethernet":"DOCSIS"));
                                exit(0);
                            }
                            else if(mtaInEthernetMode)
                            {

                                CcspTraceWarning(("%s Initializing/Reinitializing MTA\n",__FUNCTION__));
                                CosaMTAInitializeEthWanProv(pMyObject);
                            }
                        }
                }      
        }while(FOREVER);



}


/**********************************************************************

    caller:     self

    prototype:

        ANSC_STATUS
        CosaMTAInitialize
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function initiate  cosa device info object and return handle.

    argument:   ANSC_HANDLE                 hThisObject
            This handle is actually the pointer of this object
            itself.

    return:     operation status.

**********************************************************************/

ANSC_STATUS
CosaMTAInitialize
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                  returnStatus = ANSC_STATUS_SUCCESS;
    PCOSA_DATAMODEL_MTA          pMyObject    = (PCOSA_DATAMODEL_MTA)hThisObject;
    /* Initiation all functions */

    /* Initialize middle layer for Device.DeviceInfo.  */
    CosaDmlMTAInit(NULL, (PANSC_HANDLE)pMyObject);

    //Starting thread to monitor Wan mode and wan status
    CcspTraceInfo(("%s %d Starting sysevent thread \n", __FUNCTION__, __LINE__));
#ifdef ENABLE_ETH_WAN
    sysevent_fd = sysevent_open("127.0.0.1", SE_SERVER_WELL_KNOWN_PORT, SE_VERSION, "WAN State", &sysevent_token);
    pthread_t MtaInit;
#if defined (EROUTER_DHCP_OPTION_MTA)
    pthread_create(&MtaInit, NULL, &Mta_Sysevent_thread_Dhcp_Option, (ANSC_HANDLE) hThisObject);
#else
    pthread_create(&MtaInit, NULL, &Mta_Sysevent_thread, (ANSC_HANDLE) hThisObject);
#endif
    //  CosaMTAInitializeEthWanProv(hThisObject);

#endif
    CosaMTALineTableInitialize(hThisObject);
    return returnStatus;
}

/**********************************************************************

    caller:     self

    prototype:

        ANSC_STATUS
        CosaMTARemove
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function initiate  cosa device info object and return handle.

    argument:   ANSC_HANDLE                 hThisObject
            This handle is actually the pointer of this object
            itself.

    return:     operation status.

**********************************************************************/

ANSC_STATUS
CosaMTARemove
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                     returnStatus = ANSC_STATUS_SUCCESS;
    PCOSA_DATAMODEL_MTA             pMyObject    = (PCOSA_DATAMODEL_MTA)hThisObject;

    /* Remove necessary resounce */

    /* Remove self */
    AnscFreeMemory((ANSC_HANDLE)pMyObject);

    return returnStatus;
}


//#endif
