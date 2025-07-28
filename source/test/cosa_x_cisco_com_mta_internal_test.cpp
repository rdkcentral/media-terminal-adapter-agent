/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2024 RDK Management
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

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "mta_mock.h"

using ::testing::Return;
using ::testing::DoAll;
using ::testing::StrEq;
using ::testing::_;
using ::testing::Invoke;


#define MTA_DHCP_ENABLED 0
#define MTA_DHCP_DISABLED 1

extern MtaHalMock* g_mtaHALMock;
extern SyscfgMock* g_syscfgMock;
extern SafecLibMock* g_safecLibMock;
extern SyseventMock *g_syseventMock;
extern PtdHandlerMock * g_PtdHandlerMock;
extern AnscMemoryMock* g_anscMemoryMock;

extern "C"
{      
    #include "cosa_x_cisco_com_mta_internal.h"  
}

TEST_F(CcspMtaTestFixture, Create_OnSuccess) {

    EXPECT_CALL(*g_mtaHALMock, mta_hal_InitDB())
        .WillOnce(Return(RETURN_OK)); 
  
    EXPECT_CALL(*g_mtaHALMock, mta_hal_LineTableGetNumberOfEntries())
        .WillOnce(Return(5));

    ANSC_HANDLE handle = CosaMTACreate();

    EXPECT_NE(handle, nullptr); 

    PCOSA_DATAMODEL_MTA pMyObject = reinterpret_cast<PCOSA_DATAMODEL_MTA>(handle);
    EXPECT_EQ(pMyObject->Oid, COSA_DATAMODEL_MTA_OID); // Check Oid
    EXPECT_EQ(pMyObject->Create, CosaMTACreate); // Check Create function pointer
    EXPECT_EQ(pMyObject->Remove, CosaMTARemove); // Check Remove function pointer is null
    EXPECT_EQ(pMyObject->Initialize, CosaMTAInitialize); // Check Initialize function pointer is null

    free(pMyObject);
}

TEST_F(CcspMtaTestFixture, ConverStr2Hex_ValidHexadecimalStrings) {

    unsigned char buffer[] = "1A3F"; //Va;id Input
    EXPECT_CALL(*g_safecLibMock, _strcpy_s_chk(_, _, _, _)).WillRepeatedly(Return(0));
    
    ANSC_STATUS status = ConverStr2Hex(buffer);

    EXPECT_EQ(status, ANSC_STATUS_SUCCESS);
    EXPECT_EQ(buffer[0], 0x1);
    EXPECT_EQ(buffer[1], 0xA);
    EXPECT_EQ(buffer[2], 0x3);
    EXPECT_EQ(buffer[3], 0xF);
}

TEST_F(CcspMtaTestFixture, ConverStr2Hex_ValidHexadecimalLowercase) {

    unsigned char buffer[] = "1a3f"; // Valid input with lowercase
    EXPECT_CALL(*g_safecLibMock, _strcpy_s_chk(_, _, _, _)).WillRepeatedly(Return(0));
    ANSC_STATUS status = ConverStr2Hex(buffer);

    EXPECT_EQ(status, ANSC_STATUS_SUCCESS);
    EXPECT_EQ(buffer[0], 0x1);
    EXPECT_EQ(buffer[1], 0xA);
    EXPECT_EQ(buffer[2], 0x3); 
    EXPECT_EQ(buffer[3], 0xF);
}

TEST_F(CcspMtaTestFixture, ConverStr2Hex_InvalidHexadecimalString) {

    unsigned char buffer[] = "GHIA"; //Invalid input
    EXPECT_CALL(*g_safecLibMock, _strcpy_s_chk(_, _, _, _)).WillRepeatedly(Return(0));
    ANSC_STATUS status = ConverStr2Hex(buffer);

    EXPECT_EQ(status, ANSC_STATUS_FAILURE);
}

TEST_F(CcspMtaTestFixture, ConverStr2Hex_strcpyFail) {
    unsigned char buffer[] = "1A3F";
    EXPECT_CALL(*g_safecLibMock, _strcpy_s_chk(_, _, _, _)).WillRepeatedly(Return(-1));
    
    ANSC_STATUS status = ConverStr2Hex(buffer);

    EXPECT_EQ(status, ANSC_STATUS_FAILURE);
}

TEST_F(CcspMtaTestFixture, checkIfDefMtaDhcpOptionEnabled_Success) {

    EXPECT_CALL(*g_syscfgMock, syscfg_get(nullptr, "IPv4PrimaryDhcpServerOptions",_, _))
        .WillOnce(Invoke([](const char*, const char*, char* out_value, int) {
            strcpy(out_value, "FFFFFFFF");
            return 1;
        }));
    EXPECT_CALL(*g_syscfgMock, syscfg_get(nullptr, "IPv4SecondaryDhcpServerOptions",_, _))
        .WillOnce(Invoke([](const char*, const char*, char* out_value, int) {
            strcpy(out_value, "FFFFFFFF");
            return 1;
        }));
    EXPECT_CALL(*g_syscfgMock, syscfg_get(nullptr, "IPv6PrimaryDhcpServerOptions",_, _))
        .WillOnce(Invoke([](const char*, const char*, char* out_value, int) {
            strcpy(out_value, "FFFFFFFF");
            return 1;
        }));
    EXPECT_CALL(*g_syscfgMock, syscfg_get(nullptr, "IPv6SecondaryDhcpServerOptions",_, _))
        .WillOnce(Invoke([](const char*, const char*, char* out_value, int) {
            strcpy(out_value, "FFFFFFFF");
            return 1;
        }));
        
    int result = checkIfDefMtaDhcpOptionEnabled();

    EXPECT_EQ(result, MTA_DHCP_ENABLED);
}

