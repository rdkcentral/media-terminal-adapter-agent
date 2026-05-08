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
#                                              -*- Autoconf -*-
# Process this file with autoconf to produce configure script.
#

AC_PREREQ([2.65])
AC_INIT([mta_tr181], [1.0], [BUG-REPORT-ADDRESS])
AM_INIT_AUTOMAKE([subdir-objects])
LT_INIT

MTA_CFLAGS="-DMAX_TIMEOUT_MTA_DHCP_ENABLED=60 -DMAX_TIMEOUT_MTA_DHCP_DISABLED=300 -DFOREVER=1"
AC_ARG_ENABLE([unitTestDockerSupport],
             AS_HELP_STRING([--enable-unitTestDockerSupport],[enable docker support for unit testing(default is no)]),
             [
             UNIT_TEST_DOCKER_SUPPORT=""
              case "${enableval}" in
               yes) UNIT_TEST_DOCKER_SUPPORT=true
                    MTA_CFLAGS="-DMAX_TIMEOUT_MTA_DHCP_ENABLED=2 -DMAX_TIMEOUT_MTA_DHCP_DISABLED=1 -DFOREVER=0"
                    m4_if(m4_sysval,[0],[AC_CONFIG_FILES([source/test/Makefile])]);;
               no) UNIT_TEST_DOCKER_SUPPORT=false;;
               *) AC_MSG_ERROR([bad value ${enableval} for --enable-unitTestDockerSupport ]);;
              esac
             ],
             [echo "Docker support is disabled"])
AM_CONDITIONAL([UNIT_TEST_DOCKER_SUPPORT], [test x$UNIT_TEST_DOCKER_SUPPORT = xtrue])

# Checks for Voice_support
AC_ARG_ENABLE([voice_mta_support],
[  --enable-voice_mta_support=val Turn on voice support, val=yes or no],
[case "${enableval}" in
  yes) voice_support=yes;;
  no)  voice_support=no;;
  *) AC_MSG_ERROR([bad value ${enableval} for --enable-voice_mta_support]) ;;
esac],[voice_mta_support=no])
AM_CONDITIONAL(VOICE_MTA_SUPPORT, test x"$voice_support" = x"yes")

AC_PREFIX_DEFAULT(`pwd`)
AC_ENABLE_SHARED
AC_DISABLE_STATIC

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL
AM_PROG_CC_C_O
AM_PROG_LIBTOOL(libtool)

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE

# Checks for library functions.
AC_FUNC_MALLOC

AC_CONFIG_FILES(
	source/TR-181/middle_layer_src/Makefile
	source/TR-181/integration_src.shared/Makefile
	source/TR-181/Makefile
	source/MtaAgentSsp/Makefile
	source/Makefile
	Makefile
)

AC_SUBST(UNIT_TEST_DOCKER_SUPPORT)
AC_SUBST(MTA_CFLAGS)
AC_OUTPUT

