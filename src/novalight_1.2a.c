/* Novalight 1.2a tool by S.Guyart, thanks to F.Frances' tap2wav + tap2cd tools 03/2019*/
/* Produces a very fast loading, with a fast (F16 format) loaded standard loader first */
/* With thanks to the supporting Oricians from Oric.org and Defence-Force.org forums */
/* (helped a lot with tests and ideas), and to Fabrice for his huge help. */
/*
  Parameters:
  -s : standard speed for loader instead of F16 speed
  -p : longer pause between programs
  -m : multipart booster. Reduces the loader size once loaded with the 1st part, providing it remained intact.
  -r : relocate Novalight to a specific memory page, for instance -rA0 will place Novalight in $A000

  Normal byte decoding method
  Final byte	Using the fastest signal for zeros (stats show 62% of "0" in a program)
  Signal	Decod.	inverted	but decoding is faster giving "1" value for shortest sinusoids
  3 samples	00	11	00		=> Novalight codes ""00" on 3 samples,
  4 samples	01	10	01  		the code will decode it fast as "11",
  5 samples	10	01	10		(same for all other values), and at the end
  6 samples	11	00	11		the bits will be inverted to finally get the byte value!
  7 samples	1111	0000	1111            (1111 trick reduces the size of the 13 longest bytes, by 5 or 10 samples)

  Idea: decoding a byte must take less than 5 samples duration, in any case.
  Start bit length allows to switch:
  - 5 samples: what's following is a standard byte (4*2 bits)
  - 6 or 7 samples: go read the byte value in one of the two 7-bytes long dictionaries. The selected bytes are those whose
  frequency, RLE compression put aside, is the highest. In case of equal amount of occurencies, the bytes
  that have the longest stadard encoding length are selected first (and get the shortest new encoding).
  3 samples = the most repeated, 4 samples the next most repeated, etc.
  - 8 samples: what follows is a RLE repeat, 3 samples per repeat, 4 samples for the last (so 4 samples if there's only 1 repetition)
  - 9 samples: end of program (saves time at each loop, avoiding having to test the position in RAM).

*/


/* CKL adapt to gcc compile  */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int feed_parameters(char*);
    
// Here are the Oric timer thresholds to measure the sinusoid length; the key to signal decoding!
#define thres3_4 0xA9
#define thres4_5 0x96
#define thres5_6 0x7C
#define thres6_7 0x62
#define thres7_8 0x4A
#define thres8_9 0x33

FILE *in,*out;
int start, end, prog_size, file_size;
int noF16speed = 0;
int userrelocate = 0;
int longpause=0;
int multipartbooster=0;
int prognum=0;
int fileproblem=0;
int ALC=0;
int speed=44100;
int byteprev=-1;
int repeat=0;
int dicobyte=-1;
float stattotal = 0;
float statnormal = 0;
float statRLE = 0;
float statdictionary = 0;
long dico[14][6]; //Dictionary table: value, start bit length (6 or 7), new byte length, hits, total length, initial byte length
int memorymap[256];
unsigned char header[9];
unsigned char Mem[64*1024],name[40];


unsigned char loader_common_area_0a[22]=
    {  0x20,0x4A,0x01,0xA0,0x20,0x20,0x52,0x01,0x99,0x91,0x02,0x88,0xD0,0xF7,0xA0,0x3C,0x20,0x33,0x01,0x4C,
       0x83,0x01
    };

unsigned char loader_common_area_0b[5]=
    {  0xA0,0x3D,0x20,0x33,0x01
    };

unsigned char loader_common_area_0[39]=
    {  0xA0,0x00,0x20,0x52,0x01,0x91,0x33,0xE6,0x33,0xD0,0x02,0xE6,0x34,0xD0,0xF3,0x78,0x68,0x68,0x20,0x4A,
       0x01,0xA0,0x3F,0x20,0x33,0x01,0x4C,0x83,0x01,0x20,0x52,0x01,0x99,0x82,0x01,0x88,0xD0,0xF7,0x60
    };

//1st loader + common area 1. Will load common area 0a + 0, that will partially crush this code
unsigned char loader_oric[134]=
    {  0xA0,0x79,0xB9,0x3A,0x01,0x49,0xDF,0x99,0x3A,0x01,0x88,0xD0,0xF5,0xF0,0x46,0xAE,0x00,0x03,0xAE,0x08,
       0x03,0x8E,0x09,0x03,0x28,0x18,0x08,0x40,0xAE,0x00,0x03,0x38,0x58,0xB0,0xFE,0x60,0x38,0xB0,0xFE,0xE0,
       thres8_9,0x90,0xCC,0xE0,thres7_8,0x90,0x52,0xE0,thres5_6,0x90,0x22,0xE0,thres3_4,0xB0,0xED,0x38,0xA9,0xFE,0xB0,0xFE,
       0xE0,thres4_5,0xB0,0x0C,0x2A,0xE0,thres6_7,0xB0,0x02,0x0A,0x0A,0xE0,thres5_6,0x4C,0x7D,0x01,0x2A,0xE0,thres3_4,0x2A,
       0xB0,0xE8,0x49,0xFF,0x60,0xAD,0x76,0xC0,0xF0,0x07,0x20,0x6A,0xE7,0xA0,0x1C,0xD0,0x05,0x20,0xCA,0xE6,
       0xA0,0x00,0xA9,0x3D,0x99,0x29,0x02,0xA9,0x01,0x99,0x2A,0x02,0xA9,0x90,0x8D,0x0E,0x03,0x20,0x4A,0x01,
       0xA0,0x3D,0x20,0x52,0x01,0x99,0xFF,0x00,0x88,0xD0,0xF7,0x4C,0x00,0x01
    };