TEST_F(CcspMtaTestFixture, checkIfDefMtaDhcpOptionEnabled_Failure) {

    EXPECT_CALL(*g_syscfgMock, syscfg_get(nullptr, "IPv4PrimaryDhcpServerOptions",_, _))
        .WillOnce(Invoke([](const char*, const char*, char* out_value, int) {
            strcpy(out_value, "1921680101");   
            return 1;   
        }));
    EXPECT_CALL(*g_syscfgMock, syscfg_get(nullptr, "IPv4SecondaryDhcpServerOptions",_, _))
        .WillOnce(Invoke([](const char*, const char*, char* out_value, int) {
            strcpy(out_value, "2055.211");   
            return 1;   
        }));
    EXPECT_CALL(*g_syscfgMock, syscfg_get(nullptr, "IPv6PrimaryDhcpServerOptions",_, _))
        .WillOnce(Invoke([](const char*, const char*, char* out_value, int) {
            strcpy(out_value, "1921680121");   
            return 1;   
        }));
    EXPECT_CALL(*g_syscfgMock, syscfg_get(nullptr, "IPv6SecondaryDhcpServerOptions",_, _))
        .WillOnce(Invoke([](const char*, const char*, char* out_value, int) {
            strcpy(out_value, "2.55555");   
            return 1;   
        }));
        
    int result = checkIfDefMtaDhcpOptionEnabled();

    EXPECT_EQ(result, MTA_DHCP_DISABLED);
}

TEST_F(CcspMtaTestFixture, getmaxcount_Success) {

    EXPECT_CALL(*g_syscfgMock, syscfg_get(nullptr, "IPv4PrimaryDhcpServerOptions",_, _))
        .WillOnce(Invoke([](const char*, const char*, char* out_value, int) {
            strcpy(out_value, "FFFFFFFF");   
            return 1;   
        }));
    EXPECT_CALL(*g_syscfgMock, syscfg_get(nullptr, "IPv4SecondaryDhcpServerOptions",_, _))
        .WillOnce(Invoke([](const char*, const char*, char* out_value, int) {
            strcpy(out_value, "FFFFFFFF");   
            return 1;   
        }));
    EXPECT_CALL(*g_syscfgMock, syscfg_get(nullptr, "IPv6PrimaryDhcpServerOptions",_, _))
        .WillOnce(Invoke([](const char*, const char*, char* out_value, int) {
            strcpy(out_value, "FFFFFFFF");   
            return 1;   
        }));
    EXPECT_CALL(*g_syscfgMock, syscfg_get(nullptr, "IPv6SecondaryDhcpServerOptions",_, _))
        .WillOnce(Invoke([](const char*, const char*, char* out_value, int) {
            strcpy(out_value, "FFFFFFFF");   
            return 1;   
        }));
        
    int result = getMaxCount();

    EXPECT_EQ(result, MAX_TIMEOUT_MTA_DHCP_ENABLED);
}

TEST_F(CcspMtaTestFixture, getmaxcount_Failure) {

    EXPECT_CALL(*g_syscfgMock, syscfg_get(nullptr, "IPv4PrimaryDhcpServerOptions",_, _))
        .WillOnce(Invoke([](const char*, const char*, char* out_value, int) {
            strcpy(out_value, "1921680101");   
            return 1;   
        }));
    EXPECT_CALL(*g_syscfgMock, syscfg_get(nullptr, "IPv4SecondaryDhcpServerOptions",_, _))
        .WillOnce(Invoke([](const char*, const char*, char* out_value, int) {
            strcpy(out_value, "2055.211");   
            return 1;   
        }));
    EXPECT_CALL(*g_syscfgMock, syscfg_get(nullptr, "IPv6PrimaryDhcpServerOptions",_, _))
        .WillOnce(Invoke([](const char*, const char*, char* out_value, int) {
            strcpy(out_value, "1921680121");   
            return 1;   
        }));
    EXPECT_CALL(*g_syscfgMock, syscfg_get(nullptr, "IPv6SecondaryDhcpServerOptions",_, _))
        .WillOnce(Invoke([](const char*, const char*, char* out_value, int) {
            strcpy(out_value, "2.55555");   
            return 1;   
        }));
        
    int result = getMaxCount();

    EXPECT_EQ(result, MAX_TIMEOUT_MTA_DHCP_DISABLED);
}

