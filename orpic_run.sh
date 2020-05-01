#!/bin/bash
ipvar=$(ip addr | grep 'state UP' -A2 | tail -n1 | awk '{print $2}' | cut -f1  -d'/')
echo "---------------------"
echo "Will START Orpic Server"
echo "---------------------"
echo ""
echo "The log information will be sent to orpic.log file"
echo ""
cd src
source venv/bin/activate
nohup flask run > orpic.log 2>&1 &
cd ..
echo ""
echo "----------------"
echo " For information"
echo "-----------------"
echo "     To Stop Orpic Server you have to kill it !!!"
echo "             (There are 2 process to kill)"
echo "     Use following command "
echo "       * to identify pid and"
echo "       * to kill the flask process"
echo "     -------------------------------------------"
echo "     ps -ef | grep flask"
echo "     kill -9 [pid]"
echo ""
echo ""
echo "--------------------------------------"
echo " SERVER IS RUNNING AND ACCESSIBLE VIA"
echo " http://$ipvar:8080"
echo "--------------------------------------"
