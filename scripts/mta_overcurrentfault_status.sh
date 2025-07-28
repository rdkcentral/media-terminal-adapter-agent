#! /bin/sh
#
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
#


source /etc/utopia/service.d/log_capture_path.sh

# Checking MTA's PID
MTA_PID=`pidof CcspMtaAgentSsp`
if [ "$MTA_PID" = "" ]; then
   #MTA process is not running so no need to proceed further	
   exit	
fi

# Getting MTA Line entries
echo_t "Obtaining the MTA Lines"
mta_total_lines=`dmcli eRT retv Device.X_CISCO_COM_MTA.LineTableNumberOfEntries`
echo_t "MTA Total_no_of_lines: $mta_total_lines"

if [ "$mta_total_lines" != "" ]; then

	loop_count=1
	while [ $loop_count -le $mta_total_lines ]
	do
		mta_OverCurrentFault=`dmcli eRT retv Device.X_CISCO_COM_MTA.LineTable.$loop_count.OverCurrentFault`
		echo_t "MTA_LINE_"$loop_count"_OVER_CURRENT_STATUS:$mta_OverCurrentFault"

		loop_count=`expr $loop_count + 1`
	done

fi
