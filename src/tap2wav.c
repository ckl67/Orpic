/*=================================
  tap2wav tool
  Origine: F.Frances
  Adapted: C. Klugesherz
  Date 2020 March 24
  ================================== */

/* ================================= */
/* ============ INCLUDE  =========== */
/* ================================= */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* ================================= */
/* ============= DEFINE ============ */
/* ================================= */

#define SILENCE_SECOND 3   // Duration in seconds of the silence 0x80 in .wav file
#define AUDIO_LEVEL 192    // Audio level : Max = 255 : Median = 128 : Min = 0
#define FILE_PATH_LEN 250  //

/* ================================= */
/* ======== .WAV STRUCTURE========== */
/* ================================= */

struct riff {
    char sig[4];
    int riff_size;
    char typesig[4];
    char fmtsig[4];
    int fmtsize;
    short tag;
    short channels;
    int freq;
    int bytes_per_sec;
    short byte_per_sample;
    short bits_per_sample;
    char samplesig[4];
    int datalength;
};

/* ================================= */
/* ======== GLOBAL VARS ============ */
/* ================================= */

FILE *in,*out;
int file_size=0;
int speed=4800;
char param_f[5]="4";
char param_b[5]="n";
char mbaud[10] = "2000";
char split[5]="no";
char ofile[FILE_PATH_LEN];
struct riff sample_riff= { "RIFF",0,"WAVE","fmt ",16,1,1,0,0,1,8,"data",0 };

/* ================================= */
/* ========== PROTOYPES ============ */
/* ================================= */
void adapt_ofile(char*, char*, int);
void send_wav_silence_byte();
void emit_level(int);
void emit_bit(int);
void emit_byte(int);
void emit_gap();
int init(int , char **);

/* ================================= */
/* ============= MAIN ============== */
/* ================================= */

