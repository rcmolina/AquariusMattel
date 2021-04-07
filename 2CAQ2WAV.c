//CAQ2WAV.c
//  Take a .CAQ file and save it out as .WAV
//	James the Animal Tamer rewritten 2005

#define DWORD unsigned int

//#include <windows.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
//#include <mmsystem.h>
//#include "resource.h"

char  szAppName[] = "CAQ2WAV" ;
char	*gszAppName = &szAppName[0];

//HINSTANCE   ghInst = NULL;           // app's instance handle

#define LOADSTRBUFSIZE 256
char    lpstrLoadStrBuf[LOADSTRBUFSIZE] = "\0";

//HWND	  hwndMain = 0;
FILE	n;
static char			szfilenamei[_MAX_PATH];

static char			szfilenameLog[_MAX_PATH] = "CAQ2WAV_log.txt";

unsigned char       g_cflLogEnable = 0;
FILE                *g_fileLog = NULL; 

static char         szfilenametestwavo[_MAX_PATH];


static char s_cjtat[] = 
{
	13,10,13,10,32,84,104,105,
	115,32,112,114,111,103,114,97,
	109,32,119,97,115,13,10,32,
	111,114,105,103,105,110,97,108,
	108,121,32,119,114,105,116,116,
	101,110,13,10,32,98,121,32,
	13,10,32,74,97,109,101,115,
	32,116,104,101,32,65,110,105,
	109,97,108,32,84,97,109,101,
	114,13,10,13,10, 0
};


static char szFilter[] = "CAQ Files (*.CAQ)\0*.caq\0"  \
                              "All Files (*.*)\0*.*\0\0" ;



char    g_cflSine = 0;      //1= sine wave output, 0 = square wave output

#define NUMSAMPLES_SILENCE_44K   36
#define NUMSAMPLES_HALFSHORT_44K 12
#define NUMSAMPLES_HALFLONG_44K  24

#define NUMSAMPLES_SILENCE_22K   18
#define NUMSAMPLES_HALFSHORT_22K 6
#define NUMSAMPLES_HALFLONG_22K  12

#define SPEED_44K 44100
#define SPEED_22K 22050

#define PI (3.141592653589793)

//int     g_nSamplesPerSecond = SPEED_44K;
//int     g_nSamples_Silence = NUMSAMPLES_SILENCE_44K;
//int     g_nSamples_HalfShort = NUMSAMPLES_HALFSHORT_44K;
//int     g_nSamples_HalfLong = NUMSAMPLES_HALFLONG_44K;

int     g_nSamplesPerSecond = SPEED_22K;
int     g_nSamples_Silence = NUMSAMPLES_SILENCE_22K;
int     g_nSamples_HalfShort = NUMSAMPLES_HALFSHORT_22K;
int     g_nSamples_HalfLong = NUMSAMPLES_HALFLONG_22K;


int main(int argc, char *argv[])
{

strcpy(szfilenamei, argv[1]);
MakeOutputWave();

}

//LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;

