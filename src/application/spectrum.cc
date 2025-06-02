// https://github.com/billthefarmer/tuner/tree/master/src/org/billthefarmer/tuner
// http://www.codeproject.com/Articles/32172/FFT-Guitar-Tuner
// http://habrahabr.ru/post/247385/
// https://code.google.com/p/ctuner/source/browse/trunk/windows/Tuner.c

#include "spectrum.h"
#include "rt_counter.h"
#include "int2str.h"
#include "filt.h"
#include "gu.h"

static const  char* note_name_table[] =
     		{
     				"C-1",
     				"C#-1",
     				"D-1",
     				"D#-1",
     				"E-1",
     				"F-1",
     				"F#-1",
     				"G-1",
     				"G#-1",
     				"A-1",
     				"A#-1",
     				"B-1",

     				"C0",
     				"C#0",
     				"D0",
     				"D#0",
     				"E0",
     				"F0",
     				"F#0",
     				"G0",
     				"G#0",
     				"A0",
     				"A#0",
     				"B0",


     				"C1",
     				"C#1",
     				"D1",
     				"D#1",
     				"E1",
     				"F1",
     				"F#1",
     				"G1",
     				"G#1",
     				"A1",
     				"A#1",
     				"B1",

     				"C2",
     				"C#2",
     				"D2",
     				"D#2",
     				"E2",
     				"F2",
     				"F#2",
     				"G2",
     				"G#2",
     				"A2",
     				"A#2",
     				"B2",

     				"C3",
     				"C#3",
     				"D3",
     				"D#3",
     				"E3",
     				"F3",
     				"F#3",
     				"G3",
     				"G#3",
     				"A3",
     				"A#3",
     				"B3",

     				"C4",
     				"C#4",
     				"D4",
     				"D#4",
     				"E4",
     				"F4",
     				"F#4",
     				"G4",
     				"G#4",
     				"A4",
     				"A#4",
     				"B4",

     				"3/C",
     				"3/C#",
     				"3/D",
     				"3/D#",
     				"3/E",
     				"3/F",
     				"3/F#",
     				"3/G",
     				"3/G#",
     				"3/A",
     				"3/A#",
     				"3/B",

     				"4/C",
     				"4/C#",
     				"4/D",
     				"4/D#",
     				"4/E",
     				"4/F",
     				"4/F#",
     				"4/G",
     				"4/G#",
     				"4/A",
     				"4/A#",
     				"4/B",

     				"5/C",
     				"5/C#",
     				"5/D",
     				"5/D#",
     				"5/E",
     				"5/F",
     				"5/F#",
     				"5/G",
     				"5/G#",
     				"5/A",
     				"5/A#",
     				"5/B",
     		};
static const uint8_t not_num[] = {
		0,1,2,3,4,5,6,7,8,9,10,11,
		0,1,2,3,4,5,6,7,8,9,10,11,
		0,1,2,3,4,5,6,7,8,9,10,11,
		0,1,2,3,4,5,6,7,8,9,10,11,
		0,1,2,3,4,5,6,7,8,9,10,11,
		0,1,2,3,4,5,6,7,8,9,10,11,
		0,1,2,3,4,5,6,7,8,9,10,11,
		0,1,2,3,4,5,6,7,8,9,10,11,
		0,1,2,3,4,5,6,7,8,9,10,11
};

TSpectrumTask* SpectrumTask ;

volatile float Fswe;

volatile float  Fs = 3999.91f;

const size_t N = 512 ;     // sample count
volatile size_t n = 256  ;   // frame overlap
vec in_0[N];
vec in_1[N];
vec __CCM_BSS__ Wfwd[32];
//float dBv[N];
//float dBw[N];

TSpectrumTask::TSpectrumTask ()
   {
	  guitar_classic_index_table[0] = 28 ;  // 28  0082.406 Hz  Big/E
      guitar_classic_index_table[1] = 33 ;  // 33  0110.000 Hz  Big/A
      guitar_classic_index_table[2] = 38 ;  // 38  0146.832 Hz  Small/D
      guitar_classic_index_table[3] = 43 ;  // 43  0196.000 Hz  Small/G
      guitar_classic_index_table[4] = 47 ;  // 47  0246.941 Hz  Small/H
      guitar_classic_index_table[5] = 52 ;  // 52  0329.627 Hz  1.00/E

      guitar_bass_index_table[0] = 11 ;  // 11  0030.867 Hz  SubContre/B ( 5/6 sring guitar )
      guitar_bass_index_table[1] = 16 ;  // 16  0041.203 Hz  Contre/E
      guitar_bass_index_table[2] = 21 ;  // 21  0055.000 Hz  Contre/A
      guitar_bass_index_table[3] = 26 ;  // 26  0073.416 Hz  Big/D
      guitar_bass_index_table[4] = 31 ;  // 31  0097.998 Hz  Big/G
      guitar_bass_index_table[5] = 36 ;  // 36  0130.812 Hz  Small/C     ( 6 sring guitar )

      fft_0.init(in_0,N,Wfwd,0);
      fft_1.init(in_1,N,Wfwd,0);

	  Set_filt_LPF(500.0f);

	  ref_freq = 440.0f ;
   } ;
