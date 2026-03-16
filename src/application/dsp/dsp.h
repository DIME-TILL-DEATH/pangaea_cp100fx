#ifndef SRC_APPLICATION_DSP_DSP_H_
#define SRC_APPLICATION_DSP_DSP_H_

#include "appdefs.h"

#include "gate.h"
#include "compressor.h"
#include "filter.h"

#include "vdt/vdt.h"

#define MIN_GAIN  0.00001f        // -100dB  This will help prevent evaluation of denormal numbers
#define LOG_2  0.693147f
#define LOG_10 2.302585f
#define samp_rate 46875.0f

inline float dB2rap(float dB)
{
	return fast_expf((dB) * LOG_10 / 20.0f);
}
inline float rap2dB(float rap)
{
	return 20.0f * fast_logf(rap) / LOG_10;
}

#endif /* SRC_APPLICATION_DSP_DSP_H_ */