/*
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR szCmdLine, int iCmdShow)
{
     HWND         hwnd ;
     MSG          msg ;
     WNDCLASSEX   wndclass ;

	ghInst = hInstance;

     wndclass.cbSize        = sizeof (wndclass) ;
     wndclass.style         = CS_HREDRAW | CS_VREDRAW;
     wndclass.lpfnWndProc   = WndProc ;
     wndclass.cbClsExtra    = 0 ;
     wndclass.cbWndExtra    = DLGWINDOWEXTRA ;
     wndclass.hInstance     = hInstance ;
     wndclass.hIcon         = LoadIcon (hInstance, (LPCTSTR)IDI_ICON1) ;
     wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
     wndclass.hbrBackground = (HBRUSH) (COLOR_BTNFACE + 1) ;
     wndclass.lpszMenuName  = NULL ;
     wndclass.lpszClassName = szAppName ;
     wndclass.hIconSm       = LoadIcon (hInstance, (LPCTSTR)IDI_ICON1) ;

     RegisterClassEx (&wndclass) ;

     hwnd = CreateDialog (hInstance, (LPCTSTR)szAppName, 0, NULL) ;
	hwndMain = hwnd;

    ShowWindow (hwnd, iCmdShow) ;
	PostMessage(hwndMain, WM_COMMAND, IDC_DISABLEBUTS, 0);

	szfilenamei[0] = 0;

	//szfilenameLog[0] = 0;
    g_fileLog = NULL;


	ZeroMemory( (void *)&ofn, sizeof(OPENFILENAME));
     ofn.lStructSize       = sizeof (OPENFILENAME) ;
     ofn.hwndOwner         = hwndMain ;
     ofn.lpstrFilter       = szFilter ;
     ofn.nMaxFile          = _MAX_PATH ;
     ofn.nMaxFileTitle     = _MAX_FNAME + _MAX_EXT ;
     ofn.lpstrDefExt       = "wav" ;

     ofn.lpstrFile         = szfilenamei;
//     ofn.lpstrFileTitle    = pstrTitleName ;
     ofn.Flags             = OFN_FILEMUSTEXIST ;





     while (GetMessage (&msg, NULL, 0, 0))
     {
          TranslateMessage (&msg) ;
          DispatchMessage (&msg) ;
     }
     return msg.wParam ;
}

*/
#pragma pack( 1 )
struct	tagfheader
{
	unsigned char riff[4];
	DWORD			llen1;
	unsigned char	wave[4];
	unsigned char	fmt[4];
	unsigned char	unk[8];
	DWORD			samppers1;
	DWORD			samppers2;
	unsigned char	unk2[6];
	unsigned char	data[4];
	DWORD			dwnsamples;
};
typedef struct tagfheader	 FHEADER;
//RIFF
FHEADER	g_fheader =
{
	{'R', 'I', 'F', 'F'},   //
	0,                      //
	{'W', 'A', 'V', 'E'},   //
	{'f', 'm', 't', ' '},
	{0x12, 0, 0, 0,  
        1, 0,
        1, 0},      //nChannels
	44100,              //nSamplesPerSec
	44100,              //nAvgBytesPerSec
	{1, 0,          //nBlockAlign
         8, 0,      //wBitsPerSample
         0, 0},     //cbSize
	{'d', 'a', 't', 'a'},
	0
};

char	g_szerr[256];


#define WAVE_SILENCE 0
#define WAVE_SHORT 1
#define WAVE_LONG 2

//values used for high and low points of a wave
#define WAV_H 240
#define WAV_L 16
#define WAV_SILENCE 0x80
//amplitude
#define WAV_A (WAV_H - WAV_SILENCE)

unsigned long   g_lTestWaveSamplesWritten = 0;


//Making these buffers extra extra long
unsigned char   g_cbufShortFullWave[NUMSAMPLES_HALFSHORT_44K * 8];
unsigned char   g_cbufLongFullWave[NUMSAMPLES_HALFLONG_44K * 8];
unsigned char   g_cbufSilenceFullWave[NUMSAMPLES_SILENCE_44K];


