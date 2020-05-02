#!/bin/bash
echo " ----------------------------------------------------------"
echo "|    This procedure will Install or Update Orpic Server    |"
echo "|  Please to wait until installation or Update is finished |"
echo " -----------------------------------------------------------"
echo ""
echo "Target directory is ./Orpic (Enter)"
read dirtarget
OrpicDir=$(pwd)"/Orpic"
echo "Destination directory = $OrpicDir"
echo ""
echo "Which target : (U)buntu or (R)aspberry: (u/r)?"
read vartarget

if [ -d "$OrpicDir" ] 
then
    echo "Directory $OrpicDir exists --> Orpic will be Updated"
    cd Orpic
    git pull   
else
    echo "Directory $OrpicDir does not exist --> Orpic will be installed ."
    git clone https://github.com/ckl67/Orpic.git
    cd Orpic  
fi

if [ "$vartarget" = "r" ]
then
    echo "Target = Raspberry"
    cp bin/tap2wav_rasp bin/tap2wav
else
    echo "Target = Ubunutu"   
    cp bin/tap2wav_ubuntu bin/tap2wav
fi

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
echo "Now you can run Orpic Server with command: "
echo "    source orpic_run.sh"
echo ""
echo "     To deactivate the Python environement, execute : deactivate"
echo ""
echo "                                                     Have Fun @CKL !!" 
