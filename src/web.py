# March 2020
# Christian Klugesherz
# Development environment has to be activated
#    .venv/bin/activate

from flask import Flask, render_template, request
import os
import common

# We create an instance of the class Flask
# The first argument is the name of the application’s
# Using a single module we should use __name__ because depending on if it’s started as application
# ('__main__' versus the actual import name).
# This is needed so that Flask knows where to look for templates, static files, and so on.
app = Flask(__name__)

# Create a dictionary of all the tapes to display
tapes = {}

VERSION = "1.0"

@app.route("/")
def main():
   # Put the dictionary into the template data dictionary:
   templateData = {
      "tapes" : tapes
   }
   
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
      cmd = "../bin/tap2wav -i {0} -o ../data/audio.wav".format(common.formatCmd(playFile))
      print(cmd)
      os.system(cmd)
      # os.system(f"play {playFile}")
      
   # Pass the template data into the template main.html and return it to the user
   return render_template("main.html", **templateData)

@app.route('/progress')
def progress():
   
   def generate():
      x = 0
      while x <= 100:
         vprg = "data:" + str(x) + "\n\n"  
         yield vprg 
         x = x + 10
         time.sleep(0.5)
         
   return Response(generate(), mimetype= 'text/event-stream')


if __name__ == "__main__":
   common.renameFiles("static/Tapes/"," ","_")
   tapes = common.createNestedDict("static/Tapes/")
   
   app.run(host="0.0.0.0", port=8080, debug=True)

