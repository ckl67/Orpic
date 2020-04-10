https://www.pythonanywhere.com/forums/topic/12195/
https://blog.miguelgrinberg.com/post/the-flask-mega-tutorial-part-i-hello-world

.flaskenv
	FLASK_APP=web.py
		Va configurer flask, pour indiquer qu'un : "flask run" va lancer web.py

/webbapp
	Ce réperoire est un package, reconnu à travers le présence du fichier : __init__.py
	dans le répertoire

webconfig.py
	class Config(object):
	    DEBUG = "YES"
		Ce module va rassembler toute la config de l'application
		En d'autres termes de Constante

		peut être utilisé par la suite ) travers

		from webapp import app
		app.config['DEBUG']

web.py
	from webapp import app
		cette instruction importe la variable app qui est membre du package webapp
		==> Donc à fortiori va executer __init__.py

__init__.py
	from flask import Flask
	app = Flask(__name__)
		Le script ci-dessus crée simplement l'objet d'application : app
		en tant qu'instance de classe Flask importée du package flask
		La variable __name__ transmise à la classe Flask est une variable prédéfinie Python, qui définie le nom du module
		dans lequel cette variable est utilisée. 
		Flask utilise l'emplacement du module passé ici, comme point de départ lorsqu'il doit charger des ressources fichiers 

		Comme: __init__.py se trouve dans /src/webapp: le point de départ est ici: /src/webapp

			Supposons que vous ayez un mappage de fichier statique sur la page "Web" qui mappe 
			l'url 		: /static/ 
			vers
			le répertoire 	: /home/votre nom d'utilisateu/votre site/static

			Si une demande de 
			* http://yourusername.pythonanywhere.com/static/images/something.png 
			arrive, le système analysera les mappages de fichiers statiques, verra celui que vous avez configuré, 
			puis recherchera un fichier 
			* /home/votre nom d'utilisateur/votre site/statique/images/something.png.

			Comme: __init__.py se trouve dans /src/ webapp:
			--> /static/ (l'URL) sera mappé sur ./src/webapp/static (le répertoire)

			Par exemple
			 <link rel = "stylesheet" href = "static / style.css" fonctionne
			mais
			 <link rel = "stylesheet" href = "{{url_for ('static', filename = 'style.css')}}"> est préféré

		ATTENTION avec l'application python qui elle prend la référence root sur .src/
			information que remonte: os.getcwd()

	app.config.from_object(Config)
		Plutôt que de créer des Constantes de l'application à travers
		app.config['..'] = '..' 
		Il est préférable de passer par un modue externe webconfig.py
		Par la suite nous pouvons appeler ces constante à travers
		app.config['DEBUG']

	from webapp import routes
		L'application importe ensuite le module routes

		Le module routes est importé en bas et non en haut du script 
		L'importation inférieure est une solution de contournement aux importations circulaires !!
		En effet, Le module routes a besoin d'importer la variable d'application "app" définie dans ce script

routes.py
	from flask import render_template
	from webapp import app

	@app.route('/')
	def home():

		Les itinéraires sont les différentes URL que l'application implémente. 
		Dans Flask, les gestionnaires des routes d'application sont écrits en tant que fonctions Python, 
		appelées fonctions d'affichage. 
		Les fonctions d'affichage sont mappées sur une ou plusieurs URL de route afin que Flask sache 
		quelle logique exécuter lorsqu'un client demande une URL donnée.




