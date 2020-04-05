from os import listdir,rename
from os.path import isfile,isdir, join, splitext
import sys

def renameFiles(dirin,charin,charout):
    """
    function which replaces all files in a directory by 
    replacing : charin with charout
    parameter
    * dirin : directory
    * charin : characters to replace
    * charout : character to replace
    """

    for f in listdir(dirin): # lis all of item in a directory
        if isfile(join(dirin, f)): # Check if it is file
            if f.find(charin) > 0:   # Check if in the file there is charin ?
                fout = f.replace(charin,charout)
                rename(join(dirin, f),join(dirin, fout))
    return 1
    
def createNestedDict(dirin):
    '''
    Create The Oric Nested dictionnary
    Will create a Key for each item
    Each keay beeing a lsit !
    '''
    
    # www.programiz.com/python-programming/nested-dictionary
    odic = {}
    iddic = 0
    nbtap = 0
    
    for d in listdir(dirin): # lis all of item in a directory
        if isdir(join(dirin, d)): # Check if it is directory
            renameFiles(join(dirin, d)," ","_")
            iddic= iddic + 1
            #print(iddic)
            odic[iddic] = {} # Create a new Dic
            odic[iddic].setdefault('name',[]).append(d.replace("_"," ")) # Add a key in the dict as a list
            odic[iddic].setdefault('directory_src',[]).append(join(dirin, d)+"/")

            nbtap = 0
            for f in listdir(join(dirin,d)): # lis all items in directory : d
                #print("    " + f)
                #unpacking tuple
                file_name, file_extension = splitext(join(dirin,f))

                if file_extension.lower() == ".tap":
                    nbtap = nbtap + 1
                    odic[iddic].setdefault('tap_file',[]).append(f) # add as many items in the list!
                if file_extension.lower() == ".jpg":
                    odic[iddic].setdefault('picture',[]).append(f)
                if file_extension.lower() == ".png":
                    odic[iddic].setdefault('picture',[]).append(f)
            odic[iddic].setdefault('tape_nb',[]).append(nbtap)
            
    return(odic)

def formatCmd(cmd):
    '''
    Format the command to send to os.system
    For exapmle to replace ' by \\'
    '''

    out = cmd
    out = out.replace("'","\\'")
    out = out.replace("(","\\(")
    out = out.replace(")","\\)")
    print(out)
    return(out)

    
def printNestDict(oricdic):
    for  o_id,o_info in oricdic.items():
        print("\nDic id:",o_id)
        for key in o_info:
            # now we are at the list level of key in dict o_info[key]
            print(key + ":")
            for vlist in o_info[key]:
                print("   ->", vlist)
    

if __name__ =='__main__':
    vdirectory = sys.argv[1:][0]
    renameFiles(vdirectory," ","_")
    oricdic= createNestedDict(vdirectory)
    printNestDict(oricdic)

    print(oricdic[1].get("name")[0] )
    print(oricdic[1].get("tap_file")[0] )
    print(oricdic[1].get("tap_file")[1] )

    print(formatCmd("l'ile de l'ascencion"))