//Common area 1 bis. Used for parts 2, 3, ... if 1st loader and Common area 0 haven't been altered in page 1
unsigned char loader_common_area_1bis[35]=
    {  0xAD,0x76,0xC0,0xF0,0x07,0x20,0x6A,0xE7,0xA0,0x1C,0xD0,0x05,0x20,0xCA,0xE6,0xA0,0x00,0xA9,0x3D,0x99,
       0x29,0x02,0xA9,0x01,0x99,0x2A,0x02,0xA9,0x90,0x8D,0x0E,0x03,0x4C,0x00,0x01
    };

unsigned char loader_common_area_2[60]=
    {  0x78,0xAD,0x1F,0x02,0xD0,0x1B,0xAD,0x76,0xC0,0xF0,0x06,0x20,0xA0,0xE5,0x38,0xB0,0x10,0xA9,0x12,0xA0,
       0xE5,0x20,0x76,0xE5,0xE8,0xE8,0xA9,0x93,0xA0,0x02,0x20,0x2F,0xF8,0xAD,0xA9,0x02,0x85,0x33,0xAD,0xAA,
       0x02,0x85,0x34,0x20,0x4A,0x01,0xA0,0x13,0x20,0x52,0x01,0x99,0x02,0x01,0x88,0xD0,0xF7,0x4C,0x11,0x01
    };

unsigned char loader_common_area_3[61]=
    { 0xA9,0x03,0xE0,thres6_7,0xB0,0x02,0xA9,0x0A,0x38,0xB0,0xFE,0xE0,thres5_6,0x90,0x0C,0x69,0x00,0xE0,thres4_5,0x69,
      0x00,0xE0,thres3_4,0x69,0x00,0x10,0x0C,0xE0,thres6_7,0xE9,0x00,0xE0,thres7_8,0xE9,0x00,0xE0,thres8_9,0xE9,0x00,0xAA,
      0xBD,0x03,0x01,0x60,0x38,0xB0,0xFE,0xE0,thres3_4,0x90,0xF8,0x91,0x33,0xE6,0x33,0xD0,0x02,0xE6,0x34,0xD0,
      0xF0
    };

unsigned char loader_common_area_4[63]=
    { 0xA0,0x16,0x20,0x52,0x01,0x99,0xFF,0x00,0x88,0xD0,0xF7,0x78,0xAE,0x02,0xC0,0xE8,0xE8,0x8A,0xAE,0x40,
      0xFD,0x9D,0x2A,0x02,0xAD,0xCC,0xE3,0x9D,0x29,0x02,0x8A,0xF0,0x06,0x20,0x3D,0xE9,0x4C,0xD6,0xE8,0xAD,
      0x1F,0x02,0xD0,0x03,0x20,0x63,0xE5,0x20,0x07,0xE8,0xA2,0x05,0xBD,0xA9,0x02,0x95,0x5F,0xCA,0x10,0xF8,
      0x4C,0xC0,0xE7
    };

// The "Automatic Level Control" patch was used in front of Atmos recordings, to avoid having Orics
// detecting fake errors at the beginning of the synchro sequence. Useless with Novalight, it will
// be removed when encountered.
unsigned char ALC_patch[62]=
    { 0x08,0x78,0xAD,0xF9,0xFF,0xC9,0x01,0xD0,0x28,0xAD,0xB6,0xE4,0xC9,0xA2,0xD0,0x15,0xA0,0x09,0xB9,0xB5,
      0x02,0x99,0x21,0x02,0x88,0x10,0xF7,0xA9,0x21,0x8D,0x45,0x02,0xA9,0x02,0x8D,0x46,0x02,0x4C,0x67,0xE8,
      0x81,0x02,0xBF,0x02,0xC7,0x80,0x90,0x49,0x00,0x4C,0xB6,0xE7,0x48,0xA9,0x00,0x8D,0xB1,0x02,0x68,0x4C,
      0x22,0xEE 
    };

struct {
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
} sample_riff= { "RIFF",0,"WAVE","fmt ",16,1,1,0,0,1,8,"data",0 };

void creation_dico()
//*********************************************************************************************
//**** Only take into consideration "isolated" bytes (not included in a RLE compression). *****
//****  Bytes selection is very complicated. We will select those who have a better gain  *****
//****    (signal removed - signal added), using an average position in the dictionary    *****
//****                         for the "signal added" calculation.                        *****
//****    Once selected, values with higher hits must have the shortest encoding length   *****
//****     (to have the shortest singal in the end... So sort by hits at the end)         *****
//*********************************************************************************************

