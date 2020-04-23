# ######################################################
# Christian Klugesherz
# March 2020
# ######################################################
from os import listdir,rename,getcwd
from os.path import isfile, isdir, join, splitext, exists
import sys

# ######################################################
# Global Vars
# ######################################################

APPLICATION_DEBUG_LEVEL = 0

# ######################################################
# Functions Prototype
# ######################################################
# def setApplicationDebugLevel(val):
# def getApplicationDebugLevel():
# def pError(text):
# def pDbg(text,functDlv=0,appDLv=0):
# def pDbg0(text):
# def pDbg1(text):
# def pDbg2(text):
# def pDbg3(text):
# def pDbg4(text):
# def pDbg5(text):
# def renFileDir(dirin,charin,charout):
# def renFileSubdir(dirin,charin,charout):
# def cntExtFileDir(dirin,fext):
# def renSubDir(dirin,charin,charout):
# def formatCmd2os(cmd):
# def createNestedDict(dirin,dirUrl):
# def printNestedDict(oricdic):
# def pFlaskAppConf(my_dict, printAll=False):
   
# ######################################################
# Functions
# ######################################################

# -----------------------------------------------------
# Set and Get application Debug Level
# -----------------------------------------------------

def setApplicationDebugLevel(val):
    ''' 
    set Global variable: APPLICATION_DEBUG_LEVEL
    '''
    global APPLICATION_DEBUG_LEVEL
    APPLICATION_DEBUG_LEVEL = val
    pDbg1("(setApplicationDebugLevel) Set Application Debug Level: {0}".format(APPLICATION_DEBUG_LEVEL))

def getApplicationDebugLevel():
    ''' 
    get Global variable: APPLICATION_DEBUG_LEVEL
    '''
    pDbg1("(getApplicationDebugLevel) Debug Level: {0}".format(APPLICATION_DEBUG_LEVEL))
    return (APPLICATION_DEBUG_LEVEL)

# -----------------------------------------------------
# Print Error 
# -----------------------------------------------------
def pError(text):
    '''
    Print Error
    '''
    out = "!! ERROR: "+ text + " !!"
    for x in range(len(out)):
        print('-', end='')
    print()
    print(out)
    for x in range(len(out)):
        print('-', end='')
    print()
    
# -----------------------------------------------------
# Display the 'text' in the consol for debugging usage.
# text will be displayed as soon
# application debug level is >= than the level in the function
# -----------------------------------------------------
def pDbg(text,functDlv=0,appDLv=0):
    '''
    Display the 'text' in the consol for debugging usage.
    We have to consider 2 parameters :
      * appDLv 
            The Overall debug level needed by the application
            For exemple : app.config["WEBAPP_DEBUG_LEVEL"] = 3
      * functDlv
            The Debug level for information to display in the function 

       --> text will be displayed as soon application debug level is >= than the level in the function

    Exemple
       def test()
            ....    
            pDebg("Value of i="+i,3,appDLv)
            ....

       --> means if appDLv>=3, then the information will be dispayed, otherwise not
    '''
    if appDLv >= functDlv :
        print(text)
        
# -----------------------------------------------------
# Print debug level 0 == like print()
# BUT THIS FUNCTION CAN BE EMPTY ONCE THE CODE IS PERFECT
# IT IS A TRICK TO DISPLAY THE INFORMATION ONCE APPLICATION_DEBUG_LEVEL IS NOT KNOWN
# -----------------------------------------------------
def pDbg0(text):
    '''
    idem as pDbg(text,0,APPLICATION_DEBUG_LEVEL) 
    '''
    pDbg(text,0,APPLICATION_DEBUG_LEVEL)

        
# -----------------------------------------------------
# Print debug level 1
# -----------------------------------------------------
def pDbg1(text):
    '''
    idem as pDbg(text,1,APPLICATION_DEBUG_LEVEL)
    '''
    pDbg("  "+text,1,APPLICATION_DEBUG_LEVEL)
    
# -----------------------------------------------------
# Print debug level 2
# -----------------------------------------------------
def pDbg2(text):
    '''
    idem as pDbg(text,2,APPLICATION_DEBUG_LEVEL)
    '''
    pDbg("    "+text,2,APPLICATION_DEBUG_LEVEL)
    
