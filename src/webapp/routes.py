from flask import render_template,request
from webapp import app
from webapp.common import createNestedDict, printNestDict

# ######################################################
# Global Vars
# ######################################################

# Put the dictionary into the template data dictionary:
tapes = createNestedDict("webapp/static/Tapes/")

@app.route('/')
def home():

    printNestDict(tapes)

    templateData = {"tapes" : tapes }

    # http://localost?tapeIdx=1&audioIdx=1
    # Tape index 
    tapeIdx = request.args.get('tapeIdx', default = 0, type = int)
    # Record Index
    audioIdx = request.args.get('audioIdx', default = 0, type = int)
   
    if tapeIdx != 0:
        print("------------PLAY----------------")
        print("tapeIdx   : ",tapeIdx)
        print("audioIdx  : ",audioIdx)
        print("Name      : ",tapes[tapeIdx].get("name")[0])
        print("Directory : ",tapes[tapeIdx].get("directory_src")[0])
        print("Play Tape : ",tapes[tapeIdx].get("tap_file")[audioIdx])

        playFile = "{0}{1}".format(tapes[tapeIdx].get("directory_src")[0],tapes[tapeIdx].get("tap_file")[audioIdx] )
        print("Play File : ",playFile)

        # We are using ative play linux player
        # Installed with : sudo apt-get insatll sox
        # syntax: play [file name]
        cmd = "../bin/tap2wav -i {0} -o ../data/audio.wav".format(playFile)
        print(cmd)
        # os.system(cmd)
        # os.system(f"play {playFile}")

        
    # Pass the template data into the template main.html and return it to the user
    #return render_template("main.html")
    return render_template("main.html", **templateData)

@app.route('/preference')
def preference():
    
    # Pass the template data into the template main.html and return it to the user
    #return render_template("main.html", **templateData)
    return render_template("main.html")