int main(int argc,char *argv[])
{
    int i,size;
    unsigned char header[9];
    int oneshot = 0;
    int ofilenb = 0;
    char ofileresult[FILE_PATH_LEN];
    
    if (init(argc,argv) == 0) {
	printf("Usage: %s -i <.TAP file> -o <.WAV file> -f [4, 8 or 11] -s [yes/no/y/n] -b [lf,f,sf,ssf] \n",argv[0]);
	printf("  Option -f : frequence \n");
	printf("       4 --> produces a 4800 Hz WAV file (Default) \n");
	printf("       8 --> produces a 8000 Hz WAV file \n");
	printf("       11 --> produces a 11025Hz WAV file  \n");
	printf("  Option -s : split output file \n");
	printf("        yes/y--------> Will split the .WAV files in as many files than .TAP file is containing programs\n");
	printf("                       The output .WAV file will end with an index number \n");
	printf("        no/n---------> There will be only 'one' .WAV file containing all the program \n");
	printf("        yes/no/y/n --> A silence zone will be added at the end of the different parts \n");
	printf("  Option -b : mean baud rate value \n");
	printf("        n  --> normal          : ~2000 Baud (default) \n");
	printf("       lf  --> low fast        : ~2300 Baud (only for 11025Hz) \n");
	printf("        f  --> fast            : ~2900 Baud ( for 8000Hz or 11025Hz) \n");
	printf("       sf  --> super fast      : ~3300 Baud ( for 8000Hz or 11025Hz) \n");
	printf("      hsf  --> high super fast : ~4000 Baud ( only for 11025Hz) \n");
	exit(1);
    }

    if ((strcmp(split,"yes")== 0) || (strcmp(split,"y")== 0) ){
	// ++++++++++++++++++++++++++
	// WILL CREATE SEVERAL FILES
	// ++++++++++++++++++++++++++

	// Loop in the input file
	while (!feof(in)) {
	    
	    // read synchro (0x24 included) 
	    while (fgetc(in)==0x16){
		oneshot = 1;
	    }

	    // Open output File
	    if (oneshot == 1) {
		// Create output file

		adapt_ofile(ofileresult,ofile,++ofilenb);
		out=fopen(ofileresult,"wb");
		if (out==NULL) {
		    printf("Cannot create %s file\n\n",ofileresult);
		    exit(EXIT_FAILURE);
		}

		// write .wav header !
		fwrite(&sample_riff,1,sizeof(sample_riff),out);
	    }

	    if (feof(in))
		break;

	    // Emit Synchro
	    for (i=0;i<256;i++)
		emit_byte(0x16);
	    emit_byte(0x24);

	    // Emit header
	    for (i=0;i<9;i++)
		emit_byte(header[i]=fgetc(in)); 

	    // Emit Name
	    do {
		i=fgetc(in);
		emit_byte(i); 
	    } while (i!=0);

	    // Delay in order to display the Name !
	    emit_gap();

	    // Emit code : Number of bytes = size
	    size=(header[4]*256+header[5])-(header[6]*256+header[7])+1;
	    for (i=0;i<size;i++)
		emit_byte(fgetc(in));

	    // Send .Wav Silence = 0x80 : 3 secondes 
	    // at 4800Hz --> 4800 sample for 1 seconds
	    for (i=0;i<speed*SILENCE_SECOND;i++)
		send_wav_silence_byte();

	    // Close output File
	    if (oneshot == 1) {

		// finalize .Wav header
		sample_riff.freq= speed;
		sample_riff.bytes_per_sec=speed;
		sample_riff.datalength=file_size;
		sample_riff.riff_size=sample_riff.datalength+8+sample_riff.fmtsize+12;
		fseek(out,0,SEEK_SET); 
		fwrite(&sample_riff,1,sizeof(sample_riff),out);

		fclose(out);
    
		printf("%s file created at frequence %d Hz with a mean's Baud rate at : %s bits/s \n",ofileresult,speed,mbaud);

		file_size=0;		
		oneshot = 0;
	    }    
	}
	fclose(in);
    }
    else {
	// ++++++++++++++++++++++++++
	// ONLY ONE FILE TO CREATE
	// ++++++++++++++++++++++++++

	// Create output file
	adapt_ofile(ofileresult,ofile,0);
	out=fopen(ofileresult,"wb");

	if (out==NULL) {
	    printf("Cannot create %s file\n\n",ofile);
	    exit(EXIT_FAILURE);
	}

	// write .wav header !
	fwrite(&sample_riff,1,sizeof(sample_riff),out);

	// Loop in the input file
	while (!feof(in)) {
	    // read synchro (0x24 included) 
	    while (fgetc(in)==0x16){
	    } 

	    if (feof(in))
		break;

	    // Emit Synchro
	    for (i=0;i<256;i++)
		emit_byte(0x16);
	    emit_byte(0x24);

	    // Emit header
	    for (i=0;i<9;i++)
		emit_byte(header[i]=fgetc(in)); 

	    // Emit Name
	    do {
		i=fgetc(in);
		emit_byte(i); 
	    } while (i!=0);

	    // Delay in order to display the Name !
	    emit_gap();

	    // Emit code : Number of bytes = size
	    size=(header[4]*256+header[5])-(header[6]*256+header[7])+1;
	    for (i=0;i<size;i++)
		emit_byte(fgetc(in));

	    // Send .Wav Silence = 0x80 : 3 secondes 
	    // at 4800Hz --> 4800 sample for 1 seconds
	    for (i=0;i<speed*SILENCE_SECOND;i++)
		send_wav_silence_byte();
	}
	fclose(in);

	// finalize .Wav header
	sample_riff.freq= speed;
	sample_riff.bytes_per_sec=speed;
	sample_riff.datalength=file_size;
	sample_riff.riff_size=sample_riff.datalength+8+sample_riff.fmtsize+12;
	fseek(out,0,SEEK_SET); 
	fwrite(&sample_riff,1,sizeof(sample_riff),out);

	fclose(out);
    
	printf("%s file created at frequence %d Hz with a mean's Baud rate at : %s bits/s \n",ofileresult,speed,mbaud);
    }
}

/* ================================= */
/* ========= PROCEDURES ============ */
/* ================================= */

/* ---------------------------------------------------------- 
   Will create output file
      in : ../aigle_or.wav
      out: ../aigle_or_01.wav
   ------------------------------------------------------------*/
