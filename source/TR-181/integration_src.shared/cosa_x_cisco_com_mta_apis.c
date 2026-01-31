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


/**************************************************************************

    module: cosa_x_cisco_com_mta_api.c

        For COSA Data Model Library Development

    -------------------------------------------------------------------

    description:

        This file implementes back-end apis for the COSA Data Model Library

        *  CosaDmlMTAInit
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

#include <cjson/cJSON.h>

//!!!  This code assumes that all data structures are the SAME in middle-layer APIs and HAL layer APIs
//!!!  So it uses casting from one to the other
#include "cosa_x_cisco_com_mta_apis.h"
#include "mta_hal.h"
#include "syscfg/syscfg.h"
#include "safec_lib_common.h"
#include "sysevent/sysevent.h"
#include "ctype.h"
#if defined (SCXF10)
#include "voice_dhcp_hal.h"
#include "bcm_generic_hal.h"
#include "dhcp_config_key.h" // for dhcpEncryptCfgFile()
#endif


// #include "cosa_x_cisco_com_mta_internal.h"

#define PARTNERS_INFO_FILE              "/nvram/partners_defaults.json"
#define BOOTSTRAP_INFO_FILE             "/opt/secure/bootstrap.json"
#define BOOTSTRAP_INFO_FILE_BACKUP      "/nvram/bootstrap.json"
#define CLEAR_TRACK_FILE                "/nvram/ClearUnencryptedData_flags"
#define NVRAM_BOOTSTRAP_CLEARED         (1 << 0)
#define MAX_LINE_REG 256

#if defined (SCXF10)
#define MAX_STR 256
#define MAX_DHCP_OPTION_LEN 2048
static const char *cxcProvDhcpOptionsPrefix = "tmp";
static const char cxcProvDhcpV4OptionsFilePath[]   = "/%s/udhcpc/%s/option.env";
static const char cxcProvDhcpV4Option122FilePath[] = "/%s/udhcpc/%s/option122.out";
static const char cxcProvDhcpV6Option17FilePath[]  = "/%s/udhcp6c/%s/option17.out";
static const char cxcProvDhcpV6Option23FilePath[]  = "/%s/udhcp6c/%s/option23.out";
static const char cxcProvDhcpV6Option24FilePath[]  = "/%s/udhcp6c/%s/option24.out";
static const char cxcProvDhcpV6Option39FilePath[]  = "/%s/udhcp6c/%s/option39.out";


/****************************************************************************
 ** FUNCTION:    parseFqdnFormat
 **
 ** PURPOSE:     Parse FQDN format from buffer.
 **
 ** PARAMETERS:  buf      (in)   Buffer to read FQDN formatted data
 **              bufLen   (in)   Length of FQDN formatted data
 **              fqdn     (out)  FQDN string (NULL terminated)
 **              fqdnLen  (in)   length of fqdn buffer
 **
 ** RETURNS:     None
 **
 ****************************************************************************/
static void parseFqdnFormat(char *buf, int bufLen, char *fqdn, int fqdnLen)
{
    int i, subLen = 0;
    int len = (bufLen <= fqdnLen) ? bufLen : fqdnLen;   /* Use the smaller of the two */

    AnscTraceVerbose(("bufLen(%d), fqdnLen(%d)\n", bufLen, fqdnLen));

    if (!buf || !fqdn)
    {
        AnscTraceError(("Invalid input, buf(%p), fqdn(%p)\n", buf, fqdn));
        return;
    }

    /* Convert FQDN format to string
     * FQDN format:
     *  | len | value | len | value | ...
     *  eg. my.fqdn.com => 02 6D 79 04 66 71 64 6E 03 63 6F 6D  00
     *                    |   my   |     fqdn     |    com    | (optional)
     */
    for (i = 0; i < len; i++)
    {
        if (0 == subLen)
        {
            /* Store new length and replace with '.' */
            subLen = buf[i];
            if (0 == subLen)
            {
                fqdn[i - 1] = '\0';
            }
            else
            {
                if (i != 0)
                {
                    fqdn[i - 1] = '.';
                }
            }
        }
        else
        {
            fqdn[i - 1] = buf[i];
            subLen--;
        }
    }
    fqdn[i - 1] = '\0';
}

/****************************************************************************
 ** FUNCTION:    getSubOptionDHCPv6
 **
 ** PURPOSE:     Get any specified sub-option data from DHCPv6 sub-option.
 **
 ** PARAMETERS:  subData      (in)   all of sub-option TLV
 **              subDataLen   (in)   total len of subData
 **              dstCode      (in)   target sub-option code
 **              outData      (out)  out sub-option data if found
 **              outDataMax   (in)   max outData buffer len
 **              outLen       (out)  number of bytes written to outData
 **
 ** RETURNS:     1 - found,  0 - not found
 **
 ****************************************************************************/
static int getSubOptionDHCPv6(const char *subData, int subDataLen, uint16_t dstCode,
                                   char *outData, int outDataMax, int *outLen)
{
    const char *buffPtr, *endPtr;
    uint16_t subCode, subLen;

    AnscTraceVerbose(("dstCode(%d), subDataLen(%d)\n", dstCode, subDataLen));

    *outLen = 0;

    buffPtr = subData;
    endPtr = &subData[subDataLen - 1]; /* Point to the last character */

    /* walk through all sub-option */
    while (buffPtr <= endPtr - 3) /* Account for tag and length read */
    {
        /* Read tag */
        subCode = ntohs(*((uint16_t *)(buffPtr)));
        buffPtr += sizeof(uint16_t);

        /* Read length */
        subLen = ntohs(*((uint16_t *)(buffPtr)));
        buffPtr += sizeof(uint16_t);

        if ((buffPtr + subLen - 1 <= endPtr) && (dstCode == subCode))
        {
            *outLen = subLen < outDataMax ? subLen : outDataMax;
            memcpy(outData, buffPtr, *outLen);
            return 1;
        }
        buffPtr += subLen;
    }

    return 0;
}

/****************************************************************************
 ** FUNCTION:     parseDHCPv6Option17
 **
 ** PURPOSE:      parse DHCPv6 option 17 sub-options TLV to obtain tftp Server Addr and
 **               tftp File name
 **
 ** PARAMETERS:   buffer          (in)   pointer to entire DHCPv6 option 17
 **               len             (in)   option 17 total len
 **               tftpServer      (out)  pointer to tftpServer buffer
 **               tftpServerLen   (in)   tftpServer buffer  len
 **               tftpFile        (out)  pointer to tftpFile buffer
 **               tftpFileLen     (in)   tftpFile buffer len
 **               provServer      (out)  pointer to provServer buffer
 **               provServerLen   (in)   provServer buffer len
 **               syslogServer    (out)  pointer to syslogServer buffer
 **               syslogServerLen (in)   syslogServer buffer len
 **
 ** RETURNS:      0 - successful,  others - fail.
 **
 ****************************************************************************/
static int parseDHCPv6Option17(const char *buffer, int len,
                               char *tftpServer, int tftpServerLen,
                               char *tftpFile, int tftpFileLen,
                               char *provServer, int provServerLen,
                               char *syslogServer, int syslogServerLen)
{
#define SUB_OPTION_TFTP_SERVER_ADDR  32
#define SUB_OPTION_CFG_FILE_NAME     33
#define SUB_OPTION_SYSLOG_SERVER     34
#define SUB_OPTION_CABLELABS_CONFIG  2171
#define ELEMENT_PROV_SERVER          3
#define FQDN_TYPE                    0

    int offset = 0, subDataLen = 0, tmpLen = 0;
    char tmpBuff[MAX_STR] = {0};
    struct in6_addr in6Addr = {0};

    AnscTraceVerbose(("Entry\n"));

    if (!len || !buffer || !tftpServer || !tftpServerLen || !tftpFile || !tftpFileLen
        || !provServer || !provServerLen || !syslogServer || !syslogServerLen)
    {
        AnscTraceError(("Invalid input\n"));
        return -1;
    }

    offset += sizeof(uint16_t); /* skip option code */
    offset += sizeof(uint16_t); /* skip option code len */
    offset += sizeof(uint32_t); /* skip  enterprise id */
    subDataLen = len - offset;

    if (!getSubOptionDHCPv6(&buffer[offset], subDataLen,
                            SUB_OPTION_TFTP_SERVER_ADDR, tmpBuff,
                            sizeof(tmpBuff), &tmpLen))
    {
        AnscTraceInfo(("get sub-option %d failed\n", SUB_OPTION_TFTP_SERVER_ADDR));
    }
    else
    {
        memcpy(&in6Addr.s6_addr, tmpBuff, sizeof(in6Addr.s6_addr));
        inet_ntop(AF_INET6, &in6Addr, tftpServer, tftpServerLen);
        AnscTraceVerbose(("found tftpServer = [%s]\n", tftpServer));
    }

    if (!getSubOptionDHCPv6(&buffer[offset], subDataLen,
                            SUB_OPTION_CFG_FILE_NAME, tftpFile, tftpFileLen,
                            &tmpLen))
    {
        AnscTraceInfo(("get sub-option %d failed\n", SUB_OPTION_CFG_FILE_NAME));
    }
    else
    {
        AnscTraceVerbose(("found tftpFile = [%s]\n", tftpFile));
    }

    if (!getSubOptionDHCPv6(&buffer[offset], subDataLen,
                            SUB_OPTION_SYSLOG_SERVER, tmpBuff, sizeof(tmpBuff),
                            &tmpLen))
    {
        AnscTraceInfo(("get sub-option %d failed\n", SUB_OPTION_SYSLOG_SERVER));
    }
    else
    {
        memset(&in6Addr, 0, sizeof(in6Addr));
        memcpy(&in6Addr.s6_addr, tmpBuff, sizeof(in6Addr.s6_addr));
        inet_ntop(AF_INET6, &in6Addr, syslogServer, syslogServerLen);
        AnscTraceVerbose(("found syslogServer = [%s]\n", syslogServer));
    }

    if (!getSubOptionDHCPv6(&buffer[offset], subDataLen,
                            SUB_OPTION_CABLELABS_CONFIG, tmpBuff,
                            sizeof(tmpBuff), &tmpLen))
    {
        AnscTraceInfo(("get sub-option %d failed\n", SUB_OPTION_CABLELABS_CONFIG));
    }
    else
    {
        char tmpBuff2[MAX_STR] = {0};

        /* Parse elements */
        if (!getSubOptionDHCPv6(tmpBuff, tmpLen, ELEMENT_PROV_SERVER,
                                tmpBuff2, sizeof(tmpBuff2), &tmpLen))
        {
            AnscTraceInfo(("get element %d failed\n", ELEMENT_PROV_SERVER));
        }
        else if (FQDN_TYPE == tmpBuff2[0])
        {
            parseFqdnFormat(tmpBuff2 + sizeof(uint8_t), /* Skip type code */
                            tmpLen - sizeof(uint8_t),   /* Skip type code */
                            provServer, provServerLen);
            AnscTraceVerbose(("found provServer = [%s]\n", provServer));
        }
    }

    return 0;
}

/****************************************************************************
 ** FUNCTION:    getSubOptionDHCPv4
 **
 ** PURPOSE:     Get any specified sub-option data from DHCPv4 sub-option.
 **
 ** PARAMETERS:  subData      (in)   all of sub-option TLV
 **              subDataLen   (in)   total len of subData
 **              dstCode      (in)   target sub-option code
 **              outData      (out)  out sub-option data if found
 **              outDataMax   (in)   max outData buffer len
 **              outLen       (out)  number of bytes written to outData
 **
 ** RETURNS:     1 - found,  0 - not found
 **
 ****************************************************************************/