TEST_F(CcspMtaTestFixture, WaitForDhcpOption_Success) {

    EXPECT_CALL(*g_syscfgMock, syscfg_get(nullptr, "IPv4PrimaryDhcpServerOptions",_, _))
        .WillOnce(Invoke([](const char*, const char*, char* out_value, int) {
            strcpy(out_value, "FFFFFFFF"); 
            return 1; 
        }));
    EXPECT_CALL(*g_syscfgMock, syscfg_get(nullptr, "IPv4SecondaryDhcpServerOptions",_, _))
        .WillOnce(Invoke([](const char*, const char*, char* out_value, int) {
            strcpy(out_value, "FFFFFFFF"); 
            return 1; 
        }));
    EXPECT_CALL(*g_syscfgMock, syscfg_get(nullptr, "IPv6PrimaryDhcpServerOptions",_, _))
        .WillOnce(Invoke([](const char*, const char*, char* out_value, int) {
            strcpy(out_value, "FFFFFFFF");
            return 1; 
        }));
    EXPECT_CALL(*g_syscfgMock, syscfg_get(nullptr, "IPv6SecondaryDhcpServerOptions",_, _))
        .WillOnce(Invoke([](const char*, const char*, char* out_value, int) {
            strcpy(out_value, "FFFFFFFF"); 
            return 1; 
        }));

    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_, _, _, _, _))
 	.WillOnce(Return(0));
        
    EXPECT_CALL(*g_syseventMock, sysevent_get(_, _, _, _, _)).Times(1)
        .WillOnce(Invoke([&](const int fd, const token_t token, const char *inbuf, char *outbuf, int outbytes) {
            strncpy(outbuf, "received", outbytes - 1);
            outbuf[outbytes - 1] = '\0'; 
            return 0; 
        }));

    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_, _, _, _, _, _)).Times(1)
        .WillOnce(DoAll(testing::SetArgPointee<3>(0), testing::Return(0)));
        
    WaitForDhcpOption();
}

TEST_F(CcspMtaTestFixture, WaitForDhcpOption_Failure) {
    
     EXPECT_CALL(*g_syscfgMock, syscfg_get(nullptr, "IPv4PrimaryDhcpServerOptions",_, _))
        .WillOnce(Invoke([](const char*, const char*, char* out_value, int) {
            strcpy(out_value, "1921680101");   
            return 1;   
        }));
    EXPECT_CALL(*g_syscfgMock, syscfg_get(nullptr, "IPv4SecondaryDhcpServerOptions",_, _))
        .WillOnce(Invoke([](const char*, const char*, char* out_value, int) {
            strcpy(out_value, "2055.211");   
            return 1;   
        }));
    EXPECT_CALL(*g_syscfgMock, syscfg_get(nullptr, "IPv6PrimaryDhcpServerOptions",_, _))
        .WillOnce(Invoke([](const char*, const char*, char* out_value, int) {
            strcpy(out_value, "1921680121");   
            return 1;   
        }));
    EXPECT_CALL(*g_syscfgMock, syscfg_get(nullptr, "IPv6SecondaryDhcpServerOptions",_, _))
        .WillOnce(Invoke([](const char*, const char*, char* out_value, int) {
            strcpy(out_value, "2.55555");   
            return 1;   
        }));

    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_, _, _, _, _))
 	 .Times(1)
        .WillOnce(Return(0)); 
        
    EXPECT_CALL(*g_syseventMock, sysevent_get(_, _, _, _, _)).Times(1) 
        .WillOnce(Invoke([&](const int fd, const token_t token, const char *inbuf, char *outbuf, int outbytes) {
            strncpy(outbuf, "not_received", outbytes - 1);
            outbuf[outbytes - 1] = '\0';
            return 0;
        }));
       
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_, _, _, _, _, _)).Times(1)
        .WillRepeatedly(DoAll(testing::SetArgPointee<3>(1), Return(1)));

    WaitForDhcpOption();

}

TEST_F(CcspMtaTestFixture, CosaMTALineTableInitialize_Success) {

    PCOSA_DATAMODEL_MTA pMyObject = (PCOSA_DATAMODEL_MTA)malloc(sizeof(COSA_DATAMODEL_MTA));
   
    EXPECT_CALL(*g_mtaHALMock, mta_hal_LineTableGetNumberOfEntries())
        .WillOnce(Return(3));  // Simulate returning 3 entries

    ANSC_STATUS status = CosaMTALineTableInitialize((ANSC_HANDLE)pMyObject);


    EXPECT_EQ(status, ANSC_STATUS_SUCCESS);
    
    free(pMyObject);

}

TEST_F(CcspMtaTestFixture, CosaMTARemove_Success) {

    PCOSA_DATAMODEL_MTA pMyObject = (PCOSA_DATAMODEL_MTA)malloc(sizeof(COSA_DATAMODEL_MTA));
    ASSERT_NE(pMyObject, nullptr);
    memset(pMyObject, 0, sizeof(COSA_DATAMODEL_MTA));
    
    EXPECT_CALL(*g_anscMemoryMock, AnscFreeMemoryOrig((void*)pMyObject))
        .Times(1);

    ANSC_STATUS status = CosaMTARemove((ANSC_HANDLE)pMyObject);

    EXPECT_EQ(status, ANSC_STATUS_SUCCESS);
    
    free(pMyObject);
}

