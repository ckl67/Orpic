# Orpic
Acronyme d'Oric et Raspberry Pi
Orpic est un serveur Web permettant d'héberger des jeux pour Oric/Oric Atmos.

Cette application est basée sur un serveur Web python dont l'ossature est basée sur Flask, et sur une conversion .tap --> .wav écrit en C.

# Utilisation
Lancement manuel.
Une fois l'environement python activé à travers 
    source venv/bin/activate
Il suffit de lancer (sous le répertoire .src)
    flak run
    
# Serveur
L'avantage du sereur Web, est de pouvoir présenter les jeux d'une façon plus ludique, mais surtout d'avoir une possibiité d'ajouter à l'infinie des Jeux.

## Ajout d'un Jeu
Pour ajouter un jeu, il suffit de copier le répertoire jeu que l'on peut récupérer par exemple sur le site "le-grenier-informatique.fr" et copier ce jeu au niveau répertoire /static/Tapes/<nouveau jeu>.
Par la suite y déposer le fichier .tap et d'y ajouter une image en .jpg ou .png

# Programme tap2wav
Le programme tap2 wav, peut bien entendu être utilisé tout seul
Une version linux (Ubuntu) et windows est disponible

    Usage : tap2wav
    Option -f : frequence 
       4 --> produces a 4800 Hz WAV file (Default) 
       8 --> produces a 8000 Hz WAV file 
       11 --> produces a 11025Hz WAV file  
       44 --> produces a 44100Hz WAV file  
    Option -s : split output file 
        yes/y--------> Will split the .WAV files in as many files than .TAP file is containing programs
                       The output .WAV file will end with an index number 
        no/n---------> There will be only 'one' .WAV file containing all the program 
        yes/no/y/n --> A silence zone will be added at the end of the different parts 
    Option -b : mean baud rate value 
        n  --> normal          : ~2000 Baud (default) for 4800Hz or 8000Hz or 11025Hz 
                l------------> : ~2500 Baud (default) for 44100Hz 
        f  --> fast            :                        
                |------------> : ~2500 Baud for  8000Hz  
                |------------> : ~3100 Baud for 11025Hz  
                l------------> : ~3400 Baud for 44100Hz  
    Option -e : Empty duration in "seconds" (silence 0x80) in .wav file at: 
                -the end of the file  
                -between 2 secions in the .Tap file 
    Option -v : version 
    Option -h : help 


