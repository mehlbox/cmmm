#!/bin/bash
if [ "$(id -u)" != "0" ]; then
   echo "This script must be run as root" 1>&2
   exit 1
fi

if [ "$(pgrep -fl webserver.py)" == "" ]; then
	echo "starting webserver.py"
	cd /root/cmmm/webserver/
	./webserver.py &
fi

if [ "$(pgrep -fl cmmm.py)" == "" ]; then
	echo "starting cmmm.py"
	cd /root/cmmm/
	./cmmm.py &
fi

if [ "$(pgrep -fl autoswitch.py)" == "" ]; then
	echo "starting autoswitch.py"
	cd /root/cmmm/
	./autoswitch.py &
fi
