// https://github.com/billthefarmer/tuner/tree/master/src/org/billthefarmer/tuner
// http://www.codeproject.com/Articles/32172/FFT-Guitar-Tuner
// http://habrahabr.ru/post/247385/
// https://code.google.com/p/ctuner/source/browse/trunk/windows/Tuner.c
#include <tuner_task.h>
#include "codec.h"
#include "led.h"

#include "dsp.h"
#include "filter.h"
#include "system.h"

#include "rt_counter.h"
#include "int2str.h"

#include "display_task.h"
#include "sharc_task.h"
#include "console_task.h"

static const char *note_name_table[] ={
	" C ", "C# ", " D ", "D# ", " E ", " F ", "F# ", " G ", "G# ", " A ", "A# ", " B ",  // octave X0
	" C ", "C# ", " D ", "D# ", " E ", " F ", "F# ", " G ", "G# ", " A ", "A# ", " B ",  // octave X1
	" C ", "C# ", " D ", "D# ", " E ", " F ", "F# ", " G ", "G# ", " A ", "A# ", " B ",  // octave X2
	" C ", "C# ", " D ", "D# ", " E ", " F ", "F# ", " G ", "G# ", " A ", "A# ", " B ",  // octave X3
	" C ", "C# ", " D ", "D# ", " E ", " F ", "F# ", " G ", "G# ", " A ", "A# ", " B ",  // octave X4
	" C ", "C# ", " D ", "D# ", " E ", " F ", "F# ", " G ", "G# ", " A ", "A# ", " B ", //octave 3/X
	" C ", "C# ", " D ", "D# ", " E ", " F ", "F# ", " G ", "G# ", " A ", "A# ", " B ", //octave 4/X
	" C ", "C# ", " D ", "D# ", " E ", " F ", "F# ", " G ", "G# ", " A ", "A# ", " B ", //octave 5/X
};


TTunerTask *TunerTask = nullptr;

vec __CCM_BSS__ Wfwd[32];

TTunerTask::TTunerTask()
{
	guitar_classic_index_table[0] = 28;  // 28  0082.406 Hz  Big/E
	guitar_classic_index_table[1] = 33;  // 33  0110.000 Hz  Big/A
	guitar_classic_index_table[2] = 38;  // 38  0146.832 Hz  Small/D
	guitar_classic_index_table[3] = 43;  // 43  0196.000 Hz  Small/G
	guitar_classic_index_table[4] = 47;  // 47  0246.941 Hz  Small/H
	guitar_classic_index_table[5] = 52;  // 52  0329.627 Hz  1.00/E

	guitar_bass_index_table[0] = 11;  // 11  0030.867 Hz  SubContre/B ( 5/6 sring guitar )
	guitar_bass_index_table[1] = 16;  // 16  0041.203 Hz  Contre/E
	guitar_bass_index_table[2] = 21;  // 21  0055.000 Hz  Contre/A
	guitar_bass_index_table[3] = 26;  // 26  0073.416 Hz  Big/D
	guitar_bass_index_table[4] = 31;  // 31  0097.998 Hz  Big/G
	guitar_bass_index_table[5] = 36;  // 36  0130.812 Hz  Small/C     ( 6 sring guitar )

	fft_0.init(in_0, N, Wfwd, 0);
	fft_1.init(in_1, N, Wfwd, 0);

	Filter::Set_filt_LPF(500.0f);

	refFreq = 440.0f;
	tunerEnabled = false;
}

void TTunerTask::Code()
{
	GATE_ChangePreset();
	COMP_Init();
	COMP_ChangePreset(0, 0);

	extern EventGroupHandle_t startEventGroup;
	xEventGroupSync(startEventGroup, EVENT_BIT_SPTASK_STARTED, EVENT_ALL_TASK_STARTED, portMAX_DELAY);

	DMA_ITConfig(DMA1_Stream2, DMA_IT_TC, ENABLE);
	DMA_ITConfig(DMA1_Stream2, DMA_IT_HT, ENABLE);

	while(1)
	{
		Suspend();
		ToneMeter();
	}
}

void TTunerTask::Enable(TTunerMode tunerMode, int16_t samplesToSend)
{
	mode = tunerMode;
	samplesCount = samplesToSend;
	tunerEnabled = true;

	CODEC_Mute();
	SharcTask->setParameter(DSP_ADDRESS_TUNER_PROCESS, 0);	// blinking TAP off
	SharcTask->setParameter(DSP_ADDRESS_IND_SRC, DSP_INDICATOR_IN, 0);
}

void TTunerTask::Disable()
{
	tunerEnabled = false;
	SharcTask->setParameter(DSP_ADDRESS_TUNER_PROCESS, 1);
	CODEC_Unmute();
}

