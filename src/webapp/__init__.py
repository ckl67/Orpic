from flask import Flask
from webconfig import Config
from webapp.common import renameFiles

renameFiles("webapp/static/Tapes/"," ","_")

app = Flask(__name__,static_url_path="/webapp/static")
#app = Flask(__name__)
app.config.from_object(Config)

# Must be at the END of the script !!
from webapp import routes



