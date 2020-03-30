# Orpic
Permet d'émuler une cassette pour envoyer des fichiers .TAP sur Oric


 Usage: tap2wav.exe -i <.TAP file> -o <.WAV file> -f [4, 8 or 11] -s [yes/no/y/n]
 -b [lf,f,sf,ssf]
   Option -f : frequence
        4 --> produces a 4800 Hz WAV file (Default)
        8 --> produces a 8000 Hz WAV file
        11 --> produces a 11025Hz WAV file
   Option -s : split output file
         yes/y--------> Will split the .WAV files in as many files than .TAP file
 is containing programs
                       The output .WAV file will end with an index number
        no/n---------> There will be only 'one' .WAV file containing all the pro
gram
        yes/no/y/n --> A silence zone will be added at the end of the different
parts
  Option -b : mean baud rate value
        n  --> normal          : ~2000 Baud (default)
       lf  --> low fast        : ~2500 Baud (for 8000Hz or for 11025Hz)
        f  --> fast            : ~2700 Baud (only for 11025Hz)
       sf  --> super fast      : ~2900 Baud (only for 11025Hz)
      hsf  --> high super fast : ~3200 Baud (only for 11025Hz)