static int getSubOptionDHCPv4(const char *subData, int subDataLen, uint16_t dstCode,
                                   char *outData, int outDataMax, int *outLen)
{
    const char *buffPtr, *endPtr;
    uint8_t subCode, subLen;

    AnscTraceVerbose(("dstCode(%d)\n", dstCode));

    *outLen = 0;

    buffPtr = subData;
    endPtr = &subData[subDataLen - 1]; /* Point to the last character */

    /* walk through all sub-option */
    while (buffPtr <= endPtr - 1) /* Account for tag and length read */
    {
        /* Read tag */
        subCode = buffPtr[0];
        buffPtr += sizeof(uint8_t);

        /* Read length */
        subLen = buffPtr[0];
        buffPtr += sizeof(uint8_t);

        if ((buffPtr + subLen - 1 <= endPtr) && (dstCode == subCode))
        {
            *outLen = subLen < outDataMax ? subLen : outDataMax;
            memcpy(outData, buffPtr, *outLen);
            return 1;
        }
        buffPtr += subLen;
    }

    return 0;
}

/****************************************************************************
 ** FUNCTION:     parseDHCPv4Option122
 **
 ** PURPOSE:      parse DHCPv4 option 122 sub-options TLV.
 **
 ** PARAMETERS:   buffer          (in)   pointer to entire DHCPv4 option 122
 **               len             (in)   option 122 total len
 **               provServer      (out)  pointer to provServer buffer
 **               provServerLen   (in)   provServer buffer len
 **
 ** RETURNS:      0 - successful,  others - fail.
 **
 ****************************************************************************/
static int parseDHCPv4Option122(const char *buffer, int len,
                                char *provServer, int provServerLen)
{
#define SUB_OPTION_PROV_SERVER  3

    int offset = 0, subDataLen = 0, tmpLen = 0;
    char tmpBuff[MAX_STR] = {0};

    AnscTraceVerbose(("Entry\n"));

    if (!len || !buffer || !provServer || !provServerLen)
    {
        AnscTraceError(("Invalid input, buffer(%p), len(%d), provServer(%p), "
                        "provServerLen(%d)\n",
                        buffer, len, provServer, provServerLen));
        return -1;
    }

    offset += sizeof(uint8_t); /* skip option code */
    offset += sizeof(uint8_t); /* skip option code len */
    subDataLen = len - offset;

    if (!getSubOptionDHCPv4(&buffer[offset], subDataLen,
                            SUB_OPTION_PROV_SERVER, tmpBuff,
                            sizeof(tmpBuff), &tmpLen))
    {
        AnscTraceInfo(("get sub-option %d failed\n", SUB_OPTION_PROV_SERVER));
    }
    else if (FQDN_TYPE == tmpBuff[0])
    {
        parseFqdnFormat(tmpBuff + sizeof(uint8_t),  /* Skip type code */
                        tmpLen - sizeof(uint8_t),   /* Skip type code */
                        provServer, provServerLen);
    }

    return 0;
}

/****************************************************************************
 ** FUNCTION:    parseDHCPv4Options
 **
 ** PURPOSE:     Parse DHCPv4 env file to obtain DHCP parameters.
 **
 ** PARAMETERS:  buffer           (in/out)pointer to DHCPv4 env file (NULL
 **                                       terminated) - buffer may be modified
 **              tftpServer       (out)   pointer to tftpServer buffer
 **              tftpServerLen    (in)    tftpServer buffer len
 **              tftpFile         (out)   pointer to tftpFile buffer
 **              tftpFileLen      (in)    tftpFile buffer len
 **              provServer       (out)   pointer to provServer buffer
 **              provServerLen    (in)    provServer buffer len
 **              domain           (out)   pointer to domain buffer
 **              domainLen        (in)    domain buffer len
 **              syslogServer     (out)   pointer to syslogServer buffer
 **              syslogServerLen  (in)    syslogServer buffer len
 **              hostname         (out)   pointer to hostname buffer
 **              hostnameLen      (in)    hostname buffer len
 **              dnsServer        (out)   pointer to dnsServer buffer
 **              dnsServerLen     (in)    dnsServer buffer len
 **
 ** RETURNS:     0 - successful,  others - fail.
 **
 ****************************************************************************/
static int parseDHCPv4Options(char *buffer,
                              char *tftpServer, int tftpServerLen,
                              char *tftpFile, int tftpFileLen,
                              char *provServer, int provServerLen,
                              char *domain, int domainLen,
                              char *syslogServer, int syslogServerLen,
                              char *hostname, int hostnameLen,
                              char *dnsServer, int dnsServerLen)
{
#define DHCP_FIELD_NEXT_SERVER_IP_ADDRESS   "siaddr"
#define DHCP_FIELD_SERVER_HOST_NAME         "sname"
#define DHCP_FIELD_BOOT_FILE_NAME           "boot_file"
#define DHCP_OPTION_6_DOMAIN_NAME_SERVER    "dns"
#define DHCP_OPTION_7_LOG_SERVER            "logsvr"
#define DHCP_OPTION_12_HOST_NAME            "hostname"
#define DHCP_OPTION_15_DOMAIN_NAME          "domain"

    char *pLine = NULL, *pTag = NULL, *pValue = NULL;
    char *saveptrBuffer = NULL, *saveptrLine = NULL;

    AnscTraceVerbose(("Entry\n"));

    pLine = strtok_r(buffer, "\n", &saveptrBuffer);

    while (NULL != pLine)
    {
        AnscTraceVerbose(("pLine: %s\n", pLine));

        pTag = strtok_r(pLine, "=", &saveptrLine);
        pValue = strtok_r(NULL, "=", &saveptrLine);

        if (NULL != pValue)
        {
            if (tftpServer && !strncmp(pTag, DHCP_FIELD_NEXT_SERVER_IP_ADDRESS,
                                       sizeof(DHCP_FIELD_NEXT_SERVER_IP_ADDRESS)))
            {
                snprintf(tftpServer, tftpServerLen, "%s", pValue);
                AnscTraceVerbose(("found tag=[%s], tftpserver value=[%s]\n", DHCP_FIELD_NEXT_SERVER_IP_ADDRESS, pValue));
            }
            else if (provServer && !strncmp(pTag, DHCP_FIELD_SERVER_HOST_NAME,
                                            sizeof(DHCP_FIELD_SERVER_HOST_NAME)))
            {
                snprintf(provServer, provServerLen, "%s", pValue);
                AnscTraceVerbose(("found tag=[%s], provServer value=[%s]\n", DHCP_FIELD_SERVER_HOST_NAME, pValue));
            }
            else if (tftpFile && !strncmp(pTag, DHCP_FIELD_BOOT_FILE_NAME,
                                          sizeof(DHCP_FIELD_BOOT_FILE_NAME)))
            {
                snprintf(tftpFile, tftpFileLen, "%s", pValue);
                AnscTraceVerbose(("found tag=[%s], tftpFile value=[%s]\n", DHCP_FIELD_BOOT_FILE_NAME, pValue));
            }
            else if (syslogServer && !strncmp(pTag, DHCP_OPTION_7_LOG_SERVER,
                                              sizeof(DHCP_OPTION_7_LOG_SERVER)))
            {
                snprintf(syslogServer, syslogServerLen, "%s", pValue);
                AnscTraceVerbose(("found tag=[%s], syslogServer value=[%s]\n", DHCP_OPTION_7_LOG_SERVER, pValue));
            }
            else if (hostname && !strncmp(pTag, DHCP_OPTION_12_HOST_NAME,
                                      sizeof(DHCP_OPTION_12_HOST_NAME)))
            {
                snprintf(hostname, hostnameLen, "%s", pValue);
                AnscTraceVerbose(("found tag=[%s], hostname value=[%s]\n", DHCP_OPTION_12_HOST_NAME, pValue));
            }
            else if (domain && !strncmp(pTag, DHCP_OPTION_15_DOMAIN_NAME,
                                        sizeof(DHCP_OPTION_15_DOMAIN_NAME)))
            {
                snprintf(domain, domainLen, "%s", pValue);
                AnscTraceVerbose(("found tag=[%s], domain value=[%s]\n", DHCP_OPTION_15_DOMAIN_NAME, pValue));
            }
            else if (dnsServer && !strncmp(pTag, DHCP_OPTION_6_DOMAIN_NAME_SERVER,
                                           sizeof(DHCP_OPTION_6_DOMAIN_NAME_SERVER)))
            {
                snprintf(dnsServer, dnsServerLen, "%s", pValue);
                AnscTraceVerbose(("found tag=[%s], value=[%s]\n", DHCP_OPTION_6_DOMAIN_NAME_SERVER, pValue));
            }
        }

        /* Get next line */
        pLine = strtok_r(NULL, "\n", &saveptrBuffer);
    }

    return 0;
}

/****************************************************************************
 ** FUNCTION:    getOptionsFileData
 **
 ** PURPOSE:     Get options file data, decrypt, and store in buffer.
 **
 ** PARAMETERS:  buf      (out)  Buffer to store file data
 **              bufLen   (in)   Length of buffer
 **              filepath (in)   filepath string
 **              ifName   (in)   interface name string
 **
 ** RETURNS:     Length of bytes stored to buffer.
 **
 ****************************************************************************/
static int getOptionsFileData(char *buf, int bufLen, const char *filepath, const char *ifName)
{
    FILE *f = NULL;
    int len = 0;
    char fullFilePath[MAX_STR] = {0};

    if ( filepath == NULL || ifName == NULL )
    {
        AnscTraceError(( "filepath or ifName is NULL\n" ));
        return 0;
    }

    /* Build file path */
    snprintf(fullFilePath, sizeof(fullFilePath), filepath, cxcProvDhcpOptionsPrefix, ifName);

    /* Parse DHCP Options file */
    f = fopen(fullFilePath, "r");
    if (f != NULL)
    {
        len = fread(buf, 1, bufLen, f);
        fclose(f);

        AnscTraceVerbose(("fullFilePath(%s) read %d bytes\n", fullFilePath, len));

        /* decrypt file */
        dhcpEncryptCfgFile(buf, len, BRCM_DHCP_CONFIG_KEY);
    }
    else
    {
        // Not all options are expected to be present
        AnscTraceInfo(("cannot open fullFilePath(%s) or file does not exist\n",
                       fullFilePath));
        return 0;
    }

    return len;
}

/****************************************************************************
* FUNCTION:   fillDhcpOptionsV4
*
* PURPOSE:    Fill the DHCPv4 options portion of the VoiceInterfaceInfoType
*             structure if the information is available.
*
* PARAMETERS: pIfInfo - Pointer to the VoiceInterfaceInfoType structure
*
* RETURNS:    None
*
* NOTES:      This function relies on the temporary dhcp options file in the
*             system.
*
****************************************************************************/
static void fillDhcpOptionsV4(VoiceInterfaceInfoType *pIfInfo)
{
    int len;
    char buffer[MAX_DHCP_OPTION_LEN+1] = {0};

    AnscTraceVerbose(("Entry\n"));

    len = getOptionsFileData(buffer, MAX_DHCP_OPTION_LEN,
                             cxcProvDhcpV4OptionsFilePath, pIfInfo->intfName);
    if (len)
    {
        buffer[len] = '\0';

        parseDHCPv4Options(buffer,
                           pIfInfo->v4NextServerIp, sizeof(pIfInfo->v4NextServerIp),
                           pIfInfo->v4BootFileName, sizeof(pIfInfo->v4BootFileName),
                           pIfInfo->v4ServerHostName, sizeof(pIfInfo->v4ServerHostName),
                           pIfInfo->v4DomainName, sizeof(pIfInfo->v4DomainName),
                           pIfInfo->v4LogServerIp, sizeof(pIfInfo->v4LogServerIp),
                           pIfInfo->v4HostName, sizeof(pIfInfo->v4HostName),
                           pIfInfo->v4DnsServers, sizeof(pIfInfo->v4DnsServers));
    }

    /* OPTION 122 - CableLabs Client Configuration (RFC 3495) */
    memset(buffer, 0, sizeof(buffer));
    len = getOptionsFileData(buffer, sizeof(buffer),
                             cxcProvDhcpV4Option122FilePath, pIfInfo->intfName);
    if (len)
    {
        /* Overwrite with option 122 if available */
        parseDHCPv4Option122(buffer, len, pIfInfo->v4ProvServer, sizeof(pIfInfo->v4ProvServer));
        AnscTraceVerbose(("option 122 provServer = [%s]\n", pIfInfo->v4ProvServer));
    }
}