void    SetupOutputWavelengths(void)
{
    double  fwidthfudge, fwidthfudgei;
    int j, k;

    fwidthfudge = 0.0;
    fwidthfudgei = 0.5;


    for (j = 0; j < g_nSamples_Silence; j++)
    {
        g_cbufSilenceFullWave[j] = WAV_SILENCE;
    }
    if (g_cflSine)
    {
        //sine waves
        //Full wavelength of short
        for (j = 0; j < g_nSamples_HalfShort; j++)
        {
            //g_cbufShortFullWave[j] = WAV_L;
			k = j - g_nSamples_HalfShort;
			g_cbufShortFullWave[j] = WAV_SILENCE - (unsigned char)( (int)( (double)WAV_A*sin(  ((double)k+fwidthfudgei) * PI / ((double)g_nSamples_HalfShort + fwidthfudge) )  )   );
        }
        for (j = g_nSamples_HalfShort; j < 2 * g_nSamples_HalfShort; j++)
        {
            //g_cbufShortFullWave[j] = WAV_H;
			k = j;
			g_cbufShortFullWave[j] = WAV_SILENCE + (unsigned char)( (int)( (double)WAV_A*sin(  ((double)k+fwidthfudgei) * PI / ((double)g_nSamples_HalfShort + fwidthfudge) ) ) );
        }

        //Full wavelength of long
        for (j = 0; j < g_nSamples_HalfLong; j++)
        {
            //g_cbufLongFullWave[j] = WAV_L;
			k = j - g_nSamples_HalfLong;
			g_cbufLongFullWave[j] = WAV_SILENCE - (unsigned char)( (int)( (double)WAV_A*sin(  ((double)k+fwidthfudgei) * PI / ((double)g_nSamples_HalfLong + fwidthfudge) ) ) );
        }
        for (j = g_nSamples_HalfLong; j < 2 * g_nSamples_HalfLong; j++)
        {
            //g_cbufLongFullWave[j] = WAV_H;
			k = j;
			g_cbufLongFullWave[j] = WAV_SILENCE + (unsigned char)( (int)( (double)WAV_A*sin(  ((double)k+fwidthfudgei) * PI / ((double)g_nSamples_HalfLong + fwidthfudge) )      )   );
        }
    }
    else
    {
        //square waves

        //Full wavelength of short
        for (j = 0; j < g_nSamples_HalfShort; j++)
        {
            g_cbufShortFullWave[j] = WAV_L;
        }
        for (j = g_nSamples_HalfShort; j < 2 * g_nSamples_HalfShort; j++)
        {
            g_cbufShortFullWave[j] = WAV_H;
        }


        //Full wavelength of long
        for (j = 0; j < g_nSamples_HalfLong; j++)
        {
            g_cbufLongFullWave[j] = WAV_L;
        }
        for (j = g_nSamples_HalfLong; j < 2 * g_nSamples_HalfLong; j++)
        {
            g_cbufLongFullWave[j] = WAV_H;
        }
    }
}



void    OutputTestWavelength(FILE *filewavo, int ncomputertype, int nwavetype)
{
//    int j;
    int nsampleswritten;

    nsampleswritten = 0;

    if (filewavo)
    {
        switch(nwavetype)
        {
            case WAVE_SILENCE:
                fwrite(g_cbufSilenceFullWave, 1, g_nSamples_Silence, filewavo);
                nsampleswritten = g_nSamples_Silence;
                break;
            case WAVE_SHORT:
                fwrite(g_cbufShortFullWave, 1, 2 * g_nSamples_HalfShort, filewavo);
                nsampleswritten = 2 * g_nSamples_HalfShort;
                break;
            case WAVE_LONG:
                fwrite(g_cbufLongFullWave, 1, 2 * g_nSamples_HalfLong, filewavo);
                nsampleswritten = 2 * g_nSamples_HalfLong;
                break;
            default:
                break;
        }
    }
    g_lTestWaveSamplesWritten += nsampleswritten;
}



//Aquarius:
//  Each bit is two full waves.
//      Bit 0 is four half-longs
//      Bit 1 is four half-shorts
//      A half short is 9 samples @ 44,100 MHz
//      A half long is 18 samples
//  Each byte is:
//      bit of 0 for start bit
//      eight data bits, MSB first
//      bit of 1 for stop bit
//      bit of 1 for stop bit
//
//  Blocking:
//      Synch Block: at least 6 bytes of 0xFF followed by end-of-synch byte of 0x00
//      Name block:  0-terminated variable length string (BASIC uses up to 6 long)
//          Name block is followed by Synch block
//      Main data block:  Stream of data, no subdividing.
//
//  NOTE:  no checksums, no quality-of-data.  It sucks, it's unreliable.

#define MODE_PRENAMESYNCH 0
#define MODE_GETNAMEBLOCK 1
#define MODE_POSTNAMESYNCH 2
#define MODE_GETMAINBLOCK 3

