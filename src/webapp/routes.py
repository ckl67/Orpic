from flask import render_template,request, redirect
from random import randrange
from webapp import app
from webapp.forms import preferenceForm, idForm2Val_Frequence, idForm2Val_Split, idForm2Val_Baud
from webapp.common import createNestedDict, formatCmd2os, pFlaskAppConf, renSubDir, renFileSubdir, pDbg1, pDbg2
import os

# ######################################################
# Global Vars
# Reference of Python root here is /src
#    Remark :  /static/ (URL) is mapped to ./src/webapp/static (DIR)
# ######################################################

# Create nested data dictionary:
tapesNL = createNestedDict("webapp/static/Tapes/","static/Tapes/")
pDbg1("() tapesNL= {}".format(tapesNL))

audioNL = createNestedDict("webapp/static/Audio/","static/Audio/")
pDbg1("() audioNL= {}".format(audioNL))

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


@app.route('/reload')
def reload():
    global tapesNL

    pDbg1("/Reload")

    # Rename Directories
    pDbg1("(reload) Rename Directories : webapp/static/Tapes/ ")
    renSubDir("webapp/static/Tapes/"," ","_")

    # Rename all files in Sub Directories
    pDbg1("(reload) Rename all files in Sub Directories : webapp/static/Tapes/")
    renFileSubdir("webapp/static/Tapes/"," ","_")

    pDbg1("(reload) Create tapes Nested List ")
    tapesNL = createNestedDict("webapp/static/Tapes/","static/Tapes/")
    pDbg2("(reload) tapesNL= {}".format(tapesNL))

    return redirect('/')

# ######################################################
# http://localost?tapeId=1&tapeAudioId=1 
# ######################################################
@app.route('/')
def home():
    global audioNL

    pFlaskAppConf(app.config,False)

    templateData = {
        "TapeId": 0,         # Tape ID 
        "TapeAudioId":0,     # Tape Audio ID --> We can have several Tapes files (each of them can possibly be splited in several wav files)
        "TapesNL" : tapesNL,
        "AudioNL": audioNL,
        "AppConfig": app.config
    }

    # Tape index 
    tapeIdx = request.args.get('tapeId', default = 0, type = int)
    # Record Index
    tapeAudioIdx = request.args.get('tapeAudioId', default = 0, type = int)
  
    if tapeIdx != 0:
        # Flask-Caching instance test 
        # cache.clear()
        # Clear cache seams not to work !!
        # We will generate a random audio file name
                
        templateData["TapeId"] = tapeIdx
        templateData["TapeAudioId"] = tapeAudioIdx
        pDbg2("(home) tapeId     : {}".format(tapeIdx))
        pDbg2("(home) tapeAudioId: {}".format(tapeAudioIdx))
        pDbg2("(home) Name       : {}".format(tapesNL[tapeIdx]["name"][0]))
        pDbg2("(home) Directory  : {}".format(tapesNL[tapeIdx]["directory_src"][0]))
        pDbg2("(home) Tape File  : {}".format(tapesNL[tapeIdx]["tap_file"][tapeAudioIdx]))

        playFile = "{0}{1}".format(tapesNL[tapeIdx]["directory_src"][0],tapesNL[tapeIdx]["tap_file"][tapeAudioIdx] )
        audio_out = tapesNL[tapeIdx]["name"][0]
        audio_out = audio_out.replace(" ","")
        audio_out = audio_out.replace("'","")
        audio_out = audio_out.replace(",","")
        audio_out = audio_out.replace("(","")
        audio_out = audio_out.replace(")","")
        audio_out = "{0}_{1}.wav".format(audio_out,randrange(5000))
        cmd = formatCmd2os(
            "../bin/tap2wav -i {0} -o webapp/static/Audio/AudioTape/{4} -f {1} -s {2} -b {3} -e 3"
            .format(
                playFile,
                idForm2Val_Frequence(app.config["TAP2WAV_FORM_FREQUENCE_ID"]),
                idForm2Val_Split(app.config["TAP2WAV_FORM_SPLIT_ID"]),
                idForm2Val_Baud(app.config["TAP2WAV_FORM_BAUD_ID"]),
                audio_out
            )
        )
        # Force Delete files
        os.system("rm -f webapp/static/Audio/AudioTape/*.wav")
        # Create Audio Files
        pDbg2("(home) run: {0}".format(cmd))
        os.system(cmd)
        # Create Nested Dictionnary
        audioNL = createNestedDict("webapp/static/Audio/","static/Audio/")
        pDbg2("(home) audioNL= {}".format(audioNL))

        templateData["AudioNL"] = audioNL
               
    # Pass the template data into the template main.html and return it to the user
    return render_template("main.html", **templateData)

@app.route('/preferences', methods=['GET', 'POST'])
def preference():
    form = preferenceForm(
        frequence=app.config["TAP2WAV_FORM_FREQUENCE_ID"],
        split=app.config["TAP2WAV_FORM_SPLIT_ID"],
        baud=app.config["TAP2WAV_FORM_BAUD_ID"]
    )
   
    if form.validate_on_submit():
        app.config["TAP2WAV_FORM_FREQUENCE_ID"] = form.frequence.data
        app.config["TAP2WAV_FORM_SPLIT_ID"] =  form.split.data
        app.config["TAP2WAV_FORM_BAUD_ID"] = form.baud.data

        pFlaskAppConf(app.config,False)

        return redirect('/')
    return render_template("preference.html", AppConfig=app.config, form=form)
