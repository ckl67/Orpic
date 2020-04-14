from flask_wtf import FlaskForm
from wtforms import StringField, RadioField , SubmitField

class preferenceForm(FlaskForm):
    frequence = RadioField('Frequence', choices=[ (1,'4800 Hz') , (2,'8000 Hz') , (3,'11025 Hz') , (4,'44100 Hz') ], default=1, coerce=int)
    split = RadioField('Split output file', choices=[ (1,'Yes') , (2,'No')], default=1, coerce=int)
    baud =  RadioField('Baud', choices=[ (1,'Normal') , (2,'Fast')], default=1, coerce=int)
                         
    submit = SubmitField('Save')
    
