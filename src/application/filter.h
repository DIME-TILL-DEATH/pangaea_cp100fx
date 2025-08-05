#ifndef __FILTER_H__
#define __FILTER_H__
#include "appdefs.h"


namespace Filter
{

enum TFilterType
{
	PEAKING = 0,
	LOW_SHELF,
	HIGH_SHELF,
	HIGH_CUT,
	LOW_CUT,
	BYPASS
};

typedef struct
{
	float a0;
	float a1;
	float a2;
	float b0;
	float b1;
	float b2;
}TFilterCoefs;

const uint8_t EqPointsNumber = 75;
extern float freqPoints[];
extern float eqResponsePoints[];

void calcEqPoints();
void calcEqResponse();
TFilterCoefs calcFilterCoefs(TFilterType filterType, float freq, float gain, float Q);
float calcFilterResponse(float f, TFilterCoefs coefs);

void Set_filt_LPF(float fCut);
float filt_lp(float in);

}

#endif /*__FILT_H__*/