TEST_F(CcspMtaTestFixture, CosaMTAInitialize_Success) {

    PCOSA_DATAMODEL_MTA pMyObject = (PCOSA_DATAMODEL_MTA)malloc(sizeof(COSA_DATAMODEL_MTA));
    ASSERT_NE(pMyObject, nullptr);
    memset(pMyObject, 0, sizeof(COSA_DATAMODEL_MTA));
    
    EXPECT_CALL(*g_mtaHALMock, mta_hal_InitDB())
        .WillOnce(Return(RETURN_OK));       
          
    EXPECT_CALL(*g_mtaHALMock, mta_hal_LineTableGetNumberOfEntries())
        .WillOnce(Return(3)); 
        
    ANSC_STATUS status = CosaMTAInitialize((ANSC_HANDLE)pMyObject);
    
    EXPECT_EQ(status, ANSC_STATUS_SUCCESS);
        
    free(pMyObject);
}

 
TEST_F(CcspMtaTestFixture, CosaMTAInitializeEthWanProv_Success) { 

    PCOSA_DATAMODEL_MTA pMyObject = (PCOSA_DATAMODEL_MTA)malloc(sizeof(COSA_DATAMODEL_MTA));
    ASSERT_NE(pMyObject, nullptr);
    memset(pMyObject, 0, sizeof(COSA_DATAMODEL_MTA));
            
    EXPECT_CALL(*g_syscfgMock, syscfg_get(nullptr, "StartupIPMode", _, _))
    .WillOnce(Invoke([](const char*, const char*, char* out_value, int) {      
        strcpy(out_value, "2");
        return 0;   
    }));
                           
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_, _, _, _, _))
 	 .Times(8) 
        .WillRepeatedly(Return(0)); 
             
    EXPECT_CALL(*g_syscfgMock, syscfg_get(nullptr, "IPv4PrimaryDhcpServerOptions", _, _))
    .WillOnce(Invoke([](const char*, const char*, char* out_value, int) {       
        strcpy(out_value, "0A0B0C0D");
        return 0;   
    }));
        
   EXPECT_CALL(*g_safecLibMock, _strcpy_s_chk(_, _, _, _)).Times(4).WillRepeatedly(Return(0));
               
   EXPECT_CALL(*g_syscfgMock, syscfg_get(nullptr, "IPv4SecondaryDhcpServerOptions", _, _))
    .WillOnce(Invoke([](const char*, const char*, char* out_value, int) {  
        strcpy(out_value, "0A0B0C0D");
        return 0;   
    }));          
        
    EXPECT_CALL(*g_syscfgMock, syscfg_get(nullptr, "IPv6PrimaryDhcpServerOptions", _, _))
    .WillOnce(Invoke([](const char*, const char*, char* out_value, int) {
        strcpy(out_value, "20010db885a3000000008a2e03707334");
        return 0;   
    }));
        
    EXPECT_CALL(*g_syscfgMock, syscfg_get(nullptr, "IPv6SecondaryDhcpServerOptions", _, _))
    .WillOnce(Invoke([](const char*, const char*, char* out_value, int) {   
        strcpy(out_value, "220010db885a3000000008a2e03707334");
        return 0;   
    }));
    
   ANSC_STATUS status = CosaMTAInitializeEthWanProv((ANSC_HANDLE)pMyObject);
   
   EXPECT_EQ(status, ANSC_STATUS_SUCCESS);
           
   free(pMyObject);       
}

TEST_F(CcspMtaTestFixture, CosaSetMTAHal_Success) { 

    PCOSA_MTA_ETHWAN_PROV_INFO pMtaEthPro = (PCOSA_MTA_ETHWAN_PROV_INFO)malloc(sizeof(COSA_MTA_ETHWAN_PROV_INFO));
    ASSERT_NE(pMtaEthPro, nullptr);
    memset(pMtaEthPro, 0, sizeof(COSA_MTA_ETHWAN_PROV_INFO));
    
    strcpy(pMtaEthPro->IPv4PrimaryDhcpServerOptions.ActiveValue, "0A0B0C0D");
    strcpy(pMtaEthPro->IPv4SecondaryDhcpServerOptions.ActiveValue, "0A0B0C0D");
    strcpy(pMtaEthPro->IPv6PrimaryDhcpServerOptions.ActiveValue, "20010db885a3000000008a2e03707334");
    strcpy(pMtaEthPro->IPv6SecondaryDhcpServerOptions.ActiveValue, "20010db885a3000000008a2e03707334");
    pMtaEthPro->StartupIPMode.ActiveValue = 2;
    
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_, _, _, _, _)).Times(4) .WillRepeatedly(Return(0)); 
 
    EXPECT_CALL(*g_safecLibMock, _strcpy_s_chk(_, _, StrEq("0A0B0C0D"), _)).Times(4)
    .WillRepeatedly(Invoke([](char *dest, rsize_t dmax, const char *src, const size_t destbos) {
        strcpy(dest, "0A0B0C0D");
        return 0;
    }));

