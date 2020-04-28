echo "---------------------"
echo "Will RUN Orpic Server"
echo "---------------------"
echo ""
echo "The log information will be sent to orpic.log file"
echo ""
cd src
nohup flask run > orpic.log 2>&1 &
cd ..
echo ""
echo "-------------------------------------------"
echo "To Stop the process you have to kill it !!!"
echo "             -There are 2 process to kill- "
echo "-------------------------------------------"
echo "ps -ef | grep flask"
echo "kill -9 [pid]"