# -----------------------------------------------------
# Print debug level 3
# -----------------------------------------------------
def pDbg3(text):
    '''
    idem as pDbg(text,3,APPLICATION_DEBUG_LEVEL)
    '''
    pDbg("      "+text,3,APPLICATION_DEBUG_LEVEL)

# -----------------------------------------------------
# Print debug level 4
# -----------------------------------------------------
def pDbg4(text):
    '''
    idem as pDbg(text,4,APPLICATION_DEBUG_LEVEL)
    '''
    pDbg("        "+text,4,APPLICATION_DEBUG_LEVEL)

# -----------------------------------------------------
# Print debug level 5
# -----------------------------------------------------
def pDbg5(text):
    '''
    idem as pDbg(text,5,APPLICATION_DEBUG_LEVEL)
    '''
    pDbg("          "+text,5,APPLICATION_DEBUG_LEVEL)

# -----------------------------------------------------
# function which rename all files in a directory by replacing : charin by charout
# -----------------------------------------------------
def renFileDir(dirin,charin,charout):
    """
    function which rename all files in a directory by replacing : charin by charout
    parameters
    * dirin : absolute or relative directory path
    * charin : characters to replace
    * charout : character to replace
    Return the number of files which have been converted, or -1 if issue
    """

    retval = 0
    pDbg4("(renFileDir): Rename all files containing '{0}' by '{1}' in directory {2}".format(charin,charout,dirin) )

    # test if Directory exists
    if not exists(dirin):
        pError("(renFileDir): chek if : " + dirin +" exists")
        retval = -1
        return retval
       
    for f in listdir(dirin): # lis all of item in a directory
        if isfile(join(dirin, f)): # Check if 'f' it is file
            pDbg5("(renFileDir): Checking file: "+ f)
            if f.find(charin) > 0:   # Check if in the file there is charin ?
                fout = f.replace(charin,charout)
                retval = retval+1
                pDbg5("(renFileDir): --> new file="+ fout)
                rename(join(dirin, f),join(dirin, fout))
    return retval

# -----------------------------------------------------
# function which rename all files in all Sub directory by replacing : charin by charout
# -----------------------------------------------------
def renFileSubdir(dirin,charin,charout):
    """
    function which will rename all files in all Sub directories of a Directory by replacing : charin by charout
    Parameters
    * dirin : absolute or relative directory path
    * charin : characters to replace
    * charout : character to replace
    Return the number of files which have been converted, or -1 if issue
    """

    retval = 0
    pDbg3("(renFileSubdir): Rename all files containing '{0}' by '{1}' in directory {2}".format(charin,charout,dirin) )

    if not exists(dirin):
        pError("(renFileSubdir): chek if : " + dirin +" exists")
        retval = -1
        return retval
    
    for d in listdir(dirin): # lis all of item in a directory
        pDbg4("(renFileSubdir): Checking dir :" + d)
        if isdir(join(dirin, d)): # Check if it is directory
            retval = retval + renFileDir(join(dirin, d),charin,charout)
    return retval


# -----------------------------------------------------
# function which retrurn the number of file type in a directory
# -----------------------------------------------------
def cntExtFileDir(dirin,fext):
    """
    function which return the number of file type in a directory 
    parameters
    * dirin : absolute or relative directory path
    * fext  : file extension
    Return the number of files which have been converted, or -1 if issue
    """

    retval = 0
    pDbg4("(cntExtFileDir): Count all files with extension '{0}' in directory {1}".format(fext,dirin) )

    # test if Directory exists
    if not exists(dirin):
        pError("(cntExtFileDir): chek if : " + dirin +" exists")
        retval = -1
        return retval
       
    for f in listdir(dirin): # lis all of item in a directory
        if isfile(join(dirin, f)): # Check if 'f' it is file
            pDbg5("(cntExtFileDir): Checking file: "+ f)
            filename, file_extension = splitext(f)
            if file_extension == fext :  
                retval = retval+1
    return retval