void adapt_ofile(char *out, char *in, int id)
{
    char *pch;
    pch = strrchr(in,'.');

    strncpy(out,in, pch-in);
    out[pch-in] = '\0'; // null char added

    if (id == 0) {
	if (strcmp(param_b,"n")!=0)
	    if (strcmp(mbaud,"2000")!=0)
		sprintf(out,"%s_%sk_%sb.wav",out,param_f,mbaud);
	    else
		sprintf(out,"%s_%sk.wav",out,param_f);
	else
	    sprintf(out,"%s_%sk.wav",out,param_f);
	
    } else {
	if (strcmp(param_b,"n")!=0) 
	    if (strcmp(mbaud,"2000")!=0)
		sprintf(out,"%s_%sk_%sb_%d.wav",out,param_f,mbaud,id);
	    else
		sprintf(out,"%s_%sk_%d.wav",out,param_f,id);
	else
	    sprintf(out,"%s_%sk_%d.wav",out,param_f,id);
    }
}
/* ---------------------------------------------------------- 
   Will add a silence = 0x80 in the wave file
   ------------------------------------------------------------*/
void send_wav_silence_byte()
{
    fputc(0x80,out);
    file_size+=1;
}


/* ---------------------------------------------------------- 
   Emit Audio signal
   Invert Signal after each time
   ------------------------------------------------------------*/
void emit_level(int size)
{
    static int current_level=AUDIO_LEVEL;
    int i;
    current_level=255-current_level;
    for (i=0;i<size;i++)
	fputc(current_level,out);
    file_size+=size;
}

/* ---------------------------------------------------------- 
   Emit a bit which is coded as following
   Trigger(T) followed by Signal (0 or 1) : 0=2T : 1:=T
   .    __    __             __    ____
   .      \T_|1 \              \T_| 0  \  
   ------------------------------------------------------------*/
void emit_bit(int bit)
{
    switch (speed) {
    case 4800:
	// 1/4000 = T = 208us
	// bit 1: T*2 = 416us
	// bit 0: T*3 = 624us
        emit_level(1);
        if (bit)
	    emit_level(1);
        else
	    emit_level(2);
        break;
    case 8000:
	// 1/8000 = T = 125us
	// bit 1: T*3 = 375us
	// bit 0: T*5 = 625us
        if (bit) {
	    if (strcmp(param_b,"f")==0) {
		emit_level(1);
		emit_level(1);
	    } else if (strcmp(param_b,"sf")==0) {
		emit_level(1);
		emit_level(1);
	    } else {
		emit_level(1);
		emit_level(2);
	    }
        } else {
	    if (strcmp(param_b,"f")==0) {
		emit_level(2);
		emit_level(2);
	    } else if (strcmp(param_b,"sf")==0) {
		emit_level(1);
		emit_level(2);
	    } else {	     
		emit_level(2);
		emit_level(3);
	    }
        }
        break;
    case 11025:
	// 1/11025 = T = 90us
	// bit 1: T*4 = 362us
	// bit 0: T*7 = 630us
        if (bit) {
	    if (strcmp(param_b,"lf")==0) {
		emit_level(2);
		emit_level(2);
	    } else  if (strcmp(param_b,"f")==0) {
		emit_level(1);
		emit_level(2);
	    } else if (strcmp(param_b,"sf")==0) {
		emit_level(1);
		emit_level(2);
	    } else if (strcmp(param_b,"hsf")==0) {
		emit_level(1);
		emit_level(1);
	    } else {	     
		emit_level(2);
		emit_level(2);
	    }
        } else {
	    if (strcmp(param_b,"lf")==0) {
		emit_level(3);
		emit_level(3);
	    } else  if (strcmp(param_b,"f")==0) {
		emit_level(2);
		emit_level(3);
	    } else if (strcmp(param_b,"sf")==0) {
		emit_level(2);
		emit_level(2);
	    } else if (strcmp(param_b,"hsf")==0) {
		emit_level(2);
		emit_level(2);
	    } else {	     
		emit_level(3);
		emit_level(4);
	    }
        }
        break;
    }
}

/* ---------------------------------------------------------- 
   A byte is composed of
   * 1 start bit 
   * 8 data bits
   * 1 parity bit
   * 4  stop bits
   ------------------------------------------------------------*/
void emit_byte(int val)
{
    int i,parity=1;
    emit_bit(0);
    for (i=0; i<8; i++,val>>=1) {
	parity+=val&1;
	emit_bit(val&1);
    }
    emit_bit(parity&1);
    emit_bit(1);
    emit_bit(1);
    emit_bit(1);
    emit_bit(1); 
}