{ int i, j, k, h, indice, val, valprec;
    long tout[256][4]; //byte value, encoding length in samples, hits, total length.
    long dicotemp[14][6];
    //  long dicosampl[14]={9,10,10,11,11,12,12,13,13,14,14,15,15,16};
    long max;
    unsigned char valin;

    // tables initialization
    for (i=0;i<=3;i++)
	{ for (j=0;j<=3;j++)
		{ for (k=0;k<=3;k++)
			{ for (h=0;h<=3;h++)
				{ indice=(i*64)+(j*16)+(k*4)+h;
				    tout[indice][0]=indice;

				    // initial uncompressed novalight byte length, in samples
				    // (much complicated by the fact that now, '1111' is encoded with 7 samples)
				    if (i+j+k+h==12)
					{ tout[indice][1]=5+14;
					}
				    else
					{ if (i+j==6)
						{ tout[indice][1]=5+7+6+k+h;
						}
					    else
						{ if (j+k==6)
							{ tout[indice][1]=5+3+i+7+3+h;
							}
						    else
							{ if (k+h==6)
								{ tout[indice][1]=5+6+i+j+7;
								}
							    else
								{ tout[indice][1]=5+12+i+j+k+h;  
								}
							}
						}
					}
				    tout[indice][2]=0;   // hits
				    tout[indice][3]=0;   // total inital size
				}
			}
		}
	}

    for (i=0;i<=13;i++)
	{ for (j=0;j<=5;j++)
		{ dicotemp[i][j]=0;
		    dico[i][j]=0;
		}
	}

    //read the whole file

    // prog_size pre-calculated in previous function
    valprec=-1;
    for (i=0;i<prog_size;i++)
	{ // count the hits for unrepeated bytes, and update the table
	    // directly recompute the encoding length by adding 1 encoding length to the total length, minus
	    //   an average value of the new encoding (13 samples = average between 9 and 16 (6+3 to 7+9 samples) ).
	    val=Mem[i];
	    if (valprec != val)
		{  tout[val][2]=tout[val][2]+1;
		    tout[val][3]=tout[val][3]+tout[val][1] -13;
		}
	    valprec=val;
	}

    // display table
    //    for (i=0; i<=255; i++) {
    //        printf("Valeur: %ld  Durée: %ld  Occur: %ld  Total: %ld \n", tout[i][0], tout[i][1], tout[i][2], tout[i][3]);
    //    }

    //put 14 higher lengths into "dicotemp" (value, start bit, new encoding length, occurrencies, total length)
    //tout[256][4] : byte value, encoding length in samples, hits, total length
    for (i=0;i<=13;i++)
	{ for (j=0;j<=255;j++)
		// if greater total length, or equal with greater amount of occurencies, then keep this value in dicotemp
		{ if ((tout[j][3]>dicotemp[i][4]) || ((tout[j][3]==dicotemp[i][4]) && (tout[j][2]>dicotemp[i][3])))
			{ dicotemp[i][0]=tout[j][0];
			    dicotemp[i][3]=tout[j][2];
			    dicotemp[i][4]=tout[j][3];
			    dicotemp[i][5]=tout[j][1];
			}
		}
	    // after each loop on "tout", exclude from "tout" the value that has just been kept in Dicotemp
	    tout[dicotemp[i][0]][3]=-1;
	    // if in the end, dicotemp is not entirely filled, the "empty" values will be set to 0 (control)
	}

    // Give the new values to encode, by switching from "dicotemp" to "dico". Combination start bit + value
    // To optimize, give the shortest values to the higer amount of occurencies.
    for (i=0;i<=13;i++)
	{ max=0;
	    for (j=0;j<=13;j++)
		// if amount of occurencies is greater, then keep the value in dico
		{ if (dicotemp[j][3]>dico[i][3])
			{ dico[i][0]=dicotemp[j][0];
			    dico[i][3]=dicotemp[j][3];
			    dico[i][4]=dicotemp[j][4];
			    dico[i][5]=dicotemp[j][5];
			    max=j;
			}
		}
	    // after each loop on "dicotemp", exclude from "dicotemp" the value that has just been kept in Dico
	    dicotemp[max][3]=-1;
	    // if in the end, Dico is not entirely filled, the "empty" values will be set to 0 (control)
	}

    // Assign the start bit length (table 1=6 samples, table 2=7 samples) and encoding length for each byte 
    j=3;
    for (i=0;i<=13;i=i+2)
	{  dico[i][1]=6;
	    dico[i][2]=j;
	    dico[i+1][1]=7;
	    dico[i+1][2]=j;
	    j++;
	}

    // Display the final table
    printf("\n --- Program %i dictionary (%i bytes, #%04X-#%04X) ---\n", prognum, prog_size, start, end);
    for (i=0; i<=13; i++) {
        printf("Value: %3ld ($%02X)  Start: %ld  Encoding: %ld  Occ: %ld  Initial length: %ld \n", dico[i][0],(unsigned int)dico[i][0], dico[i][1], dico[i][2], dico[i][3], dico[i][5]);
    }
    printf("\n");
}

void emit_level(int size, int niveau)
{
    int i;
    for (i=0;i<size;i++) fputc(niveau,out);
    file_size+=size;
}

void emit_silence(int size)
{ /* silence for mp3 players or other players, that "eat" (i.e. don't play) the end of the signal */
  /* or silence to leave the Oric some time to swith to Hires, and things alike */
    static int current_level=0x80;
    int i;
    //size=15000;
    current_level=256-current_level;
    for (i=0;i<size;i++) fputc(current_level,out);
    file_size+=size;
}

// Novalight bit
void emit_bit(int bit)
{
    switch (speed) {
    case 44100:
        emit_level(1,48);
        emit_level(2,208);
        emit_level(bit-3,48);
    }
}

// Novalight double bit
void emit_two_fast_bits(int bits)
{
    switch (bits) {
    case 0: emit_bit(3); break;
    case 1: emit_bit(4); break;
    case 2: emit_bit(5); break;
    case 3: emit_bit(6); break;
    }
}

