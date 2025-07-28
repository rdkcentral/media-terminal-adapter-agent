##########################################################################
# If not stated otherwise in this file or this component's LICENSE file the
# following copyright and licenses apply:
#
# Copyright 2015 RDK Management
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
##########################################################################
AM_CFLAGS =-fPIC -fno-exceptions -Os -Wall -Wextra

AM_CXXFLAGS = -std=c++11

ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = CcspMtaAgentSsp_gtest.bin

CcspMtaAgentSsp_gtest_bin_CPPFLAGS = -I$(PKG_CONFIG_SYSROOT_DIR)$(includedir)/gtest \
                                     -I${top_srcdir}/gtest/include \
                                     -I${top_srcdir}/source \
                                     -I${top_srcdir}/source/include \
                                     -I${top_srcdir}/source/TR-181 \
                                     -I${top_srcdir}/source/TR-181/include \
                                     -I${top_srcdir}/source/MtaAgentSsp \
                                     -I${top_srcdir}/source/TR-181/integration_src.shared \
                                     -I${top_srcdir}/source/TR-181/middle_layer_src \
                                     -I$(AM_CFLAGS) $(MTA_CFLAGS) \                                     
                                     -D_CBR_PRODUCT_REQ_ \
                                     -D_XB6_PRODUCT_REQ_ \
                                     -DMTA_TR104SUPPORT \
                                     -DINTEL_PUMA7 \
                                     -DCONFIG_TI_BBU

CcspMtaAgentSsp_gtest_bin_SOURCES = gtest_main.cpp \
                                    ${top_srcdir}/source/MtaAgentSsp/TR104_webconfig.c \
                                    ${top_srcdir}/source/MtaAgentSsp/TR104.c \
                                    ${top_srcdir}/source/TR-181/integration_src.shared/cosa_x_cisco_com_mta_apis.c \
                                    ${top_srcdir}/source/TR-181/middle_layer_src/cosa_x_cisco_com_mta_internal.c \
                                    ${top_srcdir}/source/TR-181/middle_layer_src/plugin_main.c \
                                    ${top_srcdir}/source/TR-181/middle_layer_src/plugin_main_apis.c \
                                    ${top_srcdir}/source/TR-181/middle_layer_src/cosa_x_cisco_com_mta_dml.c \
                                    cosa_x_cisco_com_mta_apis_test.cpp \
                                    cosa_x_cisco_com_mta_dml_test.cpp \
                                    TR104_webconfig_test.cpp \
                                    TR104_test.cpp \
                                    plugin_main_apis_test.cpp \
                                    cosa_x_cisco_com_mta_internal_test.cpp \
                                    mta_mock.cpp

CcspMtaAgentSsp_gtest_bin_LDFLAGS = -lgtest -lgmock -lgcov -lcjson -lpthread

CcspMtaAgentSsp_gtest_bin_LDADD =$(HOME)/usr/local/lib/libmock_mta_hal.la \
                                $(HOME)/usr/local/lib/libmock_safec_lib.la \
                                $(HOME)/usr/local/lib/libmock_ccsp_common.la \
                                $(HOME)/usr/local/lib/libmock_syscfg.la \
                                $(HOME)/usr/local/lib/libmock_sysevent.la \
                                $(HOME)/usr/local/lib/libmock_msgpack.la \
                                $(HOME)/usr/local/lib/libmock_rbus.la \
                                $(HOME)/usr/local/lib/libmock_webconfigframework.la 


# Adding Code Coverage Flags
AM_CFLAGS += -fprofile-arcs -ftest-coverage
AM_CXXFLAGS += -fprofile-arcs -ftest-coverage
CcspMtaAgentSsp_gtest_bin_LDFLAGS += --coverage
CcspMtaAgentSsp_gtest_bin_CFLAGS = $(AM_CFLAGS) $(MTA_CFLAGS)