/****************************************************************************
* FUNCTION:   fillDhcpOptionsV6
*
* PURPOSE:    Fill the DHCPv6 options portion of the VoiceInterfaceInfoType
*             structure if the information is available.
*
* PARAMETERS: pIfInfo - Pointer to the VoiceInterfaceInfoType structure
*
* RETURNS:    None
*
* NOTES:      This function relies on the temporary dhcp options file in the
*             system.
*
****************************************************************************/
static void fillDhcpOptionsV6(VoiceInterfaceInfoType *pIfInfo)
{
    int len, subLen;
    char buffer[MAX_DHCP_OPTION_LEN] = {0};
    char *pStr;
    struct in6_addr in6Addr = {0};

    AnscTraceVerbose(("Entry\n"));

    /* OPTION 17 - Vendor-specific Information */
    len = getOptionsFileData(buffer, sizeof(buffer),
                             cxcProvDhcpV6Option17FilePath, pIfInfo->intfName);
    if (len)
    {
        parseDHCPv6Option17(buffer, len,
                            pIfInfo->v6TftpServerIp, sizeof(pIfInfo->v6TftpServerIp),
                            pIfInfo->v6TftpFileName, sizeof(pIfInfo->v6TftpFileName),
                            pIfInfo->v6ProvServerIp, sizeof(pIfInfo->v6ProvServerIp),
                            pIfInfo->v6SyslogServerIp, sizeof(pIfInfo->v6SyslogServerIp));
    }

    /* OPTION 23 - DNS list */
    len = getOptionsFileData(buffer, sizeof(buffer),
                             cxcProvDhcpV6Option23FilePath, pIfInfo->intfName);
    if (len >= (int)(sizeof(in6Addr.s6_addr) + 4)) // Account for 2 byte tag and length
    {
        char tmpBuff[MAX_STR] = {0};

        memcpy(&in6Addr.s6_addr, tmpBuff, sizeof(in6Addr.s6_addr));
        inet_ntop(AF_INET6, &in6Addr, pIfInfo->v6DnsServers, sizeof(pIfInfo->v6DnsServers));
        AnscTraceVerbose(("option 23 v6DnsServers = [%s]\n", pIfInfo->v6DnsServers));
    }

    /* OPTION 24 - Domain Search List */
    len = getOptionsFileData(buffer, sizeof(buffer),
                             cxcProvDhcpV6Option24FilePath, pIfInfo->intfName);
    if (len)
    {
        subLen = ntohs(*(uint16_t *)(buffer + sizeof(uint16_t))); /* store data length */
        pStr = buffer + sizeof(uint16_t) + sizeof(uint16_t); /* skip option code and size */
        parseFqdnFormat(pStr, subLen, pIfInfo->v6DomainName, sizeof(pIfInfo->v6DomainName));
        AnscTraceVerbose(("option 24 domain = [%s]\n", pIfInfo->v6DomainName));
    }

    /* OPTION 39 - Client FQDN */
    len = getOptionsFileData(buffer, sizeof(buffer),
                             cxcProvDhcpV6Option39FilePath, pIfInfo->intfName);
    if (len)
    {
        subLen = ntohs(*(uint16_t *)(buffer + sizeof(uint16_t))); /* store data length */
        pStr = buffer + sizeof(uint16_t) + sizeof(uint16_t); /* skip option code and size */
        parseFqdnFormat(pStr + sizeof(uint8_t),     /* Skip reserved byte */
                        subLen - sizeof(uint8_t),   /* Skip reserved byte */
                        pIfInfo->v6ClientFqdn, sizeof(pIfInfo->v6ClientFqdn));
        AnscTraceVerbose(("option 39 fqdn = [%s]\n", pIfInfo->v6ClientFqdn));
    }
}

#endif
#ifdef MTA_TR104SUPPORT

int CosaDmlTR104DataSet(char *pString,int bootup);

#define MAX_BASE64_LEN 8192

int mtaReapplytr104Conf(void)
{
	
        int length;
	char *buffer;
	FILE *fptr;

	fptr = fopen("/nvram/.vsb64.txt","rb");
        CcspTraceDebug(("%s Entering \n",__FUNCTION__));
        
	if (fptr == NULL) {    
		CcspTraceDebug(("file not present \n"));
		return ANSC_STATUS_FAILURE;
	}
         
	fseek(fptr, 0, SEEK_END);
	length = ftell (fptr);
        if (length < 0) {
            CcspTraceDebug(("Error getting file length"));
            fclose(fptr);
            return ANSC_STATUS_FAILURE;  
        } else {
           CcspTraceDebug(("Current file position: %d\n", length));
        }
	fseek(fptr, 0, SEEK_SET);
	buffer = (char*)malloc( length+1 );
	if (buffer == NULL){
            fclose(fptr);
	    return ANSC_STATUS_FAILURE;
        }

	if (buffer) {
		size_t bytesRead = fread(buffer, 1, length, fptr);
		if ( bytesRead != (size_t) length ){
			if ( bytesRead < (size_t) length){
				CcspTraceDebug((" The complete file is not read \n"));
			}
			if (ferror(fptr)) {
				CcspTraceDebug((" Error in reading the file  \n"));
			}
			fclose(fptr);
			free(buffer);
			return ANSC_STATUS_FAILURE;
		}

		buffer[bytesRead] = '\0';
		CcspTraceDebug(("buffer=%s\n", buffer));

        /* Coverity fix CID : 513281 Insecure data handling */
        if (buffer[0] == '\0')
        {
            CcspTraceDebug(("Buffer is empty or invalid\n"));
            free(buffer);
            fclose(fptr);
            return ANSC_STATUS_FAILURE;
        }

		CcspTraceInfo(("%s:Calling CosaDmlTR104DataSet \n", __func__));
		CosaDmlTR104DataSet(buffer, 1);
		free(buffer);
	}
       
        CcspTraceDebug(("%s Exiting \n",__FUNCTION__));
	fclose(fptr);
	return ANSC_STATUS_SUCCESS;
  
}

#endif // MTA_TR104SUPPORT

#if defined (SCXF10)
static char voiceInterface[32] = { 0 };

static uint8_t cbSubsIfInfo(char *pIntfName, uint8_t enable)
{
    if (enable)
    {
        /* Save voice interface selection */
        strncpy(voiceInterface, pIntfName, sizeof(voiceInterface));
    }

    return 1;
}

static void setSysevent(char * pCommand, uint8_t ui8Enable)
{
    int syseventFd = -1;
    token_t syseventToken;

    syseventFd = sysevent_open("127.0.0.1", SE_SERVER_WELL_KNOWN_PORT, SE_VERSION, "Firewall", &syseventToken);
    if (syseventFd < 0)
    {
        AnscTraceError(("%s:%d - sysevent_open failed\n", __FUNCTION__, __LINE__));
        return;
    }

    AnscTraceInfo(("%s: ui8Enable=%d\n", __FUNCTION__, ui8Enable));
    AnscTraceInfo(("%s: Calling sysevent command: %s\n", __FUNCTION__, pCommand));
    if (ui8Enable)
    {
        if (NULL == pCommand || '\0' == pCommand[0] || strlen(pCommand) <= 0)
        {
            AnscTraceError(("%s: Invalid command string\n", __FUNCTION__));
            sysevent_close(syseventFd, syseventToken);
            return;
        }
        sysevent_set(syseventFd, syseventToken, "VoiceIpRule", pCommand, 0);
    }
    else
        sysevent_unset(syseventFd, syseventToken, "VoiceIpRule");

    AnscTraceInfo(("%s: Calling sysevent firewall-restart\n", __FUNCTION__));
    sysevent_set(syseventFd, syseventToken, "firewall-restart", "", 0);
    sysevent_close(syseventFd, syseventToken);
}

static uint8_t cbSetFirewallRule(VoiceFirewallRuleType *pFirewallRule)
{
    char command[1000] = { 0 };
    char protocol[10] = "UDP";

    /* Default all protocol to UDP except TCP.  Do not support "TCP or UDP" option. */
    if (!strcmp("TCP", pFirewallRule->protocol))
    {
       strcpy(protocol, "TCP");
    }

    AnscTraceInfo(("%s: enable=%d, ifName=%s, protocol=%s, destPort=%u\n",
                   __FUNCTION__, pFirewallRule->enable,
                   pFirewallRule->ifName,
                   protocol,
                   pFirewallRule->destinationPort));
    if (pFirewallRule->enable)
    {
       snprintf(command, sizeof(command), "-A INPUT -p %s -i %s --dport %u -j ACCEPT",
                protocol, pFirewallRule->ifName, pFirewallRule->destinationPort);
    }
    setSysevent(command, pFirewallRule->enable);
    return 1;
}

static uint8_t cbGetCertInfo(VoiceCertificateInfoType *pCertInfo)
{
    UNREFERENCED_PARAMETER(pCertInfo);

    return 0;
}


void
CosaDmlNotifyIf(char *pIpAddr)
{
    VoiceInterfaceInfoType sysIfaceInfo = { 0 };

    // Fill interface info
    sysIfaceInfo.isPhyUp = TRUE;
    strncpy(sysIfaceInfo.intfName, voiceInterface, sizeof(sysIfaceInfo.intfName));
    if (strchr(pIpAddr, '.'))
    {
        sysIfaceInfo.isIpv4Up = TRUE;
        strncpy(sysIfaceInfo.ipv4Addr, pIpAddr, sizeof(sysIfaceInfo.ipv4Addr));
    }
    else
    {
        sysIfaceInfo.isIpv6Up = TRUE;
        strncpy(sysIfaceInfo.ipv6GlobalAddr, pIpAddr, sizeof(sysIfaceInfo.ipv6GlobalAddr));
    }

	// Fill DHCP options
    fillDhcpOptionsV4(&sysIfaceInfo);
	fillDhcpOptionsV6(&sysIfaceInfo);

    // Call interface info notification
	AnscTraceInfo(("Calling voice_hal_interface_info_notify...\n"));

    /* Call HAL API */
    voice_hal_interface_info_notify(&sysIfaceInfo);
}

#endif
ANSC_STATUS
CosaDmlMTAInit
    (
        ANSC_HANDLE                 hDml,
        PANSC_HANDLE                phContext
    )
{
    UNREFERENCED_PARAMETER(hDml);
    UNREFERENCED_PARAMETER(phContext);

   // PCOSA_DATAMODEL_MTA      pMyObject    = (PCOSA_DATAMODEL_MTA)phContext;

	AnscTraceInfo(("CosaDmlMTAInit:  mta_hal_InitDB() \n"));

    if ( mta_hal_InitDB() == RETURN_OK )
	{
#if defined (SCXF10)
		AnscTraceInfo(("CosaDmlMTAInit: voice_hal_register_cb() \n"));

		/* Register callback functions */
		voice_hal_register_cb(cbSubsIfInfo, cbSetFirewallRule, cbGetCertInfo);
#endif
        return ANSC_STATUS_SUCCESS;
    }
    else
        return ANSC_STATUS_FAILURE;
}