// Novalight synchro and header bytes must be uncompressed: RLE would compress the synchro, 
//  and dictionaries are not initialized yet!
// V1.1m: 6+6 samples are replaced by 7 samples. 5 possible combinations: x x x x; 7 x x; x 7 x; x x 7; 7 7.

void emit_synchro_byte(int val)
{ int x1, x2, x3, x4;

    x1=(val>>6)&3;  // set values of bits, 2 by 2.  X1=BBxxxxxx, X2=xxBBxxxx and so on.
    x2=(val>>4)&3;
    x3=(val>>2)&3;
    x4=val&3;

    //printf("%x ", val);
    emit_bit(5); /*start Novalight normal byte */

    if (x1+x2+x3+x4==12)  // case 7 + 7 samples
	{  emit_bit(7);
	    emit_bit(7);
	}
    else
	{
	    if ((x1+x2==6)&&(x3+x4<6))  // case 7 + x + x samples
		{  emit_bit(7);
		    emit_two_fast_bits((val>>2)&3);
		    emit_two_fast_bits(val&3);
		}
	    else
		{
		    if ((x1<3)&&(x2+x3==6))  // case x + 7 + x samples
			{  emit_two_fast_bits((val>>6)&3);
			    emit_bit(7);
			    emit_two_fast_bits(val&3);
			}
		    else
			{
			    if ((x2<3)&&(x3+x4==6))  // case x + x + 7 samples
				{  emit_two_fast_bits((val>>6)&3);
				    emit_two_fast_bits((val>>4)&3);
				    emit_bit(7);
				}
			    else
				{  emit_two_fast_bits((val>>6)&3);  // the shifting does NOT affect val
				    emit_two_fast_bits((val>>4)&3);
				    emit_two_fast_bits((val>>2)&3);
				    emit_two_fast_bits(val&3);
				}
			}
		}
	}
    //emit_bit(3); /*stop*/
}

// Novalight byte, with RLE compression and dictionary
void emit_byte(int val)
{ int i,j;
    if (byteprev!=val) /* always emit a byte whose value has changed (obviously, if different: it's not repeated!) */
	{ if (repeat>0) /* end pending repetitions first: emit the previous repeated byte according to amount of repeats */
		{ if ((repeat==1) && (dicobyte!=-1) && ((dico[dicobyte][1]+dico[dicobyte][2])<12)) 
			{ emit_bit(dico[dicobyte][1]);  /*some dictionary bytes (6+3, ...) are shorter than a single repeat (8+4) */
			    emit_bit(dico[dicobyte][2]);  /* so emit dictionary instead of a single repeat. */
			    stattotal++;
			    statdictionary++;
			    //        printf("dico[dicobyte][1]: %ld  dico[dicobyte][2]): %ld\n", dico[dicobyte][1], dico[dicobyte][2]));
			}
		    else
			{ emit_bit(8);  /*stop-start bit: repeated byte */
			    stattotal=stattotal+repeat; /* increse stats for RLE counting */
			    statRLE=statRLE+repeat;
			    for (i=0;i<(repeat-1);i++) emit_bit(3); /* 3 samples for each repetition */
			    emit_bit(4); /* 4 samples for the last repeated byte */
			}
		    repeat=0;
		    dicobyte=-1;
		}
	    if (val!=-1)   /* no repeat: emit normally the new byte, except if it's an 'end of program' marker (-1) */
		{
		    // First, search the byte to emit in the dictionary, stopping when meeting empty lines (dico[x][3]=0) or end of table
		    i=-1;
		    for(j=0;j<14;j++)
			{ if ((val==dico[j][0]) && (dico[j][3]!=0))
				{ i=j;
				}
			}
		    // if found, emit dico[x][1] (stop/start bit) and dico[x][2] (value)
		    if (i!=-1)
			{ emit_bit(dico[i][1]);
			    emit_bit(dico[i][2]);
			    stattotal++;
			    statdictionary++;
			    dicobyte=i;
			}
		    else
			// else emit a normal byte (not repeated, not in disctionnary)
			{ emit_synchro_byte(val);
			    stattotal++;
			    statnormal++;
			    dicobyte=-1;
			}
		    byteprev=val; /* store the emitted byte as 'new previous' */
		}
	}
    else
	{ repeat++; /* same value than previous byte: increase repeat byte counter */
	}
    // THIS is the previous code, when limiting RLE to 256 repeats to allow X or Y registers as index for RAM storage address (out of date but kept just in case)
    //  else  /* repetitions: emit a repeat byte according to amount of repeats */
    //  { if (repeat==256)
    //    { emit_bit(8);  /*stop-start: octet répété*/
    //        for (i=0;i<(repeat-1);i++) emit_bit(3); /* 3 samples for each repetition */
    //        emit_bit(4); /* 4 samples for the last repeated byte */
    //      repeat=1;  /* there was one more identical value just read*/
    //    }
    //    else
    //    { repeat++; /* same value: repeat byte */
    //    }
    //  }

}

// F16 is an optimised WAV signal working a bit faster, with all Oric standard ROM routines.
void emit_F16_bit(int bit)
{
    switch (speed) {
    case 44100:
        if (bit) {
	    emit_level(1,48);
	    emit_level(2,208);
	    emit_level(2,48);
        } else {
	    emit_level(1,48);
	    emit_level(2,208);
	    emit_level(20,48);
        }
        break;
    }
}

