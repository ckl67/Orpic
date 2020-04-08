from flask import Flask
from webconfig import Config
from webapp.common import renameAllFilesInSubDir, renameDirectoriesIn
import os



# Default application root folder to construct static and templates path
# 	Based on the Flask documentation:
# 		app = Flask(__name__)
# 		The __name__ variable passed to the Flask class is a Python predefined variable, which is set to the name 
# 		of the module in which it is used. 
# 		Flask uses the location of the module passed here as a starting point when it needs to load associated 
# 		resources such as template files,..
#
# 	As: __init__.py is located in /src/webapp: the starting point is here : /src/webapp
#
#       --> ALL MUST BE REFERENCES RELATIVE TO /src/webapp

# print("====================HELLOOOO " +os.getcwd() )
# renameDirectoriesIn(os.getcwd()+"/webapp/","static/Tapes/"," ","_")
# renameAllFilesInSubDir(os.getcwd()+"/webapp/","static/Tapes/"," ","_")

app = Flask(__name__)
app.config.from_object(Config)


# Must be at the END of the script !!
from webapp import routes