ANSC_STATUS
CosaDmlMTAGetDHCPInfo
    (
        ANSC_HANDLE                 hContext,
        PCOSA_MTA_DHCP_INFO         pInfo
    )
{

    UNREFERENCED_PARAMETER(hContext);
    if ( mta_hal_GetDHCPInfo((PMTAMGMT_MTA_DHCP_INFO)pInfo) == RETURN_OK )
        return ANSC_STATUS_SUCCESS;
    else
        return ANSC_STATUS_FAILURE;
}

ANSC_STATUS
CosaDmlMTAGetDHCPV6Info
    (
        ANSC_HANDLE                 hContext,
        PCOSA_MTA_DHCPv6_INFO       pInfo
    )
{

    UNREFERENCED_PARAMETER(hContext);
    if ( mta_hal_GetDHCPV6Info((PMTAMGMT_MTA_DHCPv6_INFO)pInfo) == RETURN_OK )
        return ANSC_STATUS_SUCCESS;
    else
        return ANSC_STATUS_FAILURE;
}

ANSC_STATUS
CosaDmlMTAGetPktc
    (
        ANSC_HANDLE                 hContext,
        PCOSA_MTA_PKTC              pPktc
    )
{
    UNREFERENCED_PARAMETER(hContext);
    UNREFERENCED_PARAMETER(pPktc);

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlMTASetPktc
    (
        ANSC_HANDLE                 hContext,
        PCOSA_MTA_PKTC              pPktc
    )
{
    UNREFERENCED_PARAMETER(hContext);
    UNREFERENCED_PARAMETER(pPktc);

    return ANSC_STATUS_SUCCESS;
}

ULONG
CosaDmlMTALineTableGetNumberOfEntries
    (
        ANSC_HANDLE                 hContext
    )
{
    UNREFERENCED_PARAMETER(hContext);
    return mta_hal_LineTableGetNumberOfEntries();
}

ANSC_STATUS
CosaDmlMTALineTableGetEntry
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulIndex,
        PCOSA_MTA_LINETABLE_INFO    pEntry
    )

{

    UNREFERENCED_PARAMETER(hContext);
    if ( mta_hal_LineTableGetEntry(ulIndex, (PMTAMGMT_MTA_LINETABLE_INFO)pEntry) == RETURN_OK )
        return ANSC_STATUS_SUCCESS;
    else
        return ANSC_STATUS_FAILURE;
}

ANSC_STATUS
CosaDmlMTATriggerDiagnostics
    (
        ULONG                         nIndex
    )
{ 
    if ( mta_hal_TriggerDiagnostics(nIndex) == RETURN_OK )
        return ANSC_STATUS_SUCCESS;
    else
        return ANSC_STATUS_FAILURE; 
}

