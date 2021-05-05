#!/bin/sh
	
appname=`basename $0 | sed s,\.sh$,,`
dirname="/opt/AthenaAgentGUI"
cd $dirname
tmp="${dirname#?}"
if [ "${dirname%$tmp}" != "/" ]; then        
	dirname=$PWD/$dirname
fi
LD_LIBRARY_PATH=$dirname/libs
export LD_LIBRARY_PATH
$dirname/$appname "$@"


