#!/bin/bash
echo "--------------------------------------------------"
echo "    This procedure will install Orpic Server"
echo "  Please to wait until installation is finished"
echo "--------------------------------------------------"

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