# -----------------------------------------------------
# function which rename all Directories in a directory by replacing : charin by charout
# -----------------------------------------------------
def renSubDir(dirin,charin,charout):
    """
    function which rename all Directories in a directory by replacing : charin by charout
    Parameters
    * dirin : absolute or relative directory path
    * charin : characters to replace
    * charout : character to replace
    Return the number of files which have been converted, or -1 if issue
    """

    retval = 0
    pDbg3("(renSubDir): Rename all directories containing '{0}' by '{1}' in directory {2}".format(charin,charout,dirin) )

    if not exists(dirin):
        pError("(renSubDir): chek if : " + dirin +" exists")
        retval = -1
        return retval
  
    for f in listdir(dirin): # lis all of item in a directory
        if isdir(join(dirin, f)): # Check if it is directory
            pDbg4("(renSubDir): Checking dir :" + f)
            if f.find(charin) > 0:   # Check if in the file there is charin ?
                fout = f.replace(charin,charout)
                rename(join(dirin, f),join(dirin, fout))
                pDbg4("(renSubDir): --> new directory ="+ fout)
                retval = retval+1
    return retval

# -----------------------------------------------------
#    Format the command to send to os.system
# -----------------------------------------------------
def formatCmd2os(cmd):
    '''
    Format the command to send to os.system
    Example: 
        Aigle_D'Or,_Le_(1983) is a correct name for a Directory, or file: This data is saved in oricdic
    However, for a Linux command the chars: "'", or "(", or ")" are not allowed alone, so we must add prefix "\"

    Auto-completion in Linux will give
        >>:~/Work/Orpic/src/webapp/static/Tapes$ ls Aigle_D\'Or,_Le_\(1984\)/

    For example we will replace an (apostrophe) ' by \' in order to make a "cd"
    '''
    pDbg3("(formatCmd2os): in="+ cmd)

    out = cmd
    out = out.replace("'","\\'")
    out = out.replace("(","\(")
    out = out.replace(")","\)")

    pDbg3("(formatCmd2os): out="+ out)
    return(out)

# -----------------------------------------------------
#    Create The Oric Nested dictionnary
# -----------------------------------------------------
def createNestedDict(dirin,dirUrl):
    '''
    Create The Oric Nested dictionnary
    Parameters
    * dirin : abslolute or relative path
    * dirUrl : URL path 
    Will create a Key for each item
    Each keay beeing a list !
    Return the nested directory 
    '''

    pDbg3("(createNestedDict): Create Nested Dictionary in Dir: '{0}' with URL: '{1}'".format(dirin,dirUrl) )

    # www.programiz.com/python-programming/nested-dictionary
    # people = {1: {'name': 'John', 'age': '27', 'sex': 'Male'},
    #          2: {'name': 'Marie', 'age': '22', 'sex': 'Female'}}
    
    odic = {}
    iddic = 0
    nbtap = 0
    nbwav = 0
    nbpicture = 0
    
    if not exists(dirin):
        pError("(createNestedDict): chek if : " + dirin +" exists")
        retval = odic
        return odic

    for d in listdir(dirin): # lis all of item in a directory
        pDbg4("(createNestedDict): Checking dir :" + d)
        if isdir(join(dirin, d)): # Check if it is directory
            iddic= iddic + 1
            odic[iddic] = {} # Create a new Dic -->  iddic : { } --> 1 : { }
            odic[iddic].setdefault('name',[]).append(d.replace("_"," ")) # Add a key in the dict as a list :  --> 1 : {"name": ["xxxxx"] }
            odic[iddic].setdefault('directory_src',[]).append(join(dirin, d)+"/")
            odic[iddic].setdefault('directory_url_static',[]).append(join(dirUrl, d)+"/")

            nbtap = 0
            nbwav = 0
            nbpicture = 0
            for f in listdir(join(dirin,d)): # lis all items in directory : d
                file_name, file_extension = splitext(join(dirin,f))

                if file_extension.lower() == ".wav":
                    nbwav = nbwav + 1
                    odic[iddic].setdefault('wav_file',[]).append(f) 
                    
                if file_extension.lower() == ".tap":
                    nbtap = nbtap + 1
                    odic[iddic].setdefault('tap_file',[]).append(f) 
                    
                if file_extension.lower() == ".jpg":
                    nbpicture =  nbpicture + 1
                    odic[iddic].setdefault('picture',[]).append(f)
                    
                if file_extension.lower() == ".png":
                    nbpicture =  nbpicture + 1
                    odic[iddic].setdefault('picture',[]).append(f)

            odic[iddic].setdefault('tape_nb',[]).append(nbtap)
            odic[iddic].setdefault('wav_nb',[]).append(nbwav)
            odic[iddic].setdefault('picture_nb',[]).append(nbpicture)

            # Sort List
            if nbtap > 0 :
                odic[iddic]["tap_file"].sort()
            if nbwav > 0:
                odic[iddic]["wav_file"].sort()
            if nbpicture > 0:
                odic[iddic]["picture"].sort()

    pDbg3("(createNestedDict) odic= {}".format(odic))
    # Sort Nested list by name not workinig here !!
    # odic = sorted(odic.items(), key = lambda x: x[1]['name']) 
    pDbg3("(createNestedDict) Sorted odic= {}".format(odic))
    
    if APPLICATION_DEBUG_LEVEL >= 5:
        printNestedDict(odic)

    return(odic)
	
