#!/bin/bash

#set -x

CFG_FILE="case.cfg"
DBUS_CMD=""

get_cmd()
{
	echo "hello"
}

get_case_counter()
{
	CASE_COUNTER=`grep "REQ"  ${CFG_FILE} | wc -l`
	grep "REQ"  ${CFG_FILE} > tmep
	echo ${CASE_COUNTER}
}

get_cmd()
{
	#AAA=$(sed -n '1p' tmp)
	AAA=`sed -n ${1}p tmep`
	AAA=${AAA#*:}
	echo ${AAA}
	${AAA}
}

main()
{
	i=1
	get_case_counter
	while [ $i -le $CASE_COUNTER ]
	do
		get_cmd $i
		${AAA}
		let "i+=1"
	done
}

main