void TTunerTask::ToneMeter()
{
	for(size_t n = 0; n<N; n++)
	{
		float h = kgp_math::window::Hann(n, N);
		in_0[n] *= h;
		in_1[n] *= h;
	}

//  rt_counter_start(counter);
	fft_0.transform();
	fft_1.transform();
//  ffts_x2_uS = rt_counter_stop_us(counter);

	float Kmes;            // измеренный индекс отсчета спектра
	kgp_math::find_tone(Kmes, fft_0, fft_1, n);
	float tone = fft_0.k2freq(Kmes, Fs);

	tone += 0.001f;

	size_t note;
	float freq_diff;
	Tone2NoteAndDiff(tone, note, freq_diff);

	int8_t centsCalculated = 1200 * log2f(tone/HalfTone(note));

	int aa = freq_diff*1000.0f;
	float bb = aa/1000.0f;
	uint8_t arrowCalculated;
	if(freq_diff<0)
	{
		float a = HalfTone(note-1);
		float b = HalfTone(note);
		float c = (b-a)*0.5f;
		arrowCalculated = (uint8_t)((64.0-(fabsf(bb)*(64.0/c))));
	}
	else
	{
		float a = HalfTone(note+1);
		float b = HalfTone(note);
		float c = (a-b)*0.5f;
		arrowCalculated = (uint8_t)(((bb*(64.0/c)))+64.0);
	}

	__disable_irq();
	arrowPosition = arrowCalculated;
	noteName = note_name_table[note];
	cents = centsCalculated;
	__enable_irq();
}

bool TTunerTask::IsrRoutine(int32_t inSample)
{
	float in = inSample * 0.000000119f;
	uint32_t absInVal = abs(inSample);
	if(absInVal > peakSample)
		peakSample = absInVal;

	if(tunerEnabled)
	{
		SpectrumBuffsUpdate(COMP_Out(in));
		if(tunerDelayCounter > System::tun_del_val)
		{
			switch(mode)
			{
				case TUNER_GUI:
				{
					if(peakSample>1500)
					{
						DisplayTask->TunerDraw(arrowPosition, noteName);
						if((arrowPosition > 62) && (arrowPosition < 66)) LED_SetState(TLedType::LED_TAP_GREEN, TLedState::ENABLED);
						else LED_SetState(TLedType::LED_TAP_GREEN, TLedState::DISABLED);
					}
					else
					{
						DisplayTask->TunerDraw(64, nullptr);
						LED_SetState(TLedType::LED_TAP_GREEN, TLedState::DISABLED);
					}
					break;
				}

				case TUNER_CONSOLE:
				{
					if(ConsoleTask)
					{
						if(samplesCount > 0)
						{
							samplesCount--;
							if(peakSample > 1500) ConsoleTask->UnsafePrintF("tn get\r%s\r%d\n", noteName, cents);
							else ConsoleTask->UnsafePrintF("tn get\r-\r0\n");
						}
						else
						{
							Disable();
						}
					}
					break;
				}
			}
			tunerDelayCounter = 0;
			peakSample = 0;
		}

		tunerDelayCounter++;
		return true;
	}
	return false;
}

void TTunerTask::SpectrumBuffsUpdate(float u)
{
	static size_t index = 0;

	if(eSuspended==TTaskUtilities::GetTaskState(TunerTask->GetHandle()))
	{
		u = Filter::filt_lp(u);
		vec val(u, 0);
		if(index<N)
			in_0[index] = vec(u, 0);

		if(index>=n)
			in_1[index-n] = vec(u, 0);

		index++;
		if(index==N+n)
		{
			if(tunerEnabled)
				Resume();
			index = 0;
		}
	}
}

//------------------------------------------------------
float TTunerTask::HalfTone(float index)
{
	return refFreq*vdt::fast_powf(2.0f, (index-57.0f)/12.0f);
}

void TTunerTask::Tone2NoteAndDiff(float tone, size_t &note, float &freq_diff)
{
	note = 0.5f+57+12*vdt::fast_logf(tone/refFreq)/vdt::fast_logf(2.0f);
	//      ^ round trip
	freq_diff = tone-HalfTone(note);
}

void TTunerTask::Sinus(float Fin, float Fs, size_t N, vec *in, size_t offset)
{
	for(size_t i = 0; i<N; i++)
	{
		float s, c;

		vdt::fast_sincosf((Fin/Fs)*(i+offset)*vdt::details::VDT_2PI_F, s, c);
		in[i] = vec(c, s);
	}
}
//------------------------------------------------------
void TTunerTask::SinusRepos(kgp_math::fft &fft, float f, float Fs, size_t N, size_t offset)
{
	for(size_t i = 0; i<N; i++)
	{
		float s, c;

		vdt::fast_sincosf((i+offset)*vdt::details::VDT_2PI_F*f/Fs, s, c);
		vec val(c, s);
		// write with reposition
		fft.reposition_write(val, i);

	}
}