# -----------------------------------------------------
# Print all elements in the Nested Dict
# -----------------------------------------------------
def printNestedDict(oricdic):
    '''
    will print all elements in the Nested Dict
    '''
    for  o_id,o_info in oricdic.items():
        print("\nDic id:",o_id)
        for key in o_info:
            # now we are at the list level of key in dict o_info[key]
            print(key + ":")
            for vlist in o_info[key]:
                print("   ->", vlist)

def pFlaskAppConf(my_dict, printAll=False):
    '''
    Print the configuration information for Flask app.Config instance !
    We will check the Debug Level to display the information
        "WEBAPP_DEBUG_LEVEL" > 1 then the information is displayed
    '''

    DList = [
        "WEBAPP_DEBUG_LEVEL",
        "WEBAPP_VERSION",
        "DEBUG",
        "TAP2WAV_FORM_BAUD_ID",
        "TAP2WAV_FORM_FREQUENCE_ID",
        "TAP2WAV_FORM_SPLIT_ID",
        "TAP2WAV_VERSION"
    ]

    if my_dict["WEBAPP_DEBUG_LEVEL"] > 1:
    
        if printAll == True:
            for item in my_dict:
                print("{0} = {1}".format(item,my_dict[item]))
        else:
            for item in my_dict:
                for vl in DList:
                    if vl == item:
                        print("{0} = {1}".format(item,my_dict[item]))
    
                    
# ######################################################
# TESTS
# ######################################################
           
if __name__ =='__main__':
    # Test print debug level functions
    setApplicationDebugLevel(0)
    print("\nWill test print debug level functions (pDbg) with overall Applicationn Debug Level={0}".format(getApplicationDebugLevel()))
    pError("Something is going wrong here")
    pDbg0("Debug level 0")
    pDbg1("Debug level 1")
    pDbg2("Debug level 2")
    pDbg3("Debug level 3")
    pDbg4("Debug level 4")
    pDbg5("Debug level 5")

    # Test Rename files in Directoy 
    print()
    setApplicationDebugLevel(0)
    print("Rename files in Directorie:")
    result = renFileDir("static/Tapes/Chess_II_(1984)"," ","_")
    print ("Number of convertion: {0}".format(result))

    # Test Rename files in Sub Directories
    print()
    setApplicationDebugLevel(0)
    print("Rename ALL files in Sub-Directorie:")
    result = renFileSubdir("static/Tapes"," ","_")
    print ("Number of convertion: {0}".format(result))

    # Count number of files in a directory
    print()
    setApplicationDebugLevel(0)
    print("Count number of files in a directory")
    vdir = "static/Tapes/Diamant_De_L'_ile_Maudite,_Le_(1984)/"
    result = cntExtFileDir(vdir,".tap")
    print("Number of files in: {0} With extension .tap = {1} ".format(vdir,result) )
    
    # Rename Directories in the directory
    print()
    setApplicationDebugLevel(0)
    print("Rename Directories in the directory: ", "static/Tapes/")
    result = renSubDir("static/Tapes/"," ","_")
    print ("Number of convertion: {0}".format(result))

    # Test formatCmd2os
    print()
    setApplicationDebugLevel(0)
    print("Test formatCmd2os")
    print(formatCmd2os("ls static/Tapes/Aigle_D'Or,_Le_(1984)/"))


    # Test createNestedDict
    print()
    setApplicationDebugLevel(0)
    print("Test createNestedDict")
    oricdic = createNestedDict( "static/Tapes/","Tapes/")
    printNestedDict(oricdic)

    wavdic = createNestedDict( "static/Audio/","Tapes/")
    printNestedDict(wavdic)
    