EXPECT_CALL(*g_safecLibMock, _strcpy_s_chk(_, _, StrEq("20010db885a3000000008a2e03707334"), _)).Times(4)
    .WillRepeatedly(Invoke([](char *dest, rsize_t dmax, const char *src, const size_t destbos) {
        strcpy(dest, "20010db885a3000000008a2e03707334");
        return 0;
    }));     
    
    ANSC_STATUS status = CosaSetMTAHal(pMtaEthPro);
    
    EXPECT_EQ(status, ANSC_STATUS_SUCCESS);
           
    free(pMtaEthPro);
}

TEST_F(CcspMtaTestFixture, Mta_Sysevent_thread_Success) {

    PCOSA_DATAMODEL_MTA pMyObject = (PCOSA_DATAMODEL_MTA)malloc(sizeof(COSA_DATAMODEL_MTA));
    ASSERT_NE(pMyObject, nullptr);	
    memset(pMyObject, 0, sizeof(COSA_DATAMODEL_MTA));
    
    EXPECT_CALL(*g_syscfgMock, syscfg_get(_, "eth_wan_enabled", _, _))
     .Times(2)
    .WillRepeatedly(Invoke([](const char*, const char*, char* out_value, int) {
          strcpy(out_value, "true");
          return 0;   
    }));
        
    EXPECT_CALL(*g_syseventMock, sysevent_set_options(_, _, _, _)).Times(1).WillOnce(Return(0));
        
    EXPECT_CALL(*g_syseventMock, sysevent_setnotification(_, _, _, _)).Times(1).WillOnce(Return(0));              
        
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_, _, _, _, _, _)).Times(3)
        .WillRepeatedly(DoAll(testing::SetArgPointee<3>(0), testing::Return(0)));
        
    EXPECT_CALL(*g_syscfgMock, syscfg_get(nullptr, "StartupIPMode", _, _))
        .Times(2)
        .WillRepeatedly(Invoke([](const char*, const char*, char* out_value, int) {
          strcpy(out_value, "2");
          return 0;   
    }));
                             
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_, _, _, _, _)).Times(18)  .WillRepeatedly(Return(0)); 
              
    EXPECT_CALL(*g_syscfgMock, syscfg_get(nullptr, "IPv4PrimaryDhcpServerOptions", _, _)).Times(2)
    .WillRepeatedly(Invoke([](const char*, const char*, char* out_value, int) {
          strcpy(out_value, "0A0B0C0D");
          return 0;   
    }));
        
   EXPECT_CALL(*g_safecLibMock, _strcpy_s_chk(_, _, _, _)) .Times(8).WillRepeatedly(Return(0));
                     
   EXPECT_CALL(*g_syscfgMock, syscfg_get(nullptr, "IPv4SecondaryDhcpServerOptions", _, _)).Times(2)
    .WillRepeatedly(Invoke([](const char*, const char*, char* out_value, int) {
        strcpy(out_value, "0A0B0C0D");
        return 0;   
    }));
                 
    EXPECT_CALL(*g_syscfgMock, syscfg_get(nullptr, "IPv6PrimaryDhcpServerOptions", _, _)).Times(2)
    .WillRepeatedly(Invoke([](const char*, const char*, char* out_value, int) {
        strcpy(out_value, "20010db885a3000000008a2e03707334");
        return 0;   
    }));
               
    EXPECT_CALL(*g_syscfgMock, syscfg_get(nullptr, "IPv6SecondaryDhcpServerOptions", _, _)).Times(2)
    .WillRepeatedly(Invoke([](const char*, const char*, char* out_value, int) {
        strcpy(out_value, "20010db885a3000000008a2e03707334");
        return 0; 
    }));
  
    EXPECT_CALL(*g_syseventMock, sysevent_get(_, _, _, _, _)).Times(1)
        .WillOnce(Return(0));
         
    EXPECT_CALL(*g_syseventMock, sysevent_getnotification(_,_,_,_,_,_,_)).Times(1).WillOnce(Return(0));
    
    Mta_Sysevent_thread((ANSC_HANDLE)pMyObject);    
    
    free(pMyObject);     
 }
               
