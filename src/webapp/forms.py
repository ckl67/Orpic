from flask_wtf import FlaskForm
from wtforms import StringField, RadioField , SubmitField
from webapp import app
from webapp.common import pError

class preferenceForm(FlaskForm):
    frequence = RadioField(
        'Frequence',choices=[
            (1,'4800 Hz'),
            (2,'8000 Hz'),
            (3,'11025 Hz'),
            (4,'44100 Hz')
        ],
        default=app.config["TAP2WAV_FORM_FREQUENCE_ID"],
        coerce=int)
    
    split = RadioField(
        'Split output file', choices=[
            (1,'Yes') ,
            (2,'No')
        ],
        default=app.config["TAP2WAV_FORM_SPLIT_ID"],
        coerce=int)
    
    baud =  RadioField(
        'Baud', choices=[
            (1,'Normal') ,
            (2,'Fast')
        ],
        default=app.config["TAP2WAV_FORM_BAUD_ID"],
        coerce=int)
                         
    submit = SubmitField('Save')

#----------------------------------
#----------------------------------
def idForm2Val_Frequence(id):
    '''
    Convert Form ID to value
    '''
    out = 0
    
    if id == 1:
        out = 4
    elif id == 2:
        out = 8
    elif id == 3:
        out = 11
    elif id == 4:
        out = 44
    else:
        pError("(idForm2Val_Frequence)  Wrong id value: {0} !! ".format(id))
        out = 0
    return(out)
          
#----------------------------------
#----------------------------------
def val2IdForm_Frequence(val):
    '''
    Convert value to Form ID
    '''
    out = 0
    
    if val == 4:
        out = 1
    elif val == 8:
        out = 2
    elif val == 11:
        out = 3
    elif val == 44:
        out = 4
    else:
        pError("(val2IdForm_Frequence)  Wrong value: {0} !! ".format(val))
        out = 0
    return(out)
    
#----------------------------------
#----------------------------------
def idForm2Val_Split(id):
    '''
    Convert Form ID to value
    '''
    out = 0
    
    if id == 1:
        out = "yes"
    elif id == 2:
        out = "no"
    else:
        pError("(idForm2Val_Split)  Wrong id value: {0} !! ".format(id))
        out = 0
    return(out)
          
#----------------------------------
#----------------------------------
def val2IdForm_Split(val):
    '''
    Convert value to Form ID
    '''
    out = 0
    
    if val == "Yes":
        out = 1
    elif val == "y":
        out = 1
    elif val == "no":
        out = 2
    elif val == ",":
        out = 2
    else:
        pError("(val2IdForm_Split)  Wrong value: {0} !! ".format(val))
        out = 0
    return(out)

#----------------------------------
#----------------------------------
def idForm2Val_Baud(id):
    '''
    Convert Form ID to value
    '''
    out = 0
    
    if id == 1:
        out = "n"
    elif id == 2:
        out = "f"
    else:
        pError("(idForm2Val_Baud)  Wrong id value: {0} !! ".format(id))
        out = 0
    return(out)
          
#----------------------------------
#----------------------------------
def val2IdForm_Baud(val):
    '''
    Convert value to Form ID
    '''
    out = 0
    
    if val == "n":
        out = 1
    elif val == "f":
        out = 2
    else:
        pError("(val2IdForm_Baud)  Wrong value: {0} !! ".format(val))
        out = 0
    return(out)

