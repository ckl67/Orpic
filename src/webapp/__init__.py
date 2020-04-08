from flask import Flask
from webconfig import Config
from webapp.common import renameAllFilesInSubDir, renameDirectoriesIn
import os

# ======================================================================
# Default application root folder to construct static and templates path
# ======================================================================
# 	Based on the Flask documentation:
# 		app = Flask(__name__)
# 		The __name__ variable passed to the Flask class is a Python predefined variable, which is set to the name 
# 		of the module in which it is used. 
# 		Flask uses the location of the module passed here as a starting point when it needs to load associated 
# 		resources such as template files = (html) or Static (css,..)
#               Mapping URL <==> Directory 
#
#               Let's say you have a static file mapping on the "Web" page that maps
#               /static/ (URL) to /home/yourusername/yoursite/static (DIR)
#
#               If a request comes in for
#                   http://yourusername.pythonanywhere.com/static/images/something.png,
#               the system will scan the static file mappings, see the one you've set up, and will then look for a file
#                  /home/yourusername/yoursite/static/images/something.png.
#
# 	As: __init__.py is located in /src/webapp 
#                 /static/ (URL) will be mapped to ./src/webapp/static (DIR)
#              For instance
#                    <link rel="stylesheet" href="static/style.css" is working 
#                    but   
#                    <link rel="stylesheet" href="{{ url_for('static', filename='style.css') }}"> is preffered
#       BE CAREFUL with the python application which takes the root reference on .src/
#                information got from : os.getcwd ()
# ======================================================================

renameDirectoriesIn("webapp/static/Tapes/"," ","_")
renameAllFilesInSubDir("webapp/static/Tapes/"," ","_")

app = Flask(__name__)
app.config.from_object(Config)

# Must be at the END of the script !!
from webapp import routes