TEST_F(CcspMtaTestFixture, CosaMTAInitializeEthWanProvDhcpOption_Success) {

    ANSC_HANDLE hThisObject = (ANSC_HANDLE)malloc(sizeof(COSA_DATAMODEL_MTA));
    ASSERT_NE(hThisObject, nullptr);	
    memset(hThisObject, 0, sizeof(COSA_DATAMODEL_MTA));

    EXPECT_CALL(*g_syseventMock, sysevent_get(_,_, "MTA_IP_PREF", _,_))
        .WillOnce(testing::Invoke([](const int fd, const token_t token, const char *inbuf, char *outbuf, int outbytes) {
             strncpy(outbuf, "05", outbytes - 1);
            outbuf[outbytes - 1] = '\0'; 
            return 0; 
        }));
    EXPECT_CALL(*g_syseventMock, sysevent_get(_,_, "MTA_DHCPv4_PrimaryAddress", testing::_, testing::_))
        .WillOnce(testing::Invoke([](const int fd, const token_t token, const char *inbuf, char *outbuf, int outbytes) {
            strncpy(outbuf, "192.168.1.1", outbytes - 1);
            outbuf[outbytes - 1] = '\0'; 
            return 0;
        }));
    EXPECT_CALL(*g_syseventMock, sysevent_get(_,_, "MTA_DHCPv4_SecondaryAddress", testing::_, testing::_))
        .WillOnce(testing::Invoke([](const int fd, const token_t token, const char *inbuf, char *outbuf, int outbytes) {
            strncpy(outbuf, "192.168.1.2", outbytes - 1);
            outbuf[outbytes - 1] = '\0'; 
            return 0;
        }));
    EXPECT_CALL(*g_syseventMock, sysevent_get(_,_, "MTA_DHCPv6_PrimaryAddress", testing::_, testing::_))
        .WillOnce(testing::Invoke([](const int fd, const token_t token, const char *inbuf, char *outbuf, int outbytes) {
           strncpy(outbuf, "", outbytes - 1);
           outbuf[outbytes - 1] = '\0'; 
            return 0;
        }));
    EXPECT_CALL(*g_syseventMock, sysevent_get(_,_, "MTA_DHCPv6_SecondaryAddress", testing::_, testing::_))
        .WillOnce(testing::Invoke([](const int fd, const token_t token, const char *inbuf, char *outbuf, int outbytes) {
           strncpy(outbuf, "", outbytes - 1);
           outbuf[outbytes - 1] = '\0'; 
            return 0;
        }));
       
    EXPECT_CALL(*g_syscfgMock, syscfg_get(nullptr, "IPv6PrimaryDhcpServerOptions", _, _))
    .WillOnce(Invoke([](const char*, const char*, char* out_value, int) {
        strcpy(out_value, "20010db885a3000000008a2e03707334");
        return 0;   
    }));
               
    EXPECT_CALL(*g_syscfgMock, syscfg_get(nullptr, "IPv6SecondaryDhcpServerOptions", _, _))
    .WillOnce(Invoke([](const char*, const char*, char* out_value, int) {
        strcpy(out_value, "20010db885a3000000008a2e03707334");
        return 0; 
    }));
        
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_, _, _, _, _)).Times(4)  .WillRepeatedly(Return(0)); 
        
    EXPECT_CALL(*g_safecLibMock, _strcpy_s_chk(_, _, _, _)).Times(4).WillRepeatedly(Return(0));
        
    ANSC_STATUS status = CosaMTAInitializeEthWanProvDhcpOption(hThisObject);

    EXPECT_EQ(status, ANSC_STATUS_SUCCESS);

    free(hThisObject);
}

TEST_F(CcspMtaTestFixture, CosaMTAInitializeEthWanProvDhcpOption_Failure) {

    ANSC_HANDLE hThisObject = (ANSC_HANDLE)malloc(sizeof(COSA_DATAMODEL_MTA));
    ASSERT_NE(hThisObject, nullptr);	
    memset(hThisObject, 0, sizeof(COSA_DATAMODEL_MTA));

    EXPECT_CALL(*g_syseventMock, sysevent_get(_,_, "MTA_IP_PREF", _,_))
        .WillOnce(testing::Invoke([](const int fd, const token_t token, const char *inbuf, char *outbuf, int outbytes) {
             strncpy(outbuf, "01", outbytes - 1);
            outbuf[outbytes - 1] = '\0'; 
            return 0; 
        }));

    EXPECT_CALL(*g_syseventMock, sysevent_get(_,_, "MTA_DHCPv4_PrimaryAddress", testing::_, testing::_))
        .WillOnce(testing::Invoke([](const int fd, const token_t token, const char *inbuf, char *outbuf, int outbytes) {
            strncpy(outbuf, "00000000", outbytes - 1);
            outbuf[outbytes - 1] = '\0'; 
            return 0;
        }));
        
    EXPECT_CALL(*g_syseventMock, sysevent_get(_,_, "MTA_DHCPv4_SecondaryAddress", testing::_, testing::_))
        .WillOnce(testing::Invoke([](const int fd, const token_t token, const char *inbuf, char *outbuf, int outbytes) {
            strncpy(outbuf, "192.168.1.2", outbytes - 1);
            outbuf[outbytes - 1] = '\0'; 
            return 0;
        }));
        
    EXPECT_CALL(*g_syseventMock, sysevent_get(_,_, "MTA_DHCPv6_PrimaryAddress", testing::_, testing::_))
        .WillOnce(testing::Invoke([](const int fd, const token_t token, const char *inbuf, char *outbuf, int outbytes) {
           strncpy(outbuf, "00000000", outbytes - 1);
           outbuf[outbytes - 1] = '\0'; 
            return 0;
        }));
    EXPECT_CALL(*g_syseventMock, sysevent_get(_,_, "MTA_DHCPv6_SecondaryAddress", testing::_, testing::_))
        .WillOnce(testing::Invoke([](const int fd, const token_t token, const char *inbuf, char *outbuf, int outbytes) {
           strncpy(outbuf, "", outbytes - 1);
           outbuf[outbytes - 1] = '\0'; 
            return 0;
        }));
        

    ANSC_STATUS status = CosaMTAInitializeEthWanProvDhcpOption(hThisObject);
    
    EXPECT_EQ(status, ANSC_STATUS_FAILURE);
    
    free(hThisObject);
}

