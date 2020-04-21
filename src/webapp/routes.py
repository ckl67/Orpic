from flask import render_template,request, redirect
from webapp import app
from webapp.forms import preferenceForm, idForm2Val_Frequence, idForm2Val_Split, idForm2Val_Baud
from webapp.common import createNestedDict, formatCmd2os, pDbg2
import os

# ######################################################
# Global Vars
# Reference of Python root here is /src
#    Remark :  /static/ (URL) is mapped to ./src/webapp/static (DIR)
# ######################################################

# Create nested data dictionary:
tapesNL = createNestedDict("webapp/static/Tapes/","static/Tapes/")
audioNL = createNestedDict("webapp/static/Audio/","static/Audio/")

# ######################################################
# Allows to Call a python function from jinja2
# ######################################################
@app.context_processor
def utility_processor():
    def FidForm2Val_Split(id):
        return (idForm2Val_Split(id))

    def FidForm2Val_Frequence(id):
        return (idForm2Val_Frequence(id))

    def FidForm2Val_Baud(id):
        return (idForm2Val_Baud(id))

    return dict(FidForm2Val_Frequence=FidForm2Val_Frequence,
                FidForm2Val_Split=FidForm2Val_Split,
                FidForm2Val_Baud=FidForm2Val_Baud)


# ######################################################
# http://localost?tapeId=1&tapeAudioId=1 
# ######################################################
@app.route('/')
def home():
    global audioNL
  
    templateData = {
        "TapeId": 0,         # Tape ID 
        "TapeAudioId":0,     # Tape Audio ID --> We can have several Tapes files (each of them can be splited in several wav files)
        "TapesNL" : tapesNL,
        "AudioNL": audioNL,
        "AppConfig": app.config
    }

    # Tape index 
    tapeIdx = request.args.get('tapeId', default = 0, type = int)
    # Record Index
    tapeAudioIdx = request.args.get('tapeAudioId', default = 0, type = int)
  
    if tapeIdx != 0:
        templateData["TapeId"] = tapeIdx
        templateData["TapeAudioId"] = tapeAudioIdx
        print("------------PLAY----------------")
        print("tapeId   : ",tapeIdx)
        print("tapeAudioId  : ",tapeAudioIdx)
        print("Name      : ",tapesNL[tapeIdx]["name"][0])
        print("Directory : ",tapesNL[tapeIdx]["directory_src"][0])
        print("Play Tape : ",tapesNL[tapeIdx]["tap_file"][tapeAudioIdx])

        # We are using ative play linux player
        # Installed with : sudo apt-get insatll sox
        # syntax: play [file name]
        playFile = "{0}{1}".format(tapesNL[tapeIdx]["directory_src"][0],tapesNL[tapeIdx]["tap_file"][tapeAudioIdx] )
        cmd = formatCmd2os(
            "../bin/tap2wav -i {0} -o webapp/static/Audio/AudioTape/audio.wav -f {1} -s {2} -b {3} -e 3"
            .format(
                playFile,
                app.config["TAP2WAV_FORM_FREQUENCE_ID"],
                app.config["TAP2WAV_FORM_SPLIT_ID"],
                app.config["TAP2WAV_FORM_BAUD_ID"]
            )
        )
        print(cmd)
        # # Force Delete directory
        # os.system("rm -R -f webapp/static/Audio/")
        # # Create Directory
        # os.system("mkdir webapp/static/Audio")
        # os.system("mkdir webapp/static/Audio/AudioTape")
        # # Create Audio Files
        # os.system(cmd)
        # # Create Nested Dictionnary
        # audioNL = createNestedDict("webapp/static/Audio/","static/Audio/")
        # templateData["AudioNL"] = audioNL
        
        # print("------------AUDIO----------------")
        # print("Nb Audio Files : ",audioNL[1].get("wav_nb")[0])
        # print("Audio Files : ",audioNL[1].get("wav_file"))
        # print("tapeIdx: ", tapeIdx)
        # print("tapeAudioIdx: ",tapeAudioIdx)
        
        # # os.system(f"play {playFile}")
        
    # Pass the template data into the template main.html and return it to the user
    return render_template("main.html", **templateData)

@app.route('/preferences', methods=['GET', 'POST'])
def preference():
    print(app.config["TAP2WAV_FORM_FREQUENCE_ID"])
    form = preferenceForm()  
    if form.validate_on_submit():
        app.config["TAP2WAV_FORM_FREQUENCE_ID"] = form.frequence.data
        app.config["TAP2WAV_FORM_SPLIT_ID"] =  form.split.data
        app.config["TAP2WAV_FORM_BAUD_ID"] = form.baud.data

        print(app.config["TAP2WAV_FORM_FREQUENCE_ID"])

        return redirect('/')
    # Pass the template data into the template main.html and return it to the user
    #return render_template("main.html", **templateData)
    return render_template("preference.html", AppConfig=app.config, form=form)
