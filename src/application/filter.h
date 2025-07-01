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
	double a0;
	double a1;
	double a2;
	double b0;
	double b1;
	double b2;
}TFilterCoefs;

const uint8_t EqPointsNumber = 150;
extern float freqPoints[];
extern float eqResponsePoints[];

void calcEqResponse();
TFilterCoefs calcFilterCoefs(TFilterType filterType, double freq, double gain, double Q);
double calcFilterResponse(double f, TFilterCoefs coefs);

void Set_filt_LPF(float fCut);
float filt_lp(float in);

}

#endif /*__FILT_H__*/
