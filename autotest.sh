#!/bin/bash

#set -x

CFG_FILE="case.cfg"
DBUS_CMD=""
TEST_RESULT_FILE="result.log"

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
    CMD=`sed -n ${1}p tmep`
    CMD=${CMD#*:}
    echo ${CMD}
    ${CMD}
}

wait_resp()
{
    while true
    do
        RES=`cat ${TEST_RESULT_FILE}`
        if [ $RES -eq 1 ] 
        then
            break
        fi
    done
    echo -n "0" > ${TEST_RESULT_FILE}
}

main()
{
    if [ -f ${TEST_RESULT_FILE} ]
    then
        echo -n "0" > ${TEST_RESULT_FILE}
        echo "${TEST_RESULT_FILE} is not exits to create"
    else
        touch ${TEST_RESULT_FILE}
        echo -n "0" > ${TEST_RESULT_FILE}
    fi
    i=1
    get_case_counter
    while [ $i -le $CASE_COUNTER ]
    do
        get_cmd $i
        wait_resp
        let "i+=1"
    done
}

main