void emit_F16_byte(int val)
{
    int i,parity=1;
    emit_F16_bit(0);
    for (i=0; i<8; i++,val>>=1) {
	parity+=val&1;
	emit_F16_bit(val&1);
    }
    emit_F16_bit(parity&1);
    emit_F16_bit(1);
    emit_F16_bit(1);  /* 2 stop bits instead of 3.5 */
}


// Standard speed for Orics that don't like F16
void emit_standard_bit(int bit)
{
    switch (speed) {
    case 44100:
        if (bit) {
	    emit_level(9,208);
	    emit_level(9,48);
        } else {
	    emit_level(9,208);
	    emit_level(18,48);
        }
        break;
    }
}

void emit_standard_byte(int val)
{
    int i,parity=1;
    emit_standard_bit(0);
    for (i=0; i<8; i++,val>>=1) {
	parity+=val&1;
	emit_standard_bit(val&1);
    }
    emit_standard_bit(parity&1);
    emit_standard_bit(1);
    emit_standard_bit(1);
    emit_standard_bit(1);  /* 3 stop bits instead of 3.5 */
}



void emit_gap()
{
    int i;
    /* several stop bits, allowing time to display the statuts line with Loading and so on */
    for (i=0;i<100;i++) emit_bit(3); 
}

void relocate()
{
    unsigned char head[9], valin2;
    int i, j;

    // Init Memory Map for auto-relocation. 0=occupied, 1=free, 2=priority, 3=top priority (page 1)
    for (i=0;i<256;i++)
	{  memorymap[i] = 0;
	}
    for (i=5;i<192;i++)   // avoid pages 0 to 4 included (system), and > #BF (ROM!). If the user wants to use one of
	{  memorymap[i] = 1;  //  those pages, he will have to 'say' it by using the option parameter (except page 0, forbidden).
	}
    memorymap[1] = 3;   // use page 1 as top priority.
    memorymap[159] = 2; // $9F00 and $9800 are both usable in TEXT and HIRES mode; the
    memorymap[152] = 2; //  only limitation is if GRAB has been used.
  
    if (userrelocate==0)  // if the user hasn't specified anything, check for auto-relocation
	{  // Search pages used in the TAP file
	    // Loop, for each program in the TAP file
	    while (!feof(in)) {
		do
		    { valin2=0;
			if (fgetc(in)==0x16)
			    {  if (feof(in)) break;
				valin2=fgetc(in);        /* read synchro (0x24 included) */
				if (feof(in)) break;
				if (valin2!=0x24) fseek(in, -1, SEEK_CUR);
			    }
		    } while ((valin2!=0x24)&&(!feof(in)));
		if (feof(in)) break;
 
		if (valin2==0x24)        /* a 0x16 0x24 sequence has been found, this is not exactly the ROM sequence, which is */
		    {                       /* 0x16 0x16 0x16 0x16 (anything in between) 0x24; hoping this won't give problems */
			// Read header, name and program
			for (i=0;i<9;i++)
			    { head[i]=fgetc(in);  /* header */
			    }
			// printf(" ------ debut=#%02X   fin=#%02X\n", head[6],head[4]);
			for (i=head[6];i<=head[4];i++)
			    { memorymap[i]=0;  // set memory map with 'memory occupied by program'
			    }

			//***** skip name and program, to avoid detecting fake program starts ($16 $24) inside the code
			start=header[6]*256+header[7];
			end=header[4]*256+header[5];

			while ((fgetc(in)!=0)&&(!feof(in)));   /* skip name */

			for (i=0;i<(end-start)+1;i++)
			    {  fgetc(in);  /* skip program */
				if (feof(in)) break;  /* sometimes the last part of the TAP is shorter than the addresses difference. */
			    }
			//***** end of skipping

		    }
	    }
	    // Search which location must be used
	    j=0;
	    for (i=0;i<=255;i++)
		{  if (memorymap[i]>j)
			{ j=memorymap[i];
			    userrelocate=i;
			}
		    //printf("Memorymap(%i) = %i\n",i,memorymap[i]);
		}      
	}
    // Here, userrelocate has the page to use, set by the user or automatically chosen.

    //"rewind" the file
    rewind(in);

    // actually relocate: change page number in ASM code.
    loader_common_area_0a[2]= userrelocate;
    loader_common_area_0a[7]= userrelocate;
    loader_common_area_0a[18]= userrelocate;
    loader_common_area_0a[21]= userrelocate;

    loader_common_area_0b[4]= userrelocate;

    loader_common_area_0[4]= userrelocate;
    loader_common_area_0[20]= userrelocate;
    loader_common_area_0[25]= userrelocate;
    loader_common_area_0[28]= userrelocate;
    loader_common_area_0[31]= userrelocate;
    loader_common_area_0[34]= userrelocate;

    loader_oric[4]= userrelocate;
    loader_oric[9]= userrelocate;
    loader_oric[75]= userrelocate;
    loader_oric[108]= userrelocate;
    loader_oric[119]= userrelocate;
    loader_oric[124]= userrelocate;
    loader_oric[127]= userrelocate-1;
    loader_oric[133]= userrelocate;

    loader_common_area_1bis[23]= userrelocate;
    loader_common_area_1bis[34]= userrelocate;

    loader_common_area_2[45]= userrelocate;
    loader_common_area_2[50]= userrelocate;
    loader_common_area_2[53]= userrelocate;
    loader_common_area_2[59]= userrelocate;

    loader_common_area_3[42]= userrelocate;

    loader_common_area_4[4]= userrelocate;
    loader_common_area_4[7]= userrelocate-1;

}


