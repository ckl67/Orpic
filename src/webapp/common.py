from os import listdir,rename,getcwd
from os.path import isfile,isdir, join, splitext, exists
import sys

def printdebug(text,vdebug=True):
    '''
    Print the text in the consol in case webconfig app.config["DEBUG"] = "True"
    '''
    if vdebug==True :
        print(text)

def renameFilesIn(basePath,dirinRlvPath,charin,charout):
    """
    function which rename all files in a directory by replacing : charin by charout
    parameters
    * basePath : directory base Path
    * dirinRlvPath : directory relative path
    * charin : characters to replace
    * charout : character to replace
    """
    printdebug("---Enter : renameFilesIn in "+ basePath + dirinRlvPath + "  to replace : {" + charin + "} by {" + charout + "} ---")

    dirin = basePath + dirinRlvPath
    if not exists(dirin):
        print("\n --------> chek if : " + basePath + dirinRlvPath +"exists\n")
       
    for f in listdir(dirin): # lis all of item in a directory
        if isfile(join(dirin, f)): # Check if it is file
            printdebug("        Check --> " + f)
            if f.find(charin) > 0:   # Check if in the file there is charin ?
                fout = f.replace(charin,charout)
                printdebug("  rename file by replacing: {"+ charin +"} by {" + charout + "} ::" + f + "-->" + fout)
                rename(join(dirin, f),join(dirin, fout))
    return 1
       
def renameAllFilesInSubDir(basePath,dirinRlvPath,charin,charout):
    """
    function which will rename all files in all Sub directories of a Directory by replacing : charin by charout
    parameters
    * basePath : directory base Path
    * dirinRlvPath : directory relative path
    * charin : characters to replace
    * charout : character to replace
    """
    printdebug("---Enter : renameFilesInSubDirs in "+ basePath + dirinRlvPath + "  to replace : {" + charin + "} by {" + charout + "} ---")

    dirin = basePath + dirinRlvPath
    if not exists(dirin):
        print("\n --------> chek if : " + basePath + dirinRlvPath +" exists\n")
  
    for d in listdir(dirin): # lis all of item in a directory
        printdebug("     Check Dir --> " + d)
        if isdir(join(dirin, d)): # Check if it is directory
            renameFilesIn(dirin,d,charin,charout)
    return 1

def renameDirectoriesIn(basePath,dirinRlvPath,charin,charout):
    """
    function which rename all Directories in a directory by replacing : charin by charout
    * basePath : directory base Path
    * dirinRlvPath : directory relative path
    * charin : characters to replace
    * charout : character to replace
    """
    printdebug("---Enter : renameDirectoriesIn in "+ basePath + dirinRlvPath + "  to replace : {" + charin + "} by {" + charout + "} ---")

    dirin = basePath + dirinRlvPath
    if not exists(dirin):
        print("\n --------> chek if : " + basePath + dirinRlvPath +" exists\n")
  
    for f in listdir(dirin): # lis all of item in a directory
        if isdir(join(dirin, f)): # Check if it is directory
            printdebug("        Check --> " + f)
            if f.find(charin) > 0:   # Check if in the file there is charin ?
                fout = f.replace(charin,charout)
                printdebug("  rename directory by replacing: {"+ charin +"} by {" + charout + "} ::" + f + "-->" + fout)
                rename(join(dirin, f),join(dirin, fout))
    return 1

def createNestedDict(dirinRlvPath):
    '''
    Create The Oric Nested dictionnary
    Will create a Key for each item
    Each keay beeing a list !
    '''

    printdebug("---Enter : createNestedDict---")

    # www.programiz.com/python-programming/nested-dictionary
    odic = {}
    iddic = 0
    nbtap = 0
    
    printdebug("    --> " + dirinRlvPath)
    
    for d in listdir(dirinRlvPath): # lis all of item in a directory
        if isdir(join(dirinRlvPath, d)): # Check if it is directory
            printdebug("       --> " + join(dirinRlvPath, d))
            iddic= iddic + 1
            odic[iddic] = {} # Create a new Dic
            odic[iddic].setdefault('name',[]).append(d.replace("_"," ")) # Add a key in the dict as a list
            odic[iddic].setdefault('directory_src',[]).append(join(dirinRlvPath, d)+"/")

            nbtap = 0
            for f in listdir(join(dirinRlvPath,d)): # lis all items in directory : d
                #unpacking tuple
                file_name, file_extension = splitext(join(dirinRlvPath,f))

                if file_extension.lower() == ".tap":
                    nbtap = nbtap + 1
                    odic[iddic].setdefault('tap_file',[]).append(f) # add as many items in the list!
                if file_extension.lower() == ".jpg":
                    odic[iddic].setdefault('picture',[]).append(f)
                if file_extension.lower() == ".png":
                    odic[iddic].setdefault('picture',[]).append(f)
            odic[iddic].setdefault('tape_nb',[]).append(nbtap)
            
    return(odic)

def formatCmd2os(cmd):
    '''
    Format the command to send to os.system
    Example: 
        Aigle_D'Or,_Le_(1983) is a correct name for a Directory, or file: This data is saved in oricdic
    However, in Linux the chars: "'", or "(", or ")" are not allowed alone, so we must add prefix "\"

        >>:~/Work/Orpic/src/webapp/static/Tapes$ cd Aigle_D\'Or,_Le_\(1984\)/
        >>:~/Work/Orpic/src/webapp/static/Tapes/Aigle_D'Or,_Le_(1984)$ 

    For example we will replace an (apostrophe) ' by \' in order to make a "cd"
    '''
    printdebug("---Enter : formatCmd2os---")

    printdebug("   cmd os in: " + cmd)
    out = cmd
    out = out.replace("'","\\'")
    out = out.replace("(","\(")
    out = out.replace(")","\)")
    printdebug("   cmd os out: " + out)
    return(out)

    
def printNestedDict(oricdic):
    '''
    will print all elements in the Nested Dict
    '''
    printdebug("---Enter : printNestDict---")
    for  o_id,o_info in oricdic.items():
        print("\nDic id:",o_id)
        for key in o_info:
            # now we are at the list level of key in dict o_info[key]
            print(key + ":")
            for vlist in o_info[key]:
                print("   ->", vlist)   

if __name__ =='__main__':
    oricdic = {}
    # python common.py static/Tapes/
    # print("Rename Directories in the directory: ", "static/Tapes/")
    # renameDirectoriesIn("/home/christian/Work/Orpic/src/webapp/", "static/Tapes/"," ","_")
    
    # print("Rename All Files in the Sub directory: ", "static/Tapes/")
    # renameAllFilesInSubDir("/home/christian/Work/Orpic/src/webapp/", "static/Tapes/"," ","_")

    # print("Test formatCmd2os")
    # print("cd " + formatCmd2os("static/Tapes/Aigle_D'Or,_Le_(1984)/"))
    
    oricdic= createNestedDict( "static/Tapes/")
    # printNestedDict(oricdic)

    # print("Print some elements in the Nested List: oricdic")
    # print(oricdic[1].get("name")[0] )
    # print(oricdic[1].get("tap_file")[0] )
    # print(oricdic[1].get("tap_file")[1] )