TEST_F(CcspMtaTestFixture, CosaMTAInitializeEthWanProvDhcpOption_strcpy_fail) {

    ANSC_HANDLE hThisObject = (ANSC_HANDLE)malloc(sizeof(COSA_DATAMODEL_MTA));
    ASSERT_NE(hThisObject, nullptr);	
    memset(hThisObject, 0, sizeof(COSA_DATAMODEL_MTA));

    EXPECT_CALL(*g_syseventMock, sysevent_get(_,_, "MTA_IP_PREF", _,_))
        .WillOnce(testing::Invoke([](const int fd, const token_t token, const char *inbuf, char *outbuf, int outbytes) {
             strncpy(outbuf, "01", outbytes - 1);
            outbuf[outbytes - 1] = '\0'; 
            return 0; 
        }));
    EXPECT_CALL(*g_syseventMock, sysevent_get(_,_, "MTA_DHCPv4_PrimaryAddress", testing::_, testing::_))
        .WillOnce(testing::Invoke([](const int fd, const token_t token, const char *inbuf, char *outbuf, int outbytes) {
            strncpy(outbuf, "192.168.1.1", outbytes - 1);
            outbuf[outbytes - 1] = '\0'; 
            return 0;
        }));
    EXPECT_CALL(*g_syseventMock, sysevent_get(_,_, "MTA_DHCPv4_SecondaryAddress", testing::_, testing::_))
        .WillOnce(testing::Invoke([](const int fd, const token_t token, const char *inbuf, char *outbuf, int outbytes) {
            strncpy(outbuf, "192.168.1.2", outbytes - 1);
            outbuf[outbytes - 1] = '\0'; 
            return 0;
        }));
    EXPECT_CALL(*g_syseventMock, sysevent_get(_,_, "MTA_DHCPv6_PrimaryAddress", testing::_, testing::_))
        .WillOnce(testing::Invoke([](const int fd, const token_t token, const char *inbuf, char *outbuf, int outbytes) {
           strncpy(outbuf, "", outbytes - 1);
           outbuf[outbytes - 1] = '\0'; 
            return 0;
        }));
    EXPECT_CALL(*g_syseventMock, sysevent_get(_,_, "MTA_DHCPv6_SecondaryAddress", testing::_, testing::_))
        .WillOnce(testing::Invoke([](const int fd, const token_t token, const char *inbuf, char *outbuf, int outbytes) {
           strncpy(outbuf, "", outbytes - 1);
           outbuf[outbytes - 1] = '\0'; 
            return 0;
        }));
        
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_, _, _, _, _)).Times(1) .WillOnce(Return(0)); 
        
    EXPECT_CALL(*g_safecLibMock, _strcpy_s_chk(_, _, StrEq("192.168.1.1"), _)).Times(1)
       .WillOnce(Invoke([](char *dest, rsize_t dmax, const char *src, const size_t destbos) {
          strcpy(dest, "192.168.1.1");
          return -1;
    }));    
        
    ANSC_STATUS status = CosaMTAInitializeEthWanProvDhcpOption(hThisObject);

    EXPECT_EQ(status, ANSC_STATUS_FAILURE);

    free(hThisObject);
}