void    MakeOutputWave(void)
{
    long    lfilelen;
    long    lposinfile;
    int     j;
    int     ncharin;

    FILE    *filei = NULL;
	char	*pperiod;
    FILE        *filetestwavo;
    int         nLeaderLength;
    int             nmode;
    int             nbyteinblock;
    int             nbytesinblock;
    int             nblocknumber;
    //int             ndatatype, nchecksum;
    int             nWriteSilence, nsamples;
    int             nchecksummismatches;
    unsigned char   *pfile;
    unsigned char   cmout;
    unsigned char   cmin;


    nLeaderLength = g_nSamplesPerSecond/2;
    SetupOutputWavelengths();

    pfile = NULL;

    filei = fopen(szfilenamei, "rb");
    if (filei)
    {
        fseek(filei, 0, SEEK_END);
        lfilelen = ftell(filei);
        if (lfilelen > 0)
        {
            fseek(filei, 0, SEEK_SET);
            pfile = malloc(lfilelen + 128);
            if (pfile)
            {
                fread(pfile, 1, lfilelen, filei);
            }
        }

        fclose(filei);
    }
    if (!pfile)
        return;

    if (pfile)
    {
        g_lTestWaveSamplesWritten = 0;
        nchecksummismatches = 0;

	    strcpy(szfilenametestwavo, szfilenamei);
	    pperiod = strrchr(szfilenametestwavo, '.');
	    if (pperiod)
	    {
		    *pperiod = 0;
	    }
        strcat(szfilenametestwavo, "_");
        if (g_nSamplesPerSecond == SPEED_44K)
            strcat(szfilenametestwavo, "4");
        else if (g_nSamplesPerSecond == SPEED_22K)
            strcat(szfilenametestwavo, "2");
        if (g_cflSine)
            strcat(szfilenametestwavo, "N");
        else        
            strcat(szfilenametestwavo, "Q");
        strcpy(szfilenameLog, szfilenametestwavo);
	    strcat(szfilenametestwavo, ".WAV");


        //diversion here to make the log file name
        strcat(szfilenameLog, "_log.TXT");
        //end diversion
        filetestwavo = fopen(szfilenametestwavo, "wb");
        if (filetestwavo)
        {
            if (g_cflLogEnable)
                g_fileLog = fopen(szfilenameLog, "w");
            fwrite((void *)&g_fheader, 1, sizeof(FHEADER), filetestwavo);
            cmout = WAV_SILENCE;
            for (j = 0; j < nLeaderLength; j++)
            {
                fwrite((void *)&cmout, 1, 1, filetestwavo);
            }
            g_lTestWaveSamplesWritten += nLeaderLength;


            nbyteinblock = 0;
            nbytesinblock = 0;
            nblocknumber = 0;
            nWriteSilence = 0;
            nmode = MODE_PRENAMESYNCH;

            lposinfile = 0;
            
            while (lposinfile < lfilelen)
            {
                cmin = pfile[lposinfile];

                ncharin = (int)cmin;    //fossil left over from time we were reading the file
                if (g_fileLog)
                {
                    if (ncharin >= ' ' && ncharin <= '~')
                    {
                        fprintf(g_fileLog, "Inbyte $%02x '%c' at file position $%04x\n", ncharin, ncharin, lposinfile);
                    }
                    else
                    {
                        fprintf(g_fileLog, "Inbyte $%02x     at file position $%04x\n", ncharin, lposinfile);
                    }
                }

                switch(nmode)
                {
                    case MODE_PRENAMESYNCH:
                        if (g_fileLog)
                        {
                            fprintf(g_fileLog, "prename synch, byte in block %d decimal, current bytein $%02x\n", nbyteinblock, ncharin);
                        }
                        nbyteinblock++;
                        if (ncharin == 0x00)
                        {
                            //nWriteSilence = 686-418;
                            nmode = MODE_GETNAMEBLOCK;
                            nbyteinblock = 0;
                        }
                        break;
                    case MODE_GETNAMEBLOCK:
                        if (g_fileLog)
                        {
                            fprintf(g_fileLog, "name block, byte in block %d decimal, current bytein $%02x\n", nbyteinblock, ncharin);
                        }
                        nbyteinblock++;
                        if (ncharin == 6)
                        {
                            nbyteinblock = 0;
                            //nWriteSilence = 400;
                            nmode = MODE_POSTNAMESYNCH;
                        }
                        break;
                    case MODE_POSTNAMESYNCH:
                        if (g_fileLog)
                        {
                            fprintf(g_fileLog, "postname synch, byte in block %d decimal, current bytein $%02x\n", nbyteinblock, ncharin);
                        }
                        nbyteinblock++;
                        if (ncharin == 0x00)
                        {
                            //nWriteSilence = 3601-418;
                            nmode = MODE_GETMAINBLOCK;
                            nbyteinblock = 0;
                        }
                        break;
                    case MODE_GETMAINBLOCK:
                        if (g_fileLog)
                        {
                            fprintf(g_fileLog, "Main block, byte in block %d decimal, current bytein $%02x\n", nbyteinblock, ncharin);
                        }
                        nbyteinblock++;
                        break;
                }


                if (nWriteSilence)
                {
                    //nWriteSilence is in cycles
                    cmout = WAV_SILENCE-1;
                    //samples = cycles * sec/cycle * samp/sec
                    nsamples = (int)((double)nWriteSilence * (double)g_nSamplesPerSecond / (14318180.0 / 4.0));
                    if (nsamples > 0)
                    {
                        if (g_fileLog)
                            fprintf(g_fileLog, "Write Silence cycles =%d, samples = %d\n", nWriteSilence, nsamples);
                        for (j = 0; j < nsamples; j++)
                        {
                            fwrite((void *)&cmout, 1, 1, filetestwavo);
                        }
                        g_lTestWaveSamplesWritten += nsamples;
                    }
                    nWriteSilence = 0;
                }

                //Starting a byte
                cmout = (unsigned char)ncharin;
                if (g_fileLog)
                    fprintf(g_fileLog, "#%d Output Byte %02x\n", lposinfile, cmout);
                //Aquarius:
                //  Each bit is two full waves.
                //      Bit 0 is four half-longs
                //      Bit 1 is four half-shorts
                //  Each byte is:
                //      bit of 0 for start bit
                //      eight data bits, MSB first
                //      bit of 1 for stop bit
                //      bit of 1 for stop bit
                
                //start bit of 0
                OutputTestWavelength(filetestwavo, 0, WAVE_LONG);
                OutputTestWavelength(filetestwavo, 0, WAVE_LONG);
                for (j = 7; j >= 0; j--)
                {
                    if (cmout & (1 << j))
                    {
                        //1 bit, two full shorts
                        OutputTestWavelength(filetestwavo, 0, WAVE_SHORT);
                        OutputTestWavelength(filetestwavo, 0, WAVE_SHORT);
                    }
                    else
                    {
                        //0 bit, two full longs
                         OutputTestWavelength(filetestwavo, 0, WAVE_LONG);
                         OutputTestWavelength(filetestwavo, 0, WAVE_LONG);
                    }
                }
                //stop bit of 1
                OutputTestWavelength(filetestwavo, 0, WAVE_SHORT);
                OutputTestWavelength(filetestwavo, 0, WAVE_SHORT);
                //stop bit of 1
                OutputTestWavelength(filetestwavo, 0, WAVE_SHORT);
                OutputTestWavelength(filetestwavo, 0, WAVE_SHORT);
                //write out post-byte delays here

                lposinfile++;
            } //while !EOF

            cmout = WAV_SILENCE;
            for (j = 0; j < nLeaderLength; j++)
            {
                fwrite((void *)&cmout, 1, 1, filetestwavo);
            }
            g_lTestWaveSamplesWritten += nLeaderLength;
            fclose(filetestwavo);
            //re-write the header output wave
	        g_fheader.dwnsamples = g_lTestWaveSamplesWritten;
	        g_fheader.llen1 = g_fheader.dwnsamples + sizeof(g_fheader) - 8;
            g_fheader.samppers1 = g_nSamplesPerSecond;
            g_fheader.samppers2 = g_nSamplesPerSecond;
	        //if (g_fheader.llen1 & 1)
		    //    g_fheader.llen1 ++;

            filetestwavo = fopen(szfilenametestwavo, "r+b");
            if (filetestwavo)
            {
                fwrite((void *)&g_fheader, 1, sizeof(FHEADER), filetestwavo);
                fclose(filetestwavo);
            }

            if (g_fileLog)
            {
                fclose(g_fileLog);
                g_fileLog = NULL;
            }
        }   //if filetestwavo
        free(pfile);
        if (nchecksummismatches)
        {
            sprintf(g_szerr, "ERROR:  %d checksum mismatches", nchecksummismatches);
        }
        else
        {
            sprintf(g_szerr, "--");
        }
        //SetDlgItemText(hwndMain, IDC_ERROUT, g_szerr);
    }   //if pfile
}




