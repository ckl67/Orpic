#!/bin/bash
echo "--------------------------------------------------"
echo "    This procedure will install Orpic Server"
echo "  Please to wait until installation is finished"
echo "--------------------------------------------------"
echo ""
echo "Which target : (U)buntu or (R)aspberry: (u/r)?"
read vartarget

if [ "$vartarget" = "r" ]
then
    echo "Target = Raspberry"
    cp bin/tap2wav_rasp bin/tap2wav
else
    echo "Target = Ubunutu"   
    cp bin/tap2wav_ubuntu bin/tap2wav
fi

exit

echo "Goto src"

cd src

echo "Create Python environment venv"
python3 -m venv venv

echo "Activate Python Environment"
source venv/bin/activate

echo "Install python packages"
pip3 install -r requirements.txt

cd ..

echo " "
echo " "
echo "Now you can run Orpic Server with : "
echo "source run.sh"
echo ""
echo "     To deactivate the Python environement, execute : deactivate"
echo ""
echo "                                                     Have Fun @CKL !!" 