volatile uint8_t notee;
void TSpectrumTask::ToneMeter()
{

  for (size_t n = 0 ; n < N ; n++)
  {
	  float h = kgp_math::window::Hann(n,N);
	  in_0[n] *= h ;
	  in_1[n] *= h ;
  }

//  rt_counter_start(counter);
  fft_0.transform() ;
  fft_1.transform() ;
//  ffts_x2_uS = rt_counter_stop_us(counter);

//  rt_counter_start(counter);

  kgp_math::find_tone( Kmes, fft_0, fft_1, n) ;

  tone =  fft_0.k2freq( Kmes , Fs ) ;

//  uS_find_tone = rt_counter_stop_us(counter);
//  rt_counter_start(counter);

  tone += 0.001f;

  size_t note ;
  Tone2NoteAndDiff(tone, note, freq_diff);

  extern volatile uint8_t t_no;
  t_no = not_num[note];
  note_name = note_name_table[note] ;

//  note_and_diff_uS = rt_counter_stop_us(counter);

  int aa = freq_diff * 1000.0f;
  float bb = aa / 1000.0f;

  extern uint8_t t_po;
  if(freq_diff < 0)
  {
	  float a = HalfTone(note - 1);
	  float b = HalfTone(note);
	  float c = (b - a) * 0.5f;
	  t_po = (uint8_t)((64.0 - (fabsf(bb) * (64.0/c))));
  }
  else {
	  float a = HalfTone(note + 1);
	  float b = HalfTone(note);
	  float c = (a - b) * 0.5f;
	  t_po = (uint8_t)(((bb * (64.0/c))) + 64.0);
  }
  notee = 1;
}

void SpectrumBuffsUpdate (float u)
{
  static size_t index = 0 ;

  if(eSuspended == TTaskUtilities::GetTaskState( SpectrumTask->GetHandle()))
   {
	  u = filt_lp(u);
	  vec val(u , 0 );
	  if ( index < N)
	    in_0[ index ] = vec(u , 0 ) ;

     if ( index >= n )
	    in_1[ index - n ] = vec(u , 0 ) ;

     index++ ;
     if ( index == N + n )
        {
    	  if(tuner_use)SpectrumTask->Resume();
          index = 0 ;
        }
    }
}

void TSpectrumTask::Code()
{

	DMA_ITConfig ( DMA1_Stream2 , DMA_IT_TC , ENABLE);
	DMA_ITConfig ( DMA1_Stream2 , DMA_IT_HT , ENABLE);

    while(1)
     {
       Suspend();
       ToneMeter();
     }
}

//-------------------------------------------------------
void TSpectrumTask::PrintSpertrum(vec* in, size_t N)
{
  /*    for ( size_t i = 0 ; i < N ; i++ )
	{
		for ( size_t k = 0 ; k < N ; k++ )
		{
			const char* sym = in[k].norma() >= N-i ? "*" : " " ;
		        rmsg("%c" , sym) ;

		}
		rmsg("\n") ;
	}*/
}
void TSpectrumTask::PrintSpertrumDB(float* in, float low_level_db, size_t N)
{
    /*  for ( size_t i = 0 ; i < N ; i++ )
	{
		for ( size_t k = 0 ; k < N ; k++ )
		{
			const char* sym = in[k]/10 >= low_level_db/10 ? "*" : " " ;
		        rmsg("%c" , sym) ;

		}
		rmsg("\n") ;
	}*/
}
//------------------------------------------------------
void TSpectrumTask::Sinus( float Fin, float Fs, size_t N, vec* in, size_t offset )
{
   for ( size_t i = 0 ; i < N ; i++ )
     {
       float s,c ;

       vdt::fast_sincosf( (Fin / Fs) * ( i + offset) * vdt::details::VDT_2PI_F , s,c);
       in[i] = vec (c,s);
     }
}
//------------------------------------------------------
void TSpectrumTask::SinusRepos( kgp_math::fft& fft, float f, float Fs, size_t N, size_t offset )
{
   for ( size_t i = 0 ; i < N ; i++ )
     {
       float s,c ;

       vdt::fast_sincosf( (i + offset) * vdt::details::VDT_2PI_F * f / Fs  , s,c);
       vec val(c,s);
       // write with reposition
       fft.reposition_write( val , i);

     }
}
//-----------------------------------------------------
void TSpectrumTask::PrintNoteTable()
{
  /*for ( int i = 0 ; i < 108 ;)
    {
      	for ( int j = 0 ; j < 12 ; j++ )
       	{
           rmsg ( "%1\t\t%4.3\t\t\t%s\n", i , HalfTone(i) , note_name_table[i]) ;
           i++ ;
        }
    }*/
}
//-----------------------------------------------------
void TSpectrumTask::PrintBassGuitarTable()
{
  /*rmsg ( "4/5/6 strings guitar freq table\n#\t\tfreq\t\t\tnote name\n");
  for ( int i = 0 ; i < 6 ; i++)
    {
           rmsg ( "%1\t\t%4.3\t\t%s\n", 6-i , HalfTone(guitar_bass_index_table[i]) , note_name_table[guitar_bass_index_table[i]]) ;
    }*/
}