void test_ALC()
{ int i, j;

    ALC=0;

    // prog_size pre-calculated in previous function
    // No need to check if the program is smaller than ALC program
    if ((prog_size >= 62))
	{
	    // Compare the 62 first bytes of the TAP program to the 62 bytes of ALC program (if it didn't
	    // begin with the same bytes, then it's a modified program).
	    for (i=0;i<46;i++)
		{ 
		    if (Mem[i] == ALC_patch[i])
			{  ALC++;
			}
		}

	    // Skip bytes in position 46-47 (header's two 'unused bytes'), as their value can change sometimes.
	    ALC=ALC+2;
	    for (i=48;i<62;i++)
		{ 
		    if (Mem[i] == ALC_patch[i])
			{  ALC++;
			}
		}
	    //printf("fin: i=%i - ALC=%i\n",i,ALC);
	    if (ALC==i) ALC=1;
	}
}


int init(int argc, char *argv[])
{
    if (argc<3) return 1;
    if (argv[1][0]=='-') {
	if (feed_parameters(argv[1])) return 1;
    }
    if (argv[2][0]=='-') {
	if (feed_parameters(argv[2])) return 1;
    }
    if ((argc>3)&&(argv[3][0]=='-')) {
	if (feed_parameters(argv[3])) return 1;
    }
    if ((argc>4)&&(argv[4][0]=='-')) {
	if (feed_parameters(argv[4])) return 1;
    }

    sample_riff.freq=sample_riff.bytes_per_sec=speed;
    in=fopen(argv[argc-2],"rb");
    if (in==NULL) {
	printf("Cannot open %s file (does not exit, or in use)\n\n",argv[argc-2]);
	fileproblem=1;
	return 1;
    }
    out=fopen(argv[argc-1],"wb");
    if (out==NULL) {
	printf("Cannot create %s file\n\n",argv[argc-1]);
	fileproblem=1;
	return 1;
    }
    return 0;
}


int feed_parameters(char *param)
{ char *paramnum;

    switch (param[1]) {
    case 'm':
    case 'M':
	// do 'multipart booster' things here;
	multipartbooster=1;
	break;
    case 's':
    case 'S':
	// do 'standard speed loader' things here;
	noF16speed=1;
	break;
    case 'p':
    case 'P':
	longpause=1;
	break;
    case 'r':
    case 'R':
	if (strlen(param)>2)
	    {  paramnum = param +2;
		userrelocate=(int)strtol(paramnum, NULL, 16);
		if ((userrelocate < 1)||(userrelocate > 255))
		    {  printf("Bad relocation parameter: %s\n", param);
			printf("Example: -rA5 relocates Novalight at page $A5. Page $00 not allowed.\n");
			fileproblem=1;
			return 1;
		    }
	    }
	else
	    {  printf("Bad relocation parameter: %s.\n", param);
		printf("Example: -rA5 relocates Novalight at page $A5. Page $00 not allowed.\n");
		fileproblem=1;
		return 1;
	    }
	break;
    default:
	printf("Bad parameter: %s\n", param);
	printf("Usage: [ -s|p|m|rxx ] [ -s|p|m|rxx ] [ -s|p|m|rxx ] <.TAP file> <.WAV file>\n");
	printf("(run the program without parameters for help)\n");
	return 1;
    }
    return 0;
}


