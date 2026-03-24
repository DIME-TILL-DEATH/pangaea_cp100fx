#ifndef __TUNER_TASK_H__
#define __TUNER_TASK_H__

#include "appdefs.h"

#include "dsp/fft.h"
#include "math/window.h"

class TTunerTask: public TTask
{
public:
	enum TTunerMode{
		TUNER_GUI,
		TUNER_CONSOLE
	};
	TTunerTask();

	float refFreq;

	void Enable(TTunerMode tunerMode, int16_t samplesToSend = 0);
	void Disable();
	bool IsrRoutine(int32_t inSample);

private:
	size_t guitar_classic_index_table[6];
	size_t guitar_bass_index_table[6];

	kgp_math::fft fft_0;
	kgp_math::fft fft_1;

	TTunerMode mode;
	bool tunerEnabled;
	char const *noteName;
	int8_t cents;
	uint8_t arrowPosition;
	uint8_t tunerDelayCounter{0};
	uint32_t peakSample{0};
	int16_t samplesCount{0};

	void Code() override;

	void ToneMeter();
	void SpectrumBuffsUpdate(float u);

	float HalfTone(float index);
	void Tone2NoteAndDiff(float tone, size_t &note, float &freq_diff);
	void Sinus(float Fin, float Fs, size_t N, vec *in, size_t offset = 0);
	void SinusRepos(kgp_math::fft &fft, float f, float Fs, size_t N, size_t offset = 0);
};
extern TTunerTask *TunerTask;



#endif /*__TUNER_TASK_H__*/
