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