/* ---------------------------------------------------------- 
   A gap of Stop bits in order to smooth to display the title
   ------------------------------------------------------------*/
void emit_gap()
{
    int i;
    for (i=0;i<100;i++) emit_bit(1); 
}

/* ---------------------------------------------------------- 
   Valid program parameters
   -i input TAP file 
   -o output WAV file
   -s Split  : yes/no (default no)
   -f frequence : -f 4,8,11 (default 4) 
   ------------------------------------------------------------*/
int init(int argc, char *argv[])
{
    size_t optind;

    char errortxt[] =  "Usage: %s -i <input file.tap> -o <output file.wav> -f [4, 8 or 11] -s [yes/no/y/n] -b [lf,f,sf,ssf] \n";
    char ifile[FILE_PATH_LEN];

   
    for (optind = 1; optind < argc ; optind++) {
	if (argv[optind][0] == '-') {
	    switch (argv[optind][1]) {
	    case 'i':
		if (argv[optind+1]!=NULL)
		    strcpy(ifile,argv[optind+1]);
		else {
		    fprintf(stderr,errortxt, argv[0]);
		    exit(EXIT_FAILURE);
		}
		break;
	    case 'o':
		if (argv[optind+1]!=NULL)
		    strcpy(ofile,argv[optind+1]);
		else {
		    fprintf(stderr,errortxt, argv[0]);
		    exit(EXIT_FAILURE);
		}
		break;
	    case 'f':
		if (argv[optind+1]!=NULL)
		    strcpy(param_f,argv[optind+1]);
		else {
		    fprintf(stderr,errortxt, argv[0]);
		    exit(EXIT_FAILURE);
		}
		break;
	    case 's':
		if (argv[optind+1]!=NULL)
		    strcpy(split,argv[optind+1]);
		else {
		    fprintf(stderr,errortxt, argv[0]);
		    exit(EXIT_FAILURE);
		}
		break;
	    case 'b':
		if (argv[optind+1]!=NULL)
		    strcpy(param_b,argv[optind+1]);
		else {
		    fprintf(stderr,errortxt, argv[0]);
		    exit(EXIT_FAILURE);
		}
		break;
	    default:
		fprintf(stderr,errortxt, argv[0]);
		exit(EXIT_FAILURE);
	    }
	}
    }

    // Check parameter : split
    if ( strcmp(split,"no") && strcmp(split,"yes") && strcmp(split,"y") && strcmp(split,"n")) {
	printf("Bad split parameter : %s  !!\n\n", split);
	return 0;
    }

    // Check parameter param_b : baud rate
    if ( (strcmp(param_b,"n") && strcmp(param_b,"lf") && strcmp(param_b,"f") && strcmp(param_b,"sf") && strcmp(param_b,"hsf")) ) {
	printf("Bad Baud rate parameter : %s  !!\n\n", param_b);
	return 0;
    }

    // Check param_f
    if (strcmp(param_f,"4")==0) {
	speed=4800;
	strcpy(mbaud,"2000");
    }
    else if (strcmp(param_f,"8")==0) {
	speed=8000;
	if (strcmp(param_b,"f")==0) 
	    strcpy(mbaud,"2900");
	else if (strcmp(param_b,"sf")==0) 
	    strcpy(mbaud,"3300");
	else 
	    strcpy(mbaud,"2000");
    }
    else if (strcmp(param_f,"11")==0) {
	speed=11025;

	if (strcmp(param_b,"lf")==0) 
	    strcpy(mbaud,"2300");
	else  if (strcmp(param_b,"f")==0) 
	    strcpy(mbaud,"2900");
	else if (strcmp(param_b,"sf")==0) 
	    strcpy(mbaud,"3300");
	else if (strcmp(param_b,"hsf")==0) 
	    strcpy(mbaud,"4000");
	else 
	    strcpy(mbaud,"2000");
    }
    else { 
	printf("Bad frequence option : %s  !!\n\n",param_f);
	return 0;
    }

    // Check input file
    in=fopen(ifile,"rb");
    if (in==NULL) {
	printf("Cannot open %s file\n\n",ifile);
	return 0;
    }
	    
    return 1;
}
