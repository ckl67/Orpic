# Orpic
Permet d'émuler une cassette pour envoyer des fichiers .TAP sur Oric

	Usage : 
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