ANSC_STATUS
CosaDmlMTAGetServiceClass
    (
        ANSC_HANDLE                 hContext,
        PULONG                      pulCount,
        PCOSA_MTA_SERVICE_CLASS     *ppCfg
    )
{
    UNREFERENCED_PARAMETER(hContext);
    UNREFERENCED_PARAMETER(ppCfg);
    *pulCount = 0;

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlMTAGetServiceFlow
    (
        ANSC_HANDLE                 hContext,
        PULONG                      pulCount,
        PCOSA_MTA_SERVICE_FLOW      *ppCfg
    )
{
    UNREFERENCED_PARAMETER(hContext);
    *pulCount = 0;
    PMTAMGMT_MTA_SERVICE_FLOW pInfo = NULL;
    if ( mta_hal_GetServiceFlow(pulCount, &pInfo) != RETURN_OK )
    {
         /*Coverity Fix CID:58790 RESOURCE_LEAK */
          if(pInfo != NULL)
          {
             free(pInfo);
              return ANSC_STATUS_FAILURE;
          }
    }

    if (*pulCount > 0) {
        if( (*ppCfg = (PCOSA_MTA_SERVICE_FLOW)AnscAllocateMemory(sizeof(MTAMGMT_MTA_SERVICE_FLOW)*(*pulCount))) == NULL )
        {
            AnscTraceWarning(("AllocateMemory error %s, %d\n", __FUNCTION__, __LINE__));
            free(pInfo);
            return ANSC_STATUS_FAILURE;  
        }
        AnscCopyMemory(*ppCfg, pInfo, sizeof(MTAMGMT_MTA_SERVICE_FLOW)*(*pulCount));
        free(pInfo);
    }
    else
    {
        /* Coverity Fix CID : 58790, RESOURCE_LEAK */
        if(pInfo != NULL)
        {
            free(pInfo);
        }
    }
    return ANSC_STATUS_SUCCESS;

    /*
    if ( mta_hal_GetServiceFlow(pulCount, (PMTAMGMT_MTA_SERVICE_FLOW *)ppCfg) == RETURN_OK )
        return ANSC_STATUS_SUCCESS;
    else
        return ANSC_STATUS_FAILURE;
    */
}

ANSC_STATUS
CosaDmlMTADectGetEnable
    (
        ANSC_HANDLE                 hContext,
        BOOLEAN                     *pBool
    )
{
    UNREFERENCED_PARAMETER(hContext);
    if (mta_hal_DectGetEnable(pBool) == RETURN_OK )
        return ANSC_STATUS_SUCCESS;
    else
        return ANSC_STATUS_FAILURE;
}
        
ANSC_STATUS
CosaDmlMTADectSetEnable
    (
        ANSC_HANDLE                 hContext,
        BOOLEAN                     bBool
    )
{
     UNREFERENCED_PARAMETER(hContext);
     if (mta_hal_DectSetEnable(bBool) == RETURN_OK )
        return ANSC_STATUS_SUCCESS;
    else
        return ANSC_STATUS_FAILURE;

}

ANSC_STATUS
CosaDmlMTADectGetRegistrationMode
    (
        ANSC_HANDLE                 hContext,
        BOOLEAN                     *pBool
    )
{
     UNREFERENCED_PARAMETER(hContext);
     if (mta_hal_DectGetRegistrationMode(pBool) == RETURN_OK )
        return ANSC_STATUS_SUCCESS;
    else
        return ANSC_STATUS_FAILURE;

}

ANSC_STATUS
CosaDmlMTADectSetRegistrationMode
    (
        ANSC_HANDLE                 hContext,
        BOOLEAN                     bBool
    )
{
     UNREFERENCED_PARAMETER(hContext);
     if (mta_hal_DectSetRegistrationMode(bBool) == RETURN_OK )
        return ANSC_STATUS_SUCCESS;
    else
        return ANSC_STATUS_FAILURE;

}

ANSC_STATUS
CosaDmlMTADectRegisterDectHandset
    (
        ANSC_HANDLE                 hContext,
        ULONG                       uValue    
    )
{
    fprintf(stderr, "%s is not implemented!\n", __FUNCTION__);
    UNREFERENCED_PARAMETER(hContext);
    UNREFERENCED_PARAMETER(uValue);
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlMTADectDeregisterDectHandset
    (
        ANSC_HANDLE                 hContext,
        ULONG                       uValue
    )
{

    UNREFERENCED_PARAMETER(hContext);
    if (mta_hal_DectDeregisterDectHandset(uValue) == RETURN_OK )
        return ANSC_STATUS_SUCCESS;
    else
        return ANSC_STATUS_FAILURE;


}

ANSC_STATUS
CosaDmlMTAGetDect
    (
        ANSC_HANDLE                 hContext,
        PCOSA_MTA_DECT              pDect
    )
{

     UNREFERENCED_PARAMETER(hContext);
     if (mta_hal_GetDect((PMTAMGMT_MTA_DECT)pDect) == RETURN_OK )
        return ANSC_STATUS_SUCCESS;
    else
        return ANSC_STATUS_FAILURE;


}

ANSC_STATUS
CosaDmlMTAGetDectPIN
    (
        ANSC_HANDLE                 hContext,
        char                        *pPINString
    )
{

     UNREFERENCED_PARAMETER(hContext);
     if (mta_hal_GetDectPIN(pPINString) == RETURN_OK )
        return ANSC_STATUS_SUCCESS;
     else
        return ANSC_STATUS_FAILURE;


}

ANSC_STATUS
CosaDmlMTASetDectPIN
    (
        ANSC_HANDLE                 hContext,
        char                        *pPINString
    )
{

     UNREFERENCED_PARAMETER(hContext);
     if (mta_hal_SetDectPIN(pPINString) == RETURN_OK )
        return ANSC_STATUS_SUCCESS;
    else
        return ANSC_STATUS_FAILURE;

}

ANSC_STATUS
CosaDmlMTAGetHandsets
    (
        ANSC_HANDLE                 hContext,
        PULONG                      pulCount,
        PCOSA_MTA_HANDSETS_INFO     *ppHandsets
    )
{
    UNREFERENCED_PARAMETER(hContext);
    PMTAMGMT_MTA_HANDSETS_INFO pInfo = NULL;
    if ( mta_hal_GetHandsets( pulCount, &pInfo) != RETURN_OK )
    {
        if(pInfo)
        {
            free(pInfo);
            pInfo = NULL;
        }
        return ANSC_STATUS_FAILURE;
    }
 
    if (*pulCount > 0) {
        if( (*ppHandsets = AnscAllocateMemory(DECT_MAX_HANDSETS * sizeof(MTAMGMT_MTA_HANDSETS_INFO))) == NULL )
        {
            AnscTraceWarning(("AllocateMemory error %s, %d\n", __FUNCTION__, __LINE__));
            free(pInfo);
            return ANSC_STATUS_FAILURE;  
        }
        AnscCopyMemory(*ppHandsets, pInfo, sizeof(MTAMGMT_MTA_HANDSETS_INFO)*DECT_MAX_HANDSETS);
        free(pInfo);
    }
    return ANSC_STATUS_SUCCESS;

}

ANSC_STATUS
CosaDmlMTASetHandsets
    (
        ANSC_HANDLE                 hContext,
        PCOSA_MTA_HANDSETS_INFO     pHandsets
    )
{
   fprintf(stderr, "%s is not implemented!\n", __FUNCTION__);
   UNREFERENCED_PARAMETER(hContext);
   UNREFERENCED_PARAMETER(pHandsets);
   return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlMTAVQMResetStats
    (
        ANSC_HANDLE                 hContext
    )
{
    UNREFERENCED_PARAMETER(hContext);
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlMTAGetCalls
    (
        ANSC_HANDLE                 hContext,
        ULONG                       InstanceNumber, /* LineTable's instance number */
        PULONG                      pulCount,
        PCOSA_MTA_CALLS             *ppCfg
    )
{ 
    UNREFERENCED_PARAMETER(hContext);
    *pulCount = 0;
    PMTAMGMT_MTA_CALLS pInfo = NULL;
    if ( mta_hal_GetCalls(InstanceNumber, pulCount, &pInfo) != RETURN_OK )
    {
        if(pInfo)
        {
            free(pInfo);
            pInfo = NULL;
        }

        return ANSC_STATUS_FAILURE;
    }

    if (*pulCount > 0) {
        if( (*ppCfg = (PCOSA_MTA_CALLS)AnscAllocateMemory(sizeof(MTAMGMT_MTA_CALLS)*(*pulCount))) == NULL )
        {
            AnscTraceWarning(("AllocateMemory error %s, %d\n", __FUNCTION__, __LINE__));
            free(pInfo);
            return ANSC_STATUS_FAILURE;  
        }
        AnscCopyMemory(*ppCfg, pInfo, sizeof(MTAMGMT_MTA_CALLS)*(*pulCount));
        free(pInfo);
    }
    return ANSC_STATUS_SUCCESS;
    
    /*
    if ( mta_hal_GetCalls(InstanceNumber, (PMTAMGMT_MTA_CALLS *)ppCfg, pulCount) == RETURN_OK )
        return ANSC_STATUS_SUCCESS;
    else 
        return ANSC_STATUS_FAILURE;
    */
}

ANSC_STATUS
CosaDmlMTAGetCALLP
    (
        ANSC_HANDLE                 hContext,
        PCOSA_MTA_CAPPL             pCallp
    )
{
    PCOSA_MTA_LINETABLE_INFO pLineTable = (PCOSA_MTA_LINETABLE_INFO)hContext;

    if ( mta_hal_GetCALLP(pLineTable->LineNumber, (PMTAMGMT_MTA_CALLP)pCallp) == RETURN_OK )
        return ANSC_STATUS_SUCCESS;
    else
        return ANSC_STATUS_FAILURE;
}

ANSC_STATUS
CosaDmlMTAGetDSXLogs
    (
        ANSC_HANDLE                 hContext,
        PULONG                      pulCount,
        PCOSA_MTA_DSXLOG            *ppDSXLog
    )
{
    UNREFERENCED_PARAMETER(hContext);
    *pulCount = 0;
    PMTAMGMT_MTA_DSXLOG pInfo = NULL;
    if ( mta_hal_GetDSXLogs(pulCount, &pInfo) != RETURN_OK )
    {

        /* Coverity Fix CID:76397 RESOURCE_LEAK */
        if( pInfo != NULL )
        {
            free(pInfo);
           return ANSC_STATUS_FAILURE;
       }
    }

    if (*pulCount > 0) {
        if( (*ppDSXLog = (PCOSA_MTA_DSXLOG)AnscAllocateMemory(sizeof(MTAMGMT_MTA_DSXLOG)*(*pulCount))) == NULL )
        {
            AnscTraceWarning(("AllocateMemory error %s, %d\n", __FUNCTION__, __LINE__));
             free(pInfo);
            return ANSC_STATUS_FAILURE;  
        }
        /* CID 160180 Dereference after null check fix */
	if(pInfo != NULL)
	{
		AnscCopyMemory(*ppDSXLog, pInfo, sizeof(MTAMGMT_MTA_DSXLOG)*(*pulCount));
		free(pInfo);
	}
    }
    else
    {
        /* Coverity Fix CID : 76397, RESOURCE_LEAK */
        if( pInfo != NULL )
        {
            free(pInfo);
        }
    }
    return ANSC_STATUS_SUCCESS;

    /*
    if ( mta_hal_GetDSXLogs(pulCount, (PMTAMGMT_MTA_DSXLOG *)ppDSXLog) == RETURN_OK )
        return ANSC_STATUS_SUCCESS;
    else
        return ANSC_STATUS_FAILURE;
    */
}

ANSC_STATUS
CosaDmlMTAGetDSXLogEnable
    (
        ANSC_HANDLE                 hContext,
        BOOLEAN                     *pBool
    )
{
    UNREFERENCED_PARAMETER(hContext);
    if ( mta_hal_GetDSXLogEnable(pBool) == RETURN_OK ) 
        return ANSC_STATUS_SUCCESS;
    else
        return ANSC_STATUS_FAILURE;
}

ANSC_STATUS
CosaDmlMTASetDSXLogEnable
    (
        ANSC_HANDLE                 hContext,
        BOOLEAN                     Bool
    )
{
    UNREFERENCED_PARAMETER(hContext);
    if ( mta_hal_SetDSXLogEnable(Bool) == RETURN_OK ) 
        return ANSC_STATUS_SUCCESS;
    else
        return ANSC_STATUS_FAILURE;
}

ANSC_STATUS
CosaDmlMTAClearDSXLog
    (
        ANSC_HANDLE                 hContext,
        BOOLEAN                     ClearLog
    )
{
    UNREFERENCED_PARAMETER(hContext);
    if ( mta_hal_ClearDSXLog(ClearLog) == RETURN_OK ) 
        return ANSC_STATUS_SUCCESS;
    else
        return ANSC_STATUS_FAILURE;
}

// CallSignalling apis
ANSC_STATUS
CosaDmlMTAGetCallSignallingLogEnable
	(
        ANSC_HANDLE                 hContext,
        BOOLEAN                     *pBool
    )
{
    UNREFERENCED_PARAMETER(hContext);
    if ( mta_hal_GetCallSignallingLogEnable(pBool) == RETURN_OK ) 
        return ANSC_STATUS_SUCCESS;
    else
        return ANSC_STATUS_FAILURE;
}

ANSC_STATUS
CosaDmlMTASetCallSignallingLogEnable
    (
        ANSC_HANDLE                 hContext,
        BOOLEAN                     Bool
    )
{
    UNREFERENCED_PARAMETER(hContext);
    if ( mta_hal_SetCallSignallingLogEnable(Bool) == RETURN_OK ) 
        return ANSC_STATUS_SUCCESS;
    else
        return ANSC_STATUS_FAILURE;
}

ANSC_STATUS
CosaDmlMTAClearCallSignallingLog
    (
        ANSC_HANDLE                 hContext,
	BOOLEAN                     ClearLog
    )
{
    UNREFERENCED_PARAMETER(hContext);
    if ( mta_hal_ClearCallSignallingLog(ClearLog) == RETURN_OK ) 
        return ANSC_STATUS_SUCCESS;
    else
        return ANSC_STATUS_FAILURE;
}

// CallSignalling apis ends

ANSC_STATUS
CosaDmlMtaBatteryGetInstalled
    (
        ANSC_HANDLE                 hContext,
        PBOOL                       pValue
    )
{
    UNREFERENCED_PARAMETER(hContext);
    if ( mta_hal_BatteryGetInstalled(pValue) == RETURN_OK )
        return ANSC_STATUS_SUCCESS;
    else
        return ANSC_STATUS_FAILURE;
}

ANSC_STATUS
CosaDmlMtaBatteryGetTotalCapacity
    (
        ANSC_HANDLE                 hContext,
        PULONG                      pValue
    )
{
    UNREFERENCED_PARAMETER(hContext);
    if ( mta_hal_BatteryGetTotalCapacity(pValue) == RETURN_OK )
        return ANSC_STATUS_SUCCESS;
    else
        return ANSC_STATUS_FAILURE;
}
    
ANSC_STATUS
CosaDmlMtaBatteryGetActualCapacity
    (
        ANSC_HANDLE                 hContext,
        PULONG                      pValue
    )
{
    UNREFERENCED_PARAMETER(hContext);
    if ( mta_hal_BatteryGetActualCapacity(pValue) == RETURN_OK )
        return ANSC_STATUS_SUCCESS;
    else
        return ANSC_STATUS_FAILURE;
}

ANSC_STATUS
CosaDmlMtaBatteryGetRemainingCharge
    (
        ANSC_HANDLE                 hContext,
        PULONG                      pValue
    )
{
    UNREFERENCED_PARAMETER(hContext);
    if ( mta_hal_BatteryGetRemainingCharge(pValue) == RETURN_OK )
        return ANSC_STATUS_SUCCESS;
    else
        return ANSC_STATUS_FAILURE;
}

ANSC_STATUS
CosaDmlMtaBatteryGetRemainingTime
    (
        ANSC_HANDLE                 hContext,
        PULONG                      pValue
    )
{
    UNREFERENCED_PARAMETER(hContext);
    if ( mta_hal_BatteryGetRemainingTime(pValue) == RETURN_OK )
        return ANSC_STATUS_SUCCESS;
    else
        return ANSC_STATUS_FAILURE;
}

ANSC_STATUS
CosaDmlMtaBatteryGetNumberofCycles
    (
        ANSC_HANDLE                 hContext,
        PULONG                      pValue
    )
{
    UNREFERENCED_PARAMETER(hContext);
    if ( mta_hal_BatteryGetNumberofCycles(pValue) == RETURN_OK )
        return ANSC_STATUS_SUCCESS;
    else
        return ANSC_STATUS_FAILURE;
}

ANSC_STATUS
CosaDmlMtaBatteryGetPowerStatus
    (
        ANSC_HANDLE                 hContext,
        PCHAR                       pValue,
        PULONG                      pSize
    )
{
    UNREFERENCED_PARAMETER(hContext);
    if ( mta_hal_BatteryGetPowerStatus(pValue, pSize) == RETURN_OK )
        return ANSC_STATUS_SUCCESS;
    else
        return ANSC_STATUS_FAILURE;
}

ANSC_STATUS
CosaDmlMtaBatteryGetCondition
    (
        ANSC_HANDLE                 hContext,
        PCHAR                       pValue,
        PULONG                      pSize
    )
{
    UNREFERENCED_PARAMETER(hContext);
    if ( mta_hal_BatteryGetCondition(pValue, pSize) == RETURN_OK )
        return ANSC_STATUS_SUCCESS;
    else
        return ANSC_STATUS_FAILURE;
}

ANSC_STATUS
CosaDmlMtaBatteryGetStatus
    (
        ANSC_HANDLE                 hContext,
        PCHAR                       pValue,
        PULONG                      pSize
    )
{
    UNREFERENCED_PARAMETER(hContext);
    if ( mta_hal_BatteryGetStatus(pValue, pSize) == RETURN_OK )
        return ANSC_STATUS_SUCCESS;
    else
        return ANSC_STATUS_FAILURE;
}

ANSC_STATUS
CosaDmlMtaBatteryGetLife
    (
        ANSC_HANDLE                 hContext,
        PCHAR                       pValue,
        PULONG                      pSize
    )
{
    UNREFERENCED_PARAMETER(hContext);
    if ( mta_hal_BatteryGetLife(pValue, pSize) == RETURN_OK )
        return ANSC_STATUS_SUCCESS;
    else
        return ANSC_STATUS_FAILURE;
}

ANSC_STATUS
CosaDmlMtaBatteryGetInfo
    (
        ANSC_HANDLE                 hContext,
        PCOSA_DML_BATTERY_INFO      pInfo
    )
{
    UNREFERENCED_PARAMETER(hContext);
    if ( mta_hal_BatteryGetInfo((PMTAMGMT_MTA_BATTERY_INFO)pInfo) == RETURN_OK )
        return ANSC_STATUS_SUCCESS;
    else 
        return ANSC_STATUS_FAILURE;
}


ANSC_STATUS
CosaDmlMtaGetMtaLog
    (
        ANSC_HANDLE                 hContext,
        PULONG                      pulCount,
        PCOSA_DML_MTALOG_FULL       *ppConf        
    )    
{
    UNREFERENCED_PARAMETER(hContext);
    *pulCount = 0;
    PMTAMGMT_MTA_MTALOG_FULL pInfo = NULL;
    if ( mta_hal_GetMtaLog(pulCount, &pInfo) != RETURN_OK )
    {
        /* Coverity Fix CID:79698 RESOURCE_LEAK */
        if(pInfo != NULL)
        {     free(pInfo);
          return ANSC_STATUS_FAILURE;
        }
    }

    if (*pulCount > 0) {
        if( (*ppConf = (PCOSA_DML_MTALOG_FULL)AnscAllocateMemory(sizeof(MTAMGMT_MTA_MTALOG_FULL)*(*pulCount))) == NULL )
        {
            AnscTraceWarning(("AllocateMemory error %s, %d\n", __FUNCTION__, __LINE__));
            /* Coverity Fix CID:79698 RESOURCE_LEAK */
            free(pInfo);
            return ANSC_STATUS_FAILURE;  
        }
        AnscCopyMemory(*ppConf, pInfo, sizeof(MTAMGMT_MTA_MTALOG_FULL)*(*pulCount));
        {
            unsigned int i;
            for (i=0; i<*pulCount; i++) {
                if (pInfo[i].pDescription) {
                    (*ppConf)[i].pDescription = AnscCloneString(pInfo[i].pDescription);
                    free(pInfo[i].pDescription);
                }
                else (*ppConf)[i].pDescription = NULL;
            }
        }
        free(pInfo);
    }
    else
    {
        /* Coverity Fix CID : 79698 RESOURCE_LEAK */
        if(pInfo != NULL)
        {     
            free(pInfo);
        }
    }
    return ANSC_STATUS_SUCCESS;

/*
    if ( mta_hal_GetMtaLog(pulCount, (PMTAMGMT_MTA_MTALOG_FULL *)ppConf) == RETURN_OK )
        return ANSC_STATUS_SUCCESS;
    else
        return ANSC_STATUS_FAILURE;
*/

}

ANSC_STATUS
CosaDmlMtaGetDectLog
    (
        ANSC_HANDLE                 hContext,
        PULONG                      pulCount,
        PCOSA_DML_DECTLOG_FULL      *ppConf        
    )    
{
    UNREFERENCED_PARAMETER(hContext);
    UNREFERENCED_PARAMETER(ppConf);

    *pulCount = 0;
    //*ppConf = (PCOSA_DML_DECTLOG_FULL)AnscAllocateMemory( sizeof(DectLog) );

    //AnscCopyMemory(*ppConf, &DectLog, sizeof(DectLog) );

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlMtaGetResetCount
    (
        ANSC_HANDLE                 hContext,
	MTA_RESET_TYPE              type,
        ULONG                       *pValue
    )
{
	UNREFERENCED_PARAMETER(hContext);
        switch(type)
	{
		case MTA_RESET:{
				mta_hal_Get_MTAResetCount(pValue);
			 }
			break;
		case LINE_RESET:{
				mta_hal_Get_LineResetCount(pValue);
			}
			break;
		default:{
			 AnscTraceWarning(("Invalide type %s, %d\n", __FUNCTION__, __LINE__));
			}
	}
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlMtaClearCalls
    (
		ULONG InstanceNumber
    )
{
	mta_hal_ClearCalls( InstanceNumber );

    return ANSC_STATUS_SUCCESS;
}
// #endif /* CONFIG_TI_PACM */

ANSC_STATUS
CosaDmlMtaGetDhcpStatus
    (
		PULONG pIpv4DhcpStatus,
		PULONG pIpv6DhcpStatus
    )
{
	*pIpv4DhcpStatus = MTA_ERROR;
	*pIpv6DhcpStatus = MTA_ERROR;

#ifdef _CBR_PRODUCT_REQ_
	MTAMGMT_MTA_STATUS output_pIpv4status = MTA_ERROR;
	MTAMGMT_MTA_STATUS output_pIpv6status = MTA_ERROR;
        if ( RETURN_OK == mta_hal_getDhcpStatus( &output_pIpv4status, &output_pIpv6status ) )
	{
		*pIpv4DhcpStatus = output_pIpv4status;
		*pIpv6DhcpStatus = output_pIpv6status;
	}
#endif /* _CBR_PRODUCT_REQ_ */

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlMtaGetConfigFileStatus
    (
		PULONG pConfigFileStatus
    )
{
        UNREFERENCED_PARAMETER(pConfigFileStatus);
#ifdef _CBR_PRODUCT_REQ_
     
         /*Coverity Fix CID 59560 */
         MTAMGMT_MTA_STATUS output_status = MTA_ERROR;
	if ( RETURN_OK == mta_hal_getConfigFileStatus( &output_status))
	{
		*pConfigFileStatus = output_status;
	}
#endif /* _CBR_PRODUCT_REQ_ */

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlMtaGetLineRegisterStatus
    (
		PCHAR pcLineRegisterStatus
    )
{
#ifdef _CBR_PRODUCT_REQ_
#define max_number_of_line 8
	MTAMGMT_MTA_STATUS output_status_array[ max_number_of_line ] = { 0 };
	char 			   aoutput_status_array_string [ 4 ][ 32 ] = { "Init", "Start", "Complete", "Error" };

	if ( RETURN_OK == mta_hal_getLineRegisterStatus( output_status_array, max_number_of_line ) )
	{
		/* CID 91923 Calling risky function fix */
		snprintf( pcLineRegisterStatus, MAX_LINE_REG, "%s,%s,%s,%s,%s,%s,%s,%s", aoutput_status_array_string[ output_status_array[ 0 ] ],
																  aoutput_status_array_string[ output_status_array[ 1 ] ],
																  aoutput_status_array_string[ output_status_array[ 2 ] ],
																  aoutput_status_array_string[ output_status_array[ 3 ] ],
																  aoutput_status_array_string[ output_status_array[ 4 ] ],
																  aoutput_status_array_string[ output_status_array[ 5 ] ],
																  aoutput_status_array_string[ output_status_array[ 6 ] ],
																  aoutput_status_array_string[ output_status_array[ 7 ] ] );
	}
	else
#endif /* _CBR_PRODUCT_REQ_ */
	{	
		/* CID 91923 Calling risky function fix */
		snprintf( pcLineRegisterStatus, MAX_LINE_REG,"%s", "" );
	}

    return ANSC_STATUS_SUCCESS;
}


ANSC_STATUS
CosaDmlMTASetStartUpIpMode
    (
        PCOSA_MTA_ETHWAN_PROV_INFO  pmtaethpro, 
        INT                         bInt
    )
{
     pmtaethpro->StartupIPMode.ActiveValue =bInt;
     return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlMTASetPrimaryDhcpServerOptions
    (
        PCOSA_MTA_ETHWAN_PROV_INFO  pmtaethpro,
        char                       *buf,
        MTA_IP_TYPE_TR              type
    )
{
    if (type == MTA_IPV4_TR)
    {
        snprintf(pmtaethpro->IPv4PrimaryDhcpServerOptions.ActiveValue, sizeof(pmtaethpro->IPv4PrimaryDhcpServerOptions.ActiveValue), "%s", buf);
    }
    else if (type == MTA_IPV6_TR)
    {
        snprintf(pmtaethpro->IPv6PrimaryDhcpServerOptions.ActiveValue, sizeof(pmtaethpro->IPv6PrimaryDhcpServerOptions.ActiveValue), "%s", buf);
    }

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlMTASetSecondaryDhcpServerOptions
    (
        PCOSA_MTA_ETHWAN_PROV_INFO  pmtaethpro,
        char                       *buf,
        MTA_IP_TYPE_TR              type
    )
{
    if (type == MTA_IPV4_TR)
    {
        snprintf(pmtaethpro->IPv4SecondaryDhcpServerOptions.ActiveValue, sizeof(pmtaethpro->IPv4SecondaryDhcpServerOptions.ActiveValue), "%s", buf);
    }
    else if (type == MTA_IPV6_TR)
    {
        snprintf(pmtaethpro->IPv6SecondaryDhcpServerOptions.ActiveValue, sizeof(pmtaethpro->IPv6SecondaryDhcpServerOptions.ActiveValue), "%s", buf);
    }

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlMtaResetNow
    (
		BOOLEAN	 bValue
    )
{
        UNREFERENCED_PARAMETER(bValue);
#if defined (_XB6_PRODUCT_REQ_) || defined (_CBR_PRODUCT_REQ_)
	if( RETURN_OK == mta_hal_devResetNow(bValue) )
#else
    if( 1 )
#endif /* _CBR_PRODUCT_REQ_ || _XB6_PRODUCT_REQ_*/
	{
		AnscTraceWarning(("MTA reset is successful \n"));
		return ANSC_STATUS_SUCCESS;
	}
	else
	{
		return ANSC_STATUS_FAILURE;
	}
}

/*Coverity fix CID 56740 argument type mismatch */
void * MtaProvisioningStatusGetFunc(void * arg)
{
	pthread_detach(pthread_self());
        CcspTraceInfo(("%s Entering \n",__FUNCTION__));

#if defined (_XB6_PRODUCT_REQ_) || defined (_CBR_PRODUCT_REQ_)
        /*Coverity Fix CID 62657 55626 */
        unsigned int ProvisioningStatus;
        int counter=0;
        char value[16] = {'\0'};
        bool tr104ApplySuccess = false;
        bool tr104Enable = false;
        MTAMGMT_MTA_PROVISION_STATUS provisionStatus = 0;
        char MTA_provision_status[2][20] = {"MTA_PROVISIONED", "MTA_NON_PROVISIONED"};
        int tr181_sysevent_fd;
        token_t tr181_sysevent_token;

        tr181_sysevent_fd = sysevent_open("127.0.0.1", SE_SERVER_WELL_KNOWN_PORT, SE_VERSION, "TR104_Apply", &tr181_sysevent_token);
        if (tr181_sysevent_fd < 0){
            CcspTraceError(("%s sysevent_open fail \n",__FUNCTION__));
            return NULL;
        }

        //Retreive the operating status only when the MTA provisioning status 
        //to non provisioning.

        do 
        {
               if( RETURN_OK != mta_hal_getMtaProvisioningStatus(&provisionStatus) ) 
	        {
	            CcspTraceInfo((" %s: Mta_halgetProvisioningstatus Fail \n ",__FUNCTION__));

	        }
                // True when MTA_NON_PROVISIONED and provisioning status value is returned.
                else if( strcmp(MTA_provision_status[provisionStatus],"MTA_PROVISIONED") ) 
	        {
	            CcspTraceInfo(("%s provisionStatus = %s \n",__FUNCTION__,MTA_provision_status[provisionStatus]));
	            break;
	        }
                sleep(5);
                counter = counter + 5;
	} while (counter < 240);
    
        if((syscfg_get(NULL,"TR104enable", value, sizeof(value)) == 0) && (strcmp(value, "true") == 0))
	{
	      tr104Enable = true;
	}
        CcspTraceDebug((" %s: tr104Enable: %s \n",__FUNCTION__, value ));
        memset_s(value,sizeof(value),0,sizeof(value));

        if (sysevent_get(tr181_sysevent_fd, tr181_sysevent_token, "tr104_applied", value, sizeof(value)) == 0)
        {
             tr104ApplySuccess = (strcmp(value, "true") == 0);
             CcspTraceDebug((" %s: tr104_applied: %s tr104ApplySuccess=%d \n",__FUNCTION__, value ,tr104ApplySuccess ));
        }
        else
        {
             CcspTraceDebug(("%s:%d tr104 not applied :value =%s \n",__FUNCTION__,__LINE__,value));
             tr104ApplySuccess = false;
        }
        CcspTraceInfo((" %s: tr104Enable: %s tr104ApplySuccess: %s \n",__FUNCTION__, tr104Enable ?"true":"false" ,tr104ApplySuccess ?"true":"false" ));
        
        
        if (0 <= tr181_sysevent_fd) {
            sysevent_close(tr181_sysevent_fd, tr181_sysevent_token);
            tr181_sysevent_fd = -1;
        }
        tr181_sysevent_token = 0;
  
	while(1)
	{
		if(RETURN_OK != mta_hal_getMtaOperationalStatus(&ProvisioningStatus) )
		{
			CcspTraceDebug((" %s: mta_hal_getMtaOperationalStatus Fail \n",__FUNCTION__));
			return NULL;
		}

		switch(ProvisioningStatus)
		{
			case COSA_MTA_INIT:
				CcspTraceDebug(("MTA init after reset  \n"));
				break;

			case COSA_MTA_START:
				CcspTraceDebug(("MTA provisiong started after reset \n"));
				break;


			case COSA_MTA_ERROR:
				CcspTraceDebug(("MTA provisioning failed after reset"));
				break;

			//Apply TR104 config if any of both the lines are Enabled / both the lines disabled
 
			case COSA_MTA_COMPLETE:
			case COSA_MTA_REJECTED:
				CcspTraceInfo((" %s:MTA is REjected/completed after reset \n",__FUNCTION__));
                        //ReApply TR104 config after reset only if MTA is TR104 capable and WC already applied before
                    
				if( tr104Enable && tr104ApplySuccess) 
                		{
#ifdef MTA_TR104SUPPORT                                       
                                	mtaReapplytr104Conf();
#endif // MTA_TR104SUPPORT
                    			CcspTraceInfo(("%s: TR104 is Enabled,Applying TR104 config. \n",__FUNCTION__));
               			}		  
				pthread_exit(NULL);
				return arg;


			default :
					break;
		}
		sleep(5);
		counter = counter+5;

		/*This thread is run for 180Sec*/
		if(counter == 180)
		{
			counter = 0;
			pthread_exit(NULL);
			return arg;
		}
        }
        
        /* Coverity Fix CID : 560416 Structurally dead code */
        //pthread_exit(NULL);
  
#endif /* _CBR_PRODUCT_REQ_ || _XB6_PRODUCT_REQ_*/
        CcspTraceInfo(("%s Exiting \n",__FUNCTION__));
        return arg;    
}



void CosaDmlMtaProvisioningStatusGet()
{
        int res = 0;
        pthread_t MtaProvisioningStatusGetThread;
        
        res = pthread_create(&MtaProvisioningStatusGetThread, NULL, MtaProvisioningStatusGetFunc, "MtaProvisioningStatusGetFunc");
	if(res != 0)
	{
	    CcspTraceError(("Create MtaProvisioningStatusGetThread error %d\n", res));
	    return;
	}

}

#define PARTNER_ID_LEN 64
ANSC_STATUS fillCurrentPartnerId
        (
                char*                       pValue,
        PULONG                      pulSize
    )
{
        char buf[PARTNER_ID_LEN] = {'\0'} ;
    	if(ANSC_STATUS_SUCCESS == syscfg_get( NULL, "PartnerID", buf, sizeof(buf)))
    	{
	    	/* CID 66248 Array compared against NULL fix */
                if( buf[0] != '\0' )
                {
                        /* Coverity Fix : STRING_NULL */
                        size_t n = strlen(buf);
                        snprintf(pValue, n + 1, "%s", buf);
                        *pulSize = AnscSizeOfString(pValue);
                        return ANSC_STATUS_SUCCESS;
                }
		/* CID 56187 Logically dead code fix with CID 66248 */		
                else
                        return ANSC_STATUS_FAILURE;
    	}
        else
                return ANSC_STATUS_FAILURE;

}

void FillParamUpdateSource(cJSON *partnerObj, char *key, char *paramUpdateSource)
{
    cJSON *paramObj = cJSON_GetObjectItem( partnerObj, key);
    if ( paramObj != NULL )
    {
        char *valuestr = NULL;
        cJSON *paramObjVal = cJSON_GetObjectItem(paramObj, "UpdateSource");
        if (paramObjVal)
            valuestr = paramObjVal->valuestring;
        if (valuestr != NULL)
        {
            AnscCopyString(paramUpdateSource, valuestr);
            valuestr = NULL;
        }
        else
        {
            CcspTraceWarning(("%s - %s UpdateSource is NULL\n", __FUNCTION__, key ));
        }
    }
    else
    {
        CcspTraceWarning(("%s - %s Object is NULL\n", __FUNCTION__, key ));
    }
}

void FillMissingSyscfgParam(cJSON *partnerObj, char *key, char *syscfgParam)
{
    int ret;
    char buf[64] = {0};

    ret = syscfg_get( NULL, syscfgParam, buf, sizeof(buf));

    if( ret != 0 || buf[0] == '\0' )
    {
	cJSON *paramObj = cJSON_GetObjectItem( partnerObj, key);
	if ( paramObj != NULL )
	{
	    char *valuestr = NULL;
	    cJSON *paramObjVal = cJSON_GetObjectItem(paramObj, "ActiveValue");
	    if (paramObjVal)
		valuestr = paramObjVal->valuestring;
	    if (valuestr != NULL)
	    {
		if (syscfg_set_commit(NULL, syscfgParam, valuestr) != 0)
		{
		    CcspTraceError(("%s - syscfg_set %s failed\n", __FUNCTION__,syscfgParam));
		}
	    }
	    else
	    {
		CcspTraceWarning(("%s - %s ActiveValue is NULL\n", __FUNCTION__, key ));
	    }
	}
	else
	{
	    CcspTraceWarning(("%s - %s Object is NULL\n", __FUNCTION__, key ));
	}
    }
}

void FillPartnerIDJournal
    (
        cJSON *json ,
        char *partnerID ,
        PCOSA_MTA_ETHWAN_PROV_INFO  pmtaethpro
    )
{
                cJSON *partnerObj = cJSON_GetObjectItem( json, partnerID );
                if( partnerObj != NULL)
                {     /*Coverity Fix CID 60029 70533 72638 53440 58371 Incompatible argument type */
                      FillParamUpdateSource(partnerObj, "Device.X_RDKCENTRAL-COM_EthernetWAN_MTA.StartupIPMode", pmtaethpro->StartupIPMode.UpdateSource);
                      FillParamUpdateSource(partnerObj, "Device.X_RDKCENTRAL-COM_EthernetWAN_MTA.IPv4PrimaryDhcpServerOptions", pmtaethpro->IPv4PrimaryDhcpServerOptions.UpdateSource);
                      FillParamUpdateSource(partnerObj, "Device.X_RDKCENTRAL-COM_EthernetWAN_MTA.IPv4SecondaryDhcpServerOptions", pmtaethpro->IPv4SecondaryDhcpServerOptions.UpdateSource);
                      FillParamUpdateSource(partnerObj, "Device.X_RDKCENTRAL-COM_EthernetWAN_MTA.IPv6PrimaryDhcpServerOptions", pmtaethpro->IPv6PrimaryDhcpServerOptions.UpdateSource);
                      FillParamUpdateSource(partnerObj, "Device.X_RDKCENTRAL-COM_EthernetWAN_MTA.IPv6SecondaryDhcpServerOptions", pmtaethpro->IPv6SecondaryDhcpServerOptions.UpdateSource);

		      //Check if syscfg parameters are missing in db. If so update the syscfg parameter based on ActiveValue from bootstrap.json
		      FillMissingSyscfgParam(partnerObj, "Device.X_RDKCENTRAL-COM_EthernetWAN_MTA.StartupIPMode","StartupIPMode");
		      FillMissingSyscfgParam(partnerObj, "Device.X_RDKCENTRAL-COM_EthernetWAN_MTA.IPv4PrimaryDhcpServerOptions","IPv4PrimaryDhcpServerOptions");
		      FillMissingSyscfgParam(partnerObj, "Device.X_RDKCENTRAL-COM_EthernetWAN_MTA.IPv4SecondaryDhcpServerOptions","IPv4SecondaryDhcpServerOptions");
		      FillMissingSyscfgParam(partnerObj, "Device.X_RDKCENTRAL-COM_EthernetWAN_MTA.IPv6PrimaryDhcpServerOptions","IPv6PrimaryDhcpServerOptions");
		      FillMissingSyscfgParam(partnerObj, "Device.X_RDKCENTRAL-COM_EthernetWAN_MTA.IPv6SecondaryDhcpServerOptions","IPv6SecondaryDhcpServerOptions");
                }
                else
                {
                      CcspTraceWarning(("%s - PARTNER ID OBJECT Value is NULL\n", __FUNCTION__ ));
                }
}

//Get the UpdateSource info from /opt/secure/bootstrap.json. This is needed to know for override precedence rules in set handlers
ANSC_STATUS
CosaMTAInitializeEthWanProvJournal
    (
        PCOSA_MTA_ETHWAN_PROV_INFO  pmtaethpro
    )
{
        char *data = NULL;
        cJSON *json = NULL;
        FILE *fileRead = NULL;
        char PartnerID[PARTNER_ID_LEN] = {0};
        ULONG size = PARTNER_ID_LEN - 1;
        int len =0;
        if (!pmtaethpro)
        {
                CcspTraceWarning(("%s-%d : NULL param\n" , __FUNCTION__, __LINE__ ));
                return ANSC_STATUS_FAILURE;
        }

         fileRead = fopen( BOOTSTRAP_INFO_FILE, "r" );
         if( fileRead == NULL )
         {
                 CcspTraceWarning(("%s-%d : Error in opening JSON file\n" , __FUNCTION__, __LINE__ ));
                 return ANSC_STATUS_FAILURE;
         }

         fseek( fileRead, 0, SEEK_END );
         len = ftell( fileRead );
	 /* Coverity Issue Fix - CID:74334 : Negative Returns */
	 if (len < 0)
	 {
		CcspTraceWarning(("%s-%d : Error finding size of JSON file\n" , __FUNCTION__, __LINE__ ));
                fclose(fileRead);
                 return ANSC_STATUS_FAILURE;
         }
	 fseek( fileRead, 0, SEEK_SET );
         data = ( char* )malloc( sizeof(char) * (len + 1) );
         if (data != NULL)
         {
                memset( data, 0, ( sizeof(char) * (len + 1) ));
               /*Coverity CID Fix :60325 CHECKED_RETURN */
               if( fread( data, 1, len, fileRead ) < (len*sizeof(char)) )
                     CcspTraceWarning(("%s-%d : fread  failed \n", __FUNCTION__, __LINE__));
	       /*CID :135523 String not null terminated (STRING_NULL) */
               data[len] = '\0';
			
         }
         else
         {
                 CcspTraceWarning(("%s-%d : Memory allocation failed \n", __FUNCTION__, __LINE__));
                 fclose( fileRead );
                 return ANSC_STATUS_FAILURE;
         }

         fclose( fileRead );

         if ( data == NULL )
         {
                CcspTraceWarning(("%s-%d : fileRead failed \n", __FUNCTION__, __LINE__));
                return ANSC_STATUS_FAILURE;
         }
	 /* CID 135523 String not null terminated fix */
         else if ( strnlen(data,len) != 0)
         {
                 json = cJSON_Parse( data );
                 if( !json )
                 {
                         CcspTraceWarning((  "%s : json file parser error : [%d]\n", __FUNCTION__,__LINE__));
                         free(data);
                         return ANSC_STATUS_FAILURE;
                 }
                 else
                 {
                         if(ANSC_STATUS_SUCCESS == fillCurrentPartnerId(PartnerID, &size))
                         {
                                if ( PartnerID[0] != '\0' )
                                {
                                        CcspTraceWarning(("%s : Partner = %s \n", __FUNCTION__, PartnerID));
                                        FillPartnerIDJournal(json, PartnerID, pmtaethpro);
                                }
                                else
                                {
                                        CcspTraceWarning(( "Reading Deafult PartnerID Values \n" ));
                                        /*Coverity Fix CID:67642 DC.STRING_BUFFER */
                                        strncpy(PartnerID, "comcast",sizeof(PartnerID)-1);
                                        FillPartnerIDJournal(json, PartnerID, pmtaethpro);
                                }
                        }
                        else{
                                CcspTraceWarning(("Failed to get Partner ID\n"));
                        }
                        cJSON_Delete(json);
                }
                free(data);
               
         }
         else
         {
                CcspTraceWarning(("BOOTSTRAP_INFO_FILE %s is empty\n", BOOTSTRAP_INFO_FILE));
                /* Coverity Fix CID: 58665 RESOURCE_LEAK */
                free(data);
                return ANSC_STATUS_FAILURE;
         }
         return ANSC_STATUS_SUCCESS;
}

static int writeToJson(char *data, char *file)
{
    FILE *fp;
    fp = fopen(file, "w");
    if (fp == NULL)
    {
        CcspTraceWarning(("%s : %d Failed to open file %s\n", __FUNCTION__,__LINE__,file));
        return -1;
    }

    fwrite(data, strlen(data), 1, fp);
    fclose(fp);
    return 0;
}

ANSC_STATUS UpdateJsonParamLegacy
	(
		char*                       pKey,
		char*			PartnerId,
		char*			pValue
    )
{
	cJSON *partnerObj = NULL;
	cJSON *json = NULL;
	FILE *fileRead = NULL;
	char * cJsonOut = NULL;
	char* data = NULL;
	 int len  = 0;
	 int configUpdateStatus = -1;
	 fileRead = fopen( PARTNERS_INFO_FILE, "r" );
	 if( fileRead == NULL ) 
	 {
		 CcspTraceWarning(("%s-%d : Error in opening JSON file\n" , __FUNCTION__, __LINE__ ));
		 return ANSC_STATUS_FAILURE;
	 }
	 
	 fseek( fileRead, 0, SEEK_END );
	 len = ftell( fileRead );
	 /* Coverity Issue Fix - CID:74334 : Negative Returns*/
	 if (len < 0)
	 {
		CcspTraceWarning(("%s-%d : Error finding size of JSON file\n" , __FUNCTION__, __LINE__ ));
                 fclose(fileRead);
                 return ANSC_STATUS_FAILURE;
         }
	 fseek( fileRead, 0, SEEK_SET );
	 data = ( char* )malloc( sizeof(char) * (len + 1) );
	 if (data != NULL) 
	 {
		memset( data, 0, ( sizeof(char) * (len + 1) ));
	 	if( fread( data, sizeof(char), len, fileRead ) < (len*sizeof(char)) )
                     CcspTraceWarning(("%s-%d : File read failed \n", __FUNCTION__, __LINE__));
	 } 
	 else 
	 {
		 CcspTraceWarning(("%s-%d : Memory allocation failed \n", __FUNCTION__, __LINE__));
		 fclose( fileRead );
		 return ANSC_STATUS_FAILURE;
	 }
	 
	 fclose( fileRead );
	 if ( data == NULL )
	 {
		CcspTraceWarning(("%s-%d : fileRead failed \n", __FUNCTION__, __LINE__));
		return ANSC_STATUS_FAILURE;
	 }
	 /* CID 135238 String not null terminated fix */
	 else if ( strnlen(data,len) != 0)
	 {
		 json = cJSON_Parse( data );
		 if( !json ) 
		 {
			 CcspTraceWarning((  "%s : json file parser error : [%d]\n", __FUNCTION__,__LINE__));
			 free(data);
			 return ANSC_STATUS_FAILURE;
		 } 
		 else
		 {
			 partnerObj = cJSON_GetObjectItem( json, PartnerId );
			 if ( NULL != partnerObj)
			 {
				 if (NULL != cJSON_GetObjectItem( partnerObj, pKey) )
				 {
					 cJSON_ReplaceItemInObject(partnerObj, pKey, cJSON_CreateString(pValue));
					 cJsonOut = cJSON_Print(json);
                     if(cJsonOut)
                     {
                        CcspTraceWarning(( "Updated json content is %s\n", cJsonOut));
                        configUpdateStatus = writeToJson(cJsonOut, PARTNERS_INFO_FILE);
                        free(cJsonOut);
                     }
					 if ( !configUpdateStatus)
					 {
						 CcspTraceWarning(( "Updated Value for %s partner\n",PartnerId));
						 CcspTraceWarning(( "Param:%s - Value:%s\n",pKey,pValue));
					 }
					 else
				 	{
						 CcspTraceWarning(( "Failed to update value for %s partner\n",PartnerId));
						 CcspTraceWarning(( "Param:%s\n",pKey));
			 			 cJSON_Delete(json);
                         /* Coverity Fix CID : 65542 Resource leak */
                         if (data)
                         {
			                 free(data);
                         }
						 return ANSC_STATUS_FAILURE;						
				 	}
				 }
				else
			 	{
			 		CcspTraceWarning(("%s - OBJECT  Value is NULL %s\n", pKey,__FUNCTION__ ));
			 		cJSON_Delete(json);
                    /* Coverity Fix CID : 65542 Resource leak */
                    if (data)
                    {
                        free(data);
                    }
			 		return ANSC_STATUS_FAILURE;
			 	}
			 
			 }
			 else
			 {
			 	CcspTraceWarning(("%s - PARTNER ID OBJECT Value is NULL\n", __FUNCTION__ ));
			 	cJSON_Delete(json);
                /* Coverity Fix CID : 65542 Resource leak */
                if (data)
                {
                    free(data);
                }
			 	return ANSC_STATUS_FAILURE;
			 }
			cJSON_Delete(json);
            /* Coverity Fix CID : 65542 Resource leak */
            if (data)
            {
               free(data);
            }
		 }
	  }
	  else
	  {
		CcspTraceWarning(("PARTNERS_INFO_FILE %s is empty\n", PARTNERS_INFO_FILE));
        /* Coverity Fix CID : 65542 Resource leak */
        if (data)
        {
            free(data);
        }
		return ANSC_STATUS_FAILURE;
	  }
	 return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS UpdateJsonParam
        (
                char*                       pKey,
                char*                   PartnerId,
                char*                   pValue,
                char*                   pSource,
                char*                   pCurrentTime
    )
{
        cJSON *partnerObj = NULL;
        cJSON *json = NULL;
        FILE *fileRead = NULL;
        char * cJsonOut = NULL;
        char* data = NULL;
         int len = 0;
         int configUpdateStatus = -1;
         fileRead = fopen( BOOTSTRAP_INFO_FILE, "r" );
         if( fileRead == NULL )
         {
                 CcspTraceWarning(("%s-%d : Error in opening JSON file\n" , __FUNCTION__, __LINE__ ));
                 return ANSC_STATUS_FAILURE;
         }

         fseek( fileRead, 0, SEEK_END );
         len = ftell( fileRead );
	 /* Coverity Issue Fix - CID:74334 : Negative Returns */
	 if (len < 0)
	 {
		CcspTraceWarning(("%s-%d : Error finding size of JSON file\n" , __FUNCTION__, __LINE__ ));
                 fclose(fileRead);
                 return ANSC_STATUS_FAILURE;
         }
         fseek( fileRead, 0, SEEK_SET );
         data = ( char* )malloc( sizeof(char) * (len + 1) );
         if (data != NULL)
         {
                memset( data, 0, ( sizeof(char) * (len + 1) ));
                if( fread( data, sizeof(char), len, fileRead ) < (len*sizeof(char)) )
                     CcspTraceWarning(("%s-%d : File read failed \n", __FUNCTION__, __LINE__));
         }
         else
         {
                 CcspTraceWarning(("%s-%d : Memory allocation failed \n", __FUNCTION__, __LINE__));
                 fclose( fileRead );
                 return ANSC_STATUS_FAILURE;
         }

         fclose( fileRead );
         if ( data == NULL )
         {
                CcspTraceWarning(("%s-%d : fileRead failed \n", __FUNCTION__, __LINE__));
                return ANSC_STATUS_FAILURE;
         }
	 /* CID 135285 String not null terminated fix */
         else if ( strnlen(data,len) != 0)
         {
                 json = cJSON_Parse( data );
                 if( !json )
                 {
                         CcspTraceWarning((  "%s : json file parser error : [%d]\n", __FUNCTION__,__LINE__));
                         free(data);
                         return ANSC_STATUS_FAILURE;
                 }
                 else
                 {
                         partnerObj = cJSON_GetObjectItem( json, PartnerId );
                         if ( NULL != partnerObj)
                         {
                                 cJSON *paramObj = cJSON_GetObjectItem( partnerObj, pKey);
                                 if (NULL != paramObj )
                                 {
                                         cJSON_ReplaceItemInObject(paramObj, "ActiveValue", cJSON_CreateString(pValue));
                                         cJSON_ReplaceItemInObject(paramObj, "UpdateTime", cJSON_CreateString(pCurrentTime));
                                         cJSON_ReplaceItemInObject(paramObj, "UpdateSource", cJSON_CreateString(pSource));

                                         cJsonOut = cJSON_Print(json);
                                         if(cJsonOut)
                                         {
                                            CcspTraceWarning(( "Updated json content is %s\n", cJsonOut));
                                            configUpdateStatus = writeToJson(cJsonOut, BOOTSTRAP_INFO_FILE);
                                            //Check CLEAR_TRACK_FILE and update in nvram, if needed.
                                            unsigned int flags = 0;
                                            FILE *fp = fopen(CLEAR_TRACK_FILE, "r");
                                            if (fp)
                                            {
                                                /* Coverity Fix : CHECKED_RETURN */
                                                if (fscanf(fp, "%u", &flags) != 1) {
                                                    CcspTraceWarning(("%s: Failed to read flags from %s, defaulting to 0\n", __FUNCTION__, CLEAR_TRACK_FILE));
                                                    flags = 0;
                                                }
                                                fclose(fp);
                                            }
                                            if ((flags & NVRAM_BOOTSTRAP_CLEARED) == 0)
                                            {
                                                CcspTraceWarning(("%s: Updating %s\n", __FUNCTION__, BOOTSTRAP_INFO_FILE_BACKUP));
                                                writeToJson(cJsonOut, BOOTSTRAP_INFO_FILE_BACKUP);
                                            }
                                            free(cJsonOut);
                                         }
                                         if ( !configUpdateStatus)
                                         {
                                                 CcspTraceWarning(( "Bootstrap config update: %s, %s, %s, %s \n", pKey, pValue, PartnerId, pSource));
                                         }
                                         else
                                        {
                                                 CcspTraceWarning(( "Failed to update value for %s partner\n",PartnerId));
                                                 CcspTraceWarning(( "Param:%s\n",pKey));
                                                 cJSON_Delete(json);
						 /* CID 72622 Resource leak fix */
						 free(data);
						 data = NULL;
                                                 return ANSC_STATUS_FAILURE;
                                        }
                                 }
                                else
                                {
                                        CcspTraceWarning(("%s - OBJECT  Value is NULL %s\n", pKey,__FUNCTION__ ));
                                        cJSON_Delete(json);
					/* CID 72622 Resource leak fix */
					free(data);
					data = NULL;
                                        return ANSC_STATUS_FAILURE;
                                }

                         }
                         else
                         {
                                CcspTraceWarning(("%s - PARTNER ID OBJECT Value is NULL\n", __FUNCTION__ ));
                                cJSON_Delete(json);
				/* CID 72622 Resource leak fix */
				free(data);
				data = NULL;
                                return ANSC_STATUS_FAILURE;
                         }
                        cJSON_Delete(json);
                 }
          }
          else
          {
                CcspTraceWarning(("BOOTSTRAP_INFO_FILE %s is empty\n", BOOTSTRAP_INFO_FILE));
                /* Coverity Fix CID : 72622 Resource leak */
                if (data)
                {
                    free(data);
                }
                return ANSC_STATUS_FAILURE;
          }

          //Also update in the legacy file /nvram/partners_defaults.json for firmware roll over purposes.
          UpdateJsonParamLegacy(pKey, PartnerId, pValue);
          /* Coverity Fix CID : 72622 Resource leak */
          if (data)
          {
              free(data);
          }
         return ANSC_STATUS_SUCCESS;
}
