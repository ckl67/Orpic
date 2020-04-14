from flask import render_template,request, redirect
from webapp import app
from webapp.common import createNestedDict, printNestedDict, formatCmd2os, printdebug
from webapp.forms import preferenceForm
import os

# ######################################################
# Global Vars
# ######################################################

# Create Tapes nested data dictionary:
# Reference of Python root here is /src
# Remark :  /static/ (URL) is mapped to ./src/webapp/static (DIR)
#    As Tapes are located in static/Tapes (DIR) --> parameter dirUrlStatic =static/Tapes
#     key: tapes[directory_url_static] = static/Tapes/ImageDirectory
tapes = createNestedDict("webapp/static/Tapes/","static/Tapes/")

tap2wavParameters = {
    "pfrequence" : "4",
    "psplit" : "y",
    "pbaud" : "n"
}

def printTap2wavParams():
    '''
    Print all parameters which will be applied for tap2wav
    '''
    print("pfrequence: ", tap2wavParameters["pfrequence"])
    print("psplit: " , tap2wavParameters["psplit"])
    print("pbaud: ", tap2wavParameters["pbaud"])

# http://localost?tapeIdx=1&tapeAudioIxd=1 
@app.route('/')
def home():

    audioFiles = {1 : {"name": ["AudioTape"],
                   "directory_src": ["webapp/static/Audio/AudioTape/"],
                   "directory_url_static": ["static/Audio/AudioTape/"],
                   "wav_file":[""],
                   "wav_nb":[0] }}

    templateData = {"tapes" : tapes,
                    "tapeIdx": 0,
                    "tapeAudioIxd":0,
                    "audioFiles": audioFiles,
                    "webappVersion" : app.config["WEBAPP_VERSION"],
                    "tap2wavVersion" : app.config["TAP2WAV_VERSION"],
                    "tap2wavpfrequence" : tap2wavParameters["pfrequence"],
                    "tap2wavpsplit" : tap2wavParameters["psplit"],
                    "tap2wavpbaud" : tap2wavParameters["pbaud"]
    }

    # Tape index 
    tapeIdx = request.args.get('tapeIdx', default = 0, type = int)
    # Record Index
    tapeAudioIxd = request.args.get('tapeAudioIxd', default = 0, type = int)
  
    if tapeIdx != 0:
        templateData["tapeIdx"] = tapeIdx
        templateData["tapeAudioIxd"] = tapeAudioIxd
        print("------------PLAY----------------")
        print("tapeIdx   : ",tapeIdx)
        print("tapeAudioIxd  : ",tapeAudioIxd)
        print("Name      : ",tapes[tapeIdx].get("name")[0])
        print("Directory : ",tapes[tapeIdx].get("directory_src")[0])
        print("Play Tape : ",tapes[tapeIdx].get("tap_file")[tapeAudioIxd])

        # We are using ative play linux player
        # Installed with : sudo apt-get insatll sox
        # syntax: play [file name]
        playFile = "{0}{1}".format(tapes[tapeIdx].get("directory_src")[0],tapes[tapeIdx].get("tap_file")[tapeAudioIxd] )
        cmd = formatCmd2os("../bin/tap2wav -i {0} -o webapp/static/Audio/AudioTape/audio.wav -f {1} -s {2} -b {3} -e 3".format(
            playFile,tap2wavParameters["pfrequence"],
            tap2wavParameters["psplit"],
            tap2wavParameters["pbaud"]))
        printdebug(cmd)
        # Force Delete directory
        os.system("rm -R -f webapp/static/Audio/")
        # Create Directory
        os.system("mkdir webapp/static/Audio")
        os.system("mkdir webapp/static/Audio/AudioTape")
        # Create Audio Files
        os.system(cmd)
        # Create Nested Dictionnary
        audioFiles = createNestedDict("webapp/static/Audio/","static/Audio/",True)
        templateData["audioFiles"] = audioFiles
        
        print("------------AUDIO----------------")
        print("Nb Audio Files : ",audioFiles[1].get("wav_nb")[0])
        print("Audio Files : ",audioFiles[1].get("wav_file"))
        print("tapeIdx: ", tapeIdx)
        print("tapeAudioIxd: ",tapeAudioIxd)
        
        # os.system(f"play {playFile}")
        
    # Pass the template data into the template main.html and return it to the user
    return render_template("main.html", **templateData)

@app.route('/preferences', methods=['GET', 'POST'])
def preference():
    form = preferenceForm()  
    if form.validate_on_submit():
        print("debug",form.frequence.data)
        
        if form.frequence.data == 1:
            tap2wavParameters["pfrequence"] = "4"
        elif form.frequence.data == 2:
            tap2wavParameters["pfrequence"] = "8"
        elif form.frequence.data == 3:
            tap2wavParameters["pfrequence"] = "11"
        else:
            tap2wavParameters["pfrequence"] = "44"

        if form.split.data == 1:
            tap2wavParameters["psplit"] = "y"
        else:
            tap2wavParameters["psplit"] = "n"
        
        if form.baud.data == 1:
            tap2wavParameters["pbaud"] = "n"
        else:
            tap2wavParameters["pbaud"] = "f"

        return redirect('/')
    # Pass the template data into the template main.html and return it to the user
    #return render_template("main.html", **templateData)
    return render_template("preference.html", title='Sign In', form=form)