TEST_F(CcspMtaTestFixture, Mta_Sysevent_thread_Dhcp_Option_Success) {

    PCOSA_DATAMODEL_MTA pMyObject = (PCOSA_DATAMODEL_MTA)malloc(sizeof(COSA_DATAMODEL_MTA));
    ASSERT_NE(pMyObject, nullptr);	
    memset(pMyObject, 0, sizeof(COSA_DATAMODEL_MTA));

    EXPECT_CALL(*g_syseventMock, sysevent_set_options(_, _, _, _)).Times(1).WillOnce(Return(0));
 
    EXPECT_CALL(*g_syseventMock, sysevent_setnotification(_, _, _, _)).Times(1).WillOnce(Return(0));
                
    EXPECT_CALL(*g_syseventMock, sysevent_get(_, _, "current_wan_state", _, _)).Times(1)
        .WillOnce(Invoke([&](const int fd, const token_t token, const char *inbuf, char *outbuf, int outbytes) {
            strncpy(outbuf, "up", outbytes - 1);
            outbuf[outbytes - 1] = '\0'; 
            return 0; 
        }));
        
    EXPECT_CALL(*g_syseventMock, sysevent_get(_, _, "dhcp_mta_option", _, _)).Times(2)
        .WillRepeatedly(Invoke([&](const int fd, const token_t token, const char *inbuf, char *outbuf, int outbytes) {
            strncpy(outbuf, "received", outbytes - 1);
            outbuf[outbytes - 1] = '\0'; 
            return 0; 
        }));
    EXPECT_CALL(*g_syscfgMock, syscfg_get(_, "eth_wan_enabled", _, _)).Times(2)
    	.WillRepeatedly(Invoke([](const char*, const char*, char* out_value, int) {
           strcpy(out_value, "true");
           return 0;   
    }));
    
    EXPECT_CALL(*g_safecLibMock, _strcmp_s_chk(_, _, _, _, _, _)).Times(5)
        .WillRepeatedly(DoAll(testing::SetArgPointee<3>(0), testing::Return(0)));
        
    EXPECT_CALL(*g_syseventMock, sysevent_get(_,_, "MTA_IP_PREF", _,_)).Times(2)
        .WillRepeatedly(testing::Invoke([](const int fd, const token_t token, const char *inbuf, char *outbuf, int outbytes) {
             strncpy(outbuf, "01", outbytes - 1);
             outbuf[outbytes - 1] = '\0'; 
             return 0; 
        }));
    EXPECT_CALL(*g_syseventMock, sysevent_get(_,_, "MTA_DHCPv4_PrimaryAddress", testing::_, testing::_)).Times(2)
        .WillRepeatedly(testing::Invoke([](const int fd, const token_t token, const char *inbuf, char *outbuf, int outbytes) {
            strncpy(outbuf, "192.168.1.1", outbytes - 1);
            outbuf[outbytes - 1] = '\0'; 
            return 0;
        }));
    EXPECT_CALL(*g_syseventMock, sysevent_get(_,_, "MTA_DHCPv4_SecondaryAddress", testing::_, testing::_)).Times(2)
        .WillRepeatedly(testing::Invoke([](const int fd, const token_t token, const char *inbuf, char *outbuf, int outbytes) {
            strncpy(outbuf, "192.168.1.2", outbytes - 1);
            outbuf[outbytes - 1] = '\0'; 
            return 0;
        }));
    EXPECT_CALL(*g_syseventMock, sysevent_get(_,_, "MTA_DHCPv6_PrimaryAddress", testing::_, testing::_)).Times(2)
        .WillRepeatedly(testing::Invoke([](const int fd, const token_t token, const char *inbuf, char *outbuf, int outbytes) {
           strncpy(outbuf, "", outbytes - 1);
           outbuf[outbytes - 1] = '\0'; 
            return 0;
        }));
    EXPECT_CALL(*g_syseventMock, sysevent_get(_,_, "MTA_DHCPv6_SecondaryAddress", testing::_, testing::_)).Times(2)
        .WillRepeatedly(testing::Invoke([](const int fd, const token_t token, const char *inbuf, char *outbuf, int outbytes) {
           strncpy(outbuf, "", outbytes - 1);
           outbuf[outbytes - 1] = '\0'; 
            return 0;
        }));
                     
    EXPECT_CALL(*g_safecLibMock, _memset_s_chk(_, _, _, _, _)).Times(11).WillRepeatedly(Return(0)); 
        
    EXPECT_CALL(*g_safecLibMock, _strcpy_s_chk(_, _, _, _)).Times(4).WillRepeatedly(Return(0));
    
    EXPECT_CALL(*g_syseventMock, sysevent_getnotification(_,_,_,_,_,_,_)).Times(1).WillOnce(Return(0));
    
    EXPECT_CALL(*g_syscfgMock, syscfg_get(nullptr, "IPv4PrimaryDhcpServerOptions",_, _)).Times(1)
        .WillRepeatedly(Invoke([](const char*, const char*, char* out_value, int) {
            strcpy(out_value, "FFFFFFFF"); 
            return 1; 
        }));
    EXPECT_CALL(*g_syscfgMock, syscfg_get(nullptr, "IPv4SecondaryDhcpServerOptions",_, _)).Times(1)
        .WillRepeatedly(Invoke([](const char*, const char*, char* out_value, int) {
            strcpy(out_value, "FFFFFFFF"); 
            return 1; 
        }));
    EXPECT_CALL(*g_syscfgMock, syscfg_get(nullptr, "IPv6PrimaryDhcpServerOptions",_, _)).Times(3)
        .WillRepeatedly(Invoke([](const char*, const char*, char* out_value, int) {
            strcpy(out_value, "FFFFFFFF");
            return 1; 
        }));
    EXPECT_CALL(*g_syscfgMock, syscfg_get(nullptr, "IPv6SecondaryDhcpServerOptions",_, _)).Times(3)
        .WillRepeatedly(Invoke([](const char*, const char*, char* out_value, int) {
            strcpy(out_value, "FFFFFFFF"); 
            return 1; 
        }));       

    Mta_Sysevent_thread_Dhcp_Option((ANSC_HANDLE)pMyObject);
    
    free(pMyObject);    
}