int main(int argc,char *argv[])
{
    int i, j, trans, nomprog[17];
    unsigned char header[9], valin;
    char loader_header[9]={ 
			   0, /* not an array of integers */
			   0, /* not an array of strings */
			   0x80, /* memory block */
			   0xC7, /* AUTO-exec */
			   0x01,0xB3, /* end address (0x1B3) */
			   0x01,0x2E, /* start address (0x12E for the pre-loader) */
			   0xFF    /* unused, FF to load faster (F16 load "1" bits much faster than "0") */
    };

    if (init(argc,argv)) {
	if (fileproblem==0)
	    { printf("\nUsage: Novalight [ -opt1 ] [ -opt2 ] [ -opt3 ] [ -opt4 ] <.TAP file> <.WAV file>\n\n");
		printf("Produces a 44 kHz WAV file that should load very fast on most Orics,\n");
		printf("ROM 1.0 or ROM 1.1.\n\n");
		printf("Options: -s  'standard speed': use standard speed instead of F16 speed\n");
		printf("             for the loader (slower, but some Orics may not like F16?)\n");
		printf("         -p  'long pause': generates a 5 seconds silence between each part\n");
		printf("             of multipart programs (ERE Informatique programs, for instance,\n");
		printf("             require time to draw a loading screen).\n");
		printf("         -m  'multipart booster': if you know that page 1 is never affected by\n");
		printf("             the multipart program you're loading, this option allows to reduce\n");
		printf("             the loader size after part 1 (all following parts load faster!).\n");
		printf("         -rXX 'relocate': you can specify another memory page to store\n");
		printf("             Novalight. Must be in hexadecimal, for instance '-r9F', '-r01'.\n");
		printf("             Default uses page 1, or another if something is loaded in page 1.\n");
		printf("Note:    dropped 'no loader' and 'old loader' options, that worked together,\n");
		printf("         since the old loader became too big to remain compatible.\n\n");
		printf("WARNING: requires a *perfect* WAV player. DO NOT convert the WAV file.\n");
	    }
	printf("(v1.2a - 03/2019)\n");
	exit(1);
    }

    // Check if program has to be relocated

    relocate();

    if (userrelocate>0)
	{  loader_header[4]= userrelocate;
	    loader_header[6]= userrelocate;
	}

    fwrite(&sample_riff,1,sizeof(sample_riff),out);

    emit_silence(3000);

    // Loop, for each program in the TAP file
    while (!feof(in)) {
	do
	    { valin=0; // avoids fake RLE when end address program goes beyond the end of file.
		if (fgetc(in)==0x16)
		    {  if (feof(in)) break;
			valin=fgetc(in);        /* read synchro (0x24 included) */
			if (feof(in)) break;
			if (valin!=0x24) fseek(in, -1, SEEK_CUR);
		    }
	    } while ((valin!=0x24)&&(!feof(in)));
	if (feof(in)) break;
 
	if (valin==0x24)        /* a 0x16 0x24 sequence has been found, this is not exactly the ROM sequence, which is */
	    {                       /* 0x16 0x16 0x16 0x16 (anything in between) 0x24; hoping this won't give problems */

		// 1- Read header, name and program
		for (i=0;i<9;i++) header[i]=fgetc(in);  /* header */
		start=header[6]*256+header[7];
		end=header[4]*256+header[5];

		for (i=0;i<40;i++) name[i]=0;   /* initialisation */
		i=0;                            /* name, hoping it won't be over 40 chars. 16 max in theory */
		while (((name[i++]=fgetc(in))!=0)&&(!feof(in)));   /* but ROM allows unlimited size, not reading over 16 */

		prog_size=0;
		for (i=0;i<(end-start)+1;i++)
		    {  Mem[i]=fgetc(in);  /* program */
			if (feof(in)) break;  /* sometimes the last part of the TAP is shorter than the addresses difference. */
			prog_size++;  /* so calculate the actual real size */
		    }
		if (prog_size!=((end-start)+1)) printf("\n*Warning*: program %i size does not match the addresses difference.\n", prognum);


		// 1.5- Test: if it's the ALC program, skip it

		//    Testing the full ALC program, as some French companies (Loriciels, Infogrames, Cobra Soft) liked to hide
		//    a protection inside. In such case, we need to keep modified the program, to get a change to load the rest.

		test_ALC();

		if (ALC==1)
		    { printf("\n --- ALC correction program detected; skipped (useless).\n");
			ALC=0;
		    }
		else
		    {
			prognum++;

			// 2- Create program dictionary

			creation_dico();


			// 3- Emit loader, loaded by CLOAD. Using F16 speed or standard speed.
			if ((prognum>1)&&(multipartbooster==1)) //if this is not the 1st part, and multipart booster asked
			    {  loader_header[7]= 0x8C;  /* update header's program size with short loader instead of long one */
				loader_header[5]= 0xAE;
			    }
			if (noF16speed==0)
			    {  for (i=0;i<30;i++) emit_F16_byte(0x16); /* 30 synchro bytes */
				emit_F16_byte(0x24);
				for (i=0;i<9;i++) emit_F16_byte(loader_header[i]);
				for (i=0;name[i];i++) emit_F16_byte(name[i]);  // Emit program name so CLOAD"xyz" still works. Smart C code
				// (not by me): stops when name[i]=0x00
				emit_F16_byte(0);
				emit_gap();  /* works for both F16 or Novalight speed */
				if ((prognum>1)&&(multipartbooster==1)) //if this is not the 1st part, and multipart booster asked
				    {  for (i=0;i<35;i++) emit_F16_byte(loader_common_area_1bis[i]);
				    }
				else   //default: normal loader for 1st part and next ones if multipart booster NOT asked
				    {  for (i=0;i<13;i++) emit_F16_byte(loader_oric[i]);  // 13 normal bytes intended to decode the following 121 inverted ones
					for (i=13;i<134;i++) emit_F16_byte(0xDF^loader_oric[i]); // invert bits value (except the 3rd), this saves 10% of
					// loading time since "1" loads faster than "0"
					// 3rd bit already has a small majority of "1"!
				    }
			    }
			else
			    {  for (i=0;i<30;i++) emit_standard_byte(0x16); /* 30 synchro bytes */
				emit_standard_byte(0x24);
				for (i=0;i<9;i++) emit_standard_byte(loader_header[i]);
				for (i=0;name[i];i++) emit_standard_byte(name[i]);  // Emit program name so CLOAD"xyz" still works. Smart C code
				// (not by me): stops when name[i]=0x00
				emit_standard_byte(0);
				emit_gap();  /* works for both F16 or Novalight speed */
				if ((prognum>1)&&(multipartbooster==1)) //if this is not the 1st part, and multipart booster asked
				    {  for (i=0;i<35;i++) emit_standard_byte(loader_common_area_1bis[i]);
				    }
				else   //default: normal loader for 1st part and next ones if multipart booster NOT asked
				    {  for (i=0;i<13;i++) emit_standard_byte(loader_oric[i]);  // 13 normal bytes intended to decode the following 121 inverted ones
					for (i=13;i<134;i++) emit_standard_byte(0xDF^loader_oric[i]); // invert bits value (except the 3rd), this saves 10% of
					// loading time since "1" loads faster than "0"
					// 3rd bit already has a small majority of "1"!
				    }
				printf("Standard speed asked for loader.\n");
			    }
			emit_silence(500); /* short silence since Novalight doesn't require any special time. Separates programs in WAV file */


			// 3.5- Emit Common Area 0a and 0, reversed, at Novalight synchro speed
			emit_gap();
			if ((multipartbooster==0)||(prognum==1))
			    {  // /!\ reversed order /!\ (loaded together in ASM code)
				for (i=38;i>=0;i--) emit_synchro_byte(loader_common_area_0[i]);  /* Common Area 0 */
				for (i=21;i>=0;i--) emit_synchro_byte(loader_common_area_0a[i]);  /* Common Area 0a  */
			    }

			// (now useless)      emit_gap();


			// 4- Emit program header, Novalight synchro speed

			for(i=0;i<10;i++)
			    { emit_bit(3);    /* 30 stop bits for synchro at start          */
				emit_bit(3);    /* (because decoding signal doesn't synchronize at bit level)*/
				emit_bit(3);    /* stop */
			    }

			emit_gap();

			// Header etc. : 292-2B1 (32 bytes).
			// The header is stored in memory in reversed mode, so has to be emitted reversed.
 
			// 2B1: set to 0 (originally used to count the amounts of errors read on tape, unused with Novalight)
			emit_synchro_byte(0);
	  
			// - 2B0-2A8: header (already reversed!)
			for (i=0;i<9;i++) emit_synchro_byte(header[i]);  /* header */
			// NOTE: 2A8 is unused, so could be used for another purpose

			// - 2A7-2A4: 0 (unused by Oric, could be used for something?)
			emit_synchro_byte(0);
			emit_synchro_byte(0);
			emit_synchro_byte(0);
			emit_synchro_byte(0);
  
			// NOTE: 2A3 will always = 0
			// - 2A3-293: zeroes + program name reversed  (nomprog[17])
			for (j=0;j<17;j++) nomprog[j]=0;   /* initialization */

			for(j=16;j>0;j--)            /* name truncated to 16 chars from TAP + #00 (maximum size in Oric RAM page 2)*/
			    {  nomprog[j]=name[16-j];    /* (truncated because Oric programs in TAP files can be hacked to have more) */
			    }

			for (j=0;j<17;j++)
			    {  i= nomprog[j];
				emit_synchro_byte(i);   /* emit name */
				// printf("Name value: %d \n", i);
			    }
 
			// 292: zero (allows in the code to have A already set to 0 with this last byte, saves a LDA#00 if required)
			// TOTAL (without dictionary)=32 bytes
			emit_synchro_byte(0);

			// 5- Emit Common Area 2, reversed, at Novalight synchro speed

			for (i=59;i>=0;i--) emit_synchro_byte(loader_common_area_2[i]);  /* Common Area 2 */

			// End of header, give some time to let 'Loading' being displayed
			emit_gap();

			// /!\ reversed order /!\ (loaded together in ASM code)
			// 5.3- Emit Common Area 0b, reversed, at Novalight synchro speed

			for (i=4;i>=0;i--) emit_synchro_byte(loader_common_area_0b[i]);  /* Common Area 0b */

			// 5.6 - Emit dictionary (103-110), 14 bytes, in two 7 bytes long tables,
			//       leave it un-reversed since the decoding with SBC is done reversed...

			// Table 2 = bytes 2,4,6,8,10,12,14
			for (i=1;i<=13;i=i+2)
			    {  emit_synchro_byte(dico[i][0]);
			    }
			// Table 1 = bytes 1,3,5,7,9,11,13
			for (i=0;i<=12;i=i+2)
			    {  emit_synchro_byte(dico[i][0]);
			    }
	  

			// 6- Emit Common Area 3, reversed, at Novalight synchro speed

			for (i=60;i>=0;i--) emit_synchro_byte(loader_common_area_3[i]);  /* Common Area 3 */

			// Not gap required here
			//emit_gap();


			// 7- Emit Program, at full Novalight speed

			for (i=0;i<prog_size;i++) emit_byte(Mem[i]);

			emit_byte(-1); // to mark the end of program and emit the potentially last repeated bytes.
			// /!\ Byte not emitted itself in the WAV signal.
			emit_bit(9);   // 2 very last bits so Oric can measure the last sinudoïd and detect end of program - one is not enough?
			emit_bit(9);


			// 8- Emit Common Area 4, reversed, at Novalight synchro speed
			// Resynch in the decoder so emit gap here - lets the Oric work
			emit_gap();

			for (i=62;i>=0;i--) emit_synchro_byte(loader_common_area_4[i]);  /* Common Area 4 */


			// 9- Re-emit (restore) Common Area 0a, reversed, at Novalight synchro speed
			// emit_gap();
			for (i=21;i>=0;i--) emit_synchro_byte(loader_common_area_0a[i]);  /* Common Area 0a */


			// 10- End
			emit_bit(3); /* 2 very last bits so Oric can measure the last sinudoïd and detect end of program - one is not enough? */
			emit_bit(3);

			emit_silence(15000);
			if(longpause==1) emit_silence(205000);
		    }
	    }
    }
    fclose(in);

    sample_riff.datalength=file_size;
    sample_riff.riff_size=sample_riff.datalength+8+sample_riff.fmtsize+12;
    fseek(out,0,SEEK_SET); 
    fwrite(&sample_riff,1,sizeof(sample_riff),out);

    fclose(out);
    printf("Compression stats: %.2f%% none - %.2f%% RLE - %.2f%% dictionary.\n\n", statnormal*100/stattotal, statRLE*100/stattotal, statdictionary*100/stattotal);
    printf("Novalight v1.2a (c) 03/2019 --- File converted, using page #%02X.\n", userrelocate);
}
