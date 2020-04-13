from ctypes import *

# gcc -shared -O2 -o tap2wavlib.so -fPIC tap2wav.c
#t2wlib = cdll.LoadLibrary('./tap2wavlib.so')
#t2w_version = str(t2wlib.get_version())
## FONCTIONNE PAS POUR L'INSTANT

class Config(object):
    WEBAPP_DEBUG_LEVEL = 0
    WEBAPP_VERSION = "1.00"
    TAP2WAV_VERSION = "1.12"
    SECRET_KEY = 'you-will-never-guess'