/*
LRESULT CALLBACK WndProc (HWND hwnd, UINT iMsg, WPARAM wparam, LPARAM lparam)
{
//	int		j;
	FILE	*fi;
	WORD	wid;
//	unsigned char	szerr[256];

	switch (iMsg)
	{

		case WM_COMMAND :
			SetFocus (hwnd) ;
			wid = LOWORD(wparam);
			switch(wid)
			{
				case IDC_DISABLEBUTS:
					//This serves as a sort of initdialog

                    CheckDlgButton(hwnd, IDC_TMQFASTFAST, BST_UNCHECKED);
                    CheckDlgButton(hwnd, IDC_44K, BST_CHECKED);
                    CheckDlgButton(hwnd, IDC_22K, BST_UNCHECKED);
                    CheckDlgButton(hwnd, IDC_SQUARE, BST_CHECKED);
                    CheckDlgButton(hwnd, IDC_SINE, BST_UNCHECKED);
					break;

				case IDC_BROWSE_I:
					ofn.lpstrFile = szfilenamei;
					ofn.Flags =  OFN_FILEMUSTEXIST;
					if (GetOpenFileName(&ofn))
					{
						fi = NULL;
						fi = fopen(ofn.lpstrFile, "rb");
						if (fi)
						{
							fclose(fi);
							SetDlgItemText(hwnd, IDC_IN, szfilenamei);

                            if (BST_CHECKED == IsDlgButtonChecked(hwnd, IDC_22K))
                            {
                                g_nSamplesPerSecond = SPEED_22K;
                                g_nSamples_Silence = NUMSAMPLES_SILENCE_22K;
                                g_nSamples_HalfShort = NUMSAMPLES_HALFSHORT_22K;
                                g_nSamples_HalfLong = NUMSAMPLES_HALFLONG_22K;
                            }
                            else
                            {
                                //44K
                                g_nSamplesPerSecond = SPEED_44K;
                                g_nSamples_Silence = NUMSAMPLES_SILENCE_44K;
                                g_nSamples_HalfShort = NUMSAMPLES_HALFSHORT_44K;
                                g_nSamples_HalfLong = NUMSAMPLES_HALFLONG_44K;
                            }
                            g_cflSine = 0;  //assume square wave
                            if (BST_CHECKED == IsDlgButtonChecked(hwnd, IDC_SINE))
                            {
                                g_cflSine = 1;  //sine
                            }
                            g_cflLogEnable = 0; //assume no log
                            if (BST_CHECKED == IsDlgButtonChecked(hwnd, IDC_ENABLELOG))
                            {
                                g_cflLogEnable = 1;  //output log file
                            }

                            MakeOutputWave();
						}
					}
					break;
				case IDCANCEL:
					PostMessage(hwnd, WM_SYSCOMMAND, SC_CLOSE, 0);
					break;
			}
			return 0 ;

		case WM_DESTROY :
            if (g_fileLog)
            {
                fclose(g_fileLog);
                g_fileLog = NULL;
            }
			PostQuitMessage (0) ;
			return 0 ;
		default:
			break;
	}
	return DefWindowProc (hwnd, iMsg, wparam, lparam) ;
}
*/

