##########################################################################
# If not stated otherwise in this file or this component's LICENSE
# file the following copyright and licenses apply:
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
AM_CFLAGS =

ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = CcspMtaAgentSsp

CcspMtaAgentSsp_CPPFLAGS = -I$(top_srcdir)/source/TR-181/board_sbapi -I$(top_srcdir)/source/TR-181/middle_layer_src -I$(top_srcdir)/source/TR-181/include -I$(top_srcdir)/source/Custom -I$(top_srcdir)/source/TR-181/integration_src.shared
CcspMtaAgentSsp_SOURCES = ssp_messagebus_interface.c TR104.c TR104_webconfig.c ssp_main.c ssp_action.c dm_pack_datamodel.c ../TR-181/integration_src.shared/cosa_x_cisco_com_mta_apis.c
CcspMtaAgentSsp_LDFLAGS = -lccsp_common -lhal_mta -lsyscfg -lprivilege -lsecure_wrapper -lwebconfig_framework -lmsgpackc -lsysevent -lcjson

