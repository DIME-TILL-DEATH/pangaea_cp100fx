#include "filter.h"

#include "appdefs.h"
#include "eepr.h"

#include "modules.h"

#include <complex>
#include <cmath>
#include "vdt/vdt.h"

#include "preset.h"

float fil_lp_in[2];
float fil_lp_out[2];
float fil_a0;
float fil_a1;
float fil_b1;

using namespace Filter;

double Fdiscr = 48000;

float Filter::freqPoints[EqPointsNumber];
float Filter::eqResponsePoints[EqPointsNumber];

const uint16_t fr_l[5] = {120, 360, 800, 2000, 6000};


void Filter::Set_filt_LPF(float fCut)
{
	float w = 2.0*8000.0;
	float Norm;
	fCut *= 2.0*3.14159265359f;
	Norm = 1.0/(fCut+w);
	fil_b1 = (w-fCut)*Norm;
	fil_a0 = fil_a1 = fCut*Norm;
}

float Filter::filt_lp(float in)
{
	fil_lp_in[0] = in;
	fil_lp_out[0] = fil_lp_in[0]*fil_a0+fil_lp_in[1]*fil_a1+fil_lp_out[1]*fil_b1;
	fil_lp_in[1] = fil_lp_in[0];
	fil_lp_out[1] = fil_lp_out[0];
	return fil_lp_out[0];
}

void Filter::calcEqResponse()
{
	freqPoints[0] = 10;
	double power = log10(freqPoints[0]);
	double powerStep = (log10(Fdiscr/2)-power)/EqPointsNumber;
	for(int i=0; i<EqPointsNumber; i++)
	{
		eqResponsePoints[i] = 0;

		power += powerStep;
		freqPoints[i] = pow(10, power);
	}


	TFilterCoefs bandCoefs;
	for(uint8_t bandNum=0; bandNum<5; bandNum++)
	{
		double f;
		double g;
		double Q;

		switch(bandNum)
		{
			case 0:case 1:f = fr_l[bandNum] + currentPreset.modules.rawData[EQ_F0 + bandNum];break;
			case 2:f = fr_l[bandNum] + currentPreset.modules.rawData[EQ_F0 + bandNum]*2;break;
			case 3:f = fr_l[bandNum] + currentPreset.modules.rawData[EQ_F0 + bandNum]*10;break;
			case 4:f = fr_l[bandNum] + currentPreset.modules.rawData[EQ_F0 + bandNum]*50;break;
		}

		g = currentPreset.modules.rawData[EQ_G0 + bandNum] - 15;

		if(currentPreset.modules.rawData[EQ_Q0 + bandNum] <= 30)
		{
			Q = currentPreset.modules.rawData[EQ_Q0 + bandNum] * 0.01f + 0.701f;
		}
		else {
			Q = (currentPreset.modules.rawData[EQ_Q0 + bandNum] - 20) * 0.1f + 0.001f;
		}

		bandCoefs = Filter::calcFilterCoefs(Filter::TFilterType::PEAKING, f, g, Q);

		for(int i=0; i<EqPointsNumber; i++)
		{
			eqResponsePoints[i] += calcFilterResponse(freqPoints[i], bandCoefs);
		}
	}
}

double Filter::calcFilterResponse(double f, TFilterCoefs coefs)
{
	std::complex<double> z = std::polar((double)1.0, 2*vdt::details::VDT_PI_F*f/Fdiscr);
	std::complex<double> z2 = z*z;
	std::complex<double> h = (coefs.b0*z2 + coefs.b1*z + coefs.b2) / (coefs.a0*z2 + coefs.a1*z + coefs.a2);
	return 20*log10(abs(h));
}

TFilterCoefs Filter::calcFilterCoefs(TFilterType filterType, double freq, double gain, double Q)
{
	TFilterCoefs coefs;

    double A = pow(10, gain/40);
    double w0 = 2 * vdt::details::VDT_PI_F * freq/Fdiscr;
    double a = sin(w0)/(2*Q);

    switch(filterType)
    {
    case TFilterType::LOW_CUT:
    {
        coefs.b0 = (1 + cos(w0))/2;
        coefs.b1 = -(1 + cos(w0));
        coefs.b2 = coefs.b0;

        coefs.a0 = 1 + a;
        coefs.a1 = -2*cos(w0);
        coefs.a2 = 1 - a;

        break;
    }
    case TFilterType::LOW_SHELF:
    {
        coefs.b0 = A*((A+1) - (A-1)*cos(w0) + 2*vdt::details::sqrt(A)*a);
        coefs.b1 = 2*A*((A-1) - (A+1)*cos(w0));
        coefs.b2 = A*((A+1) - (A-1)*cos(w0) - 2*vdt::details::sqrt(A)*a);

        coefs.a0 = (A+1) + (A-1)*cos(w0) + 2*vdt::details::sqrt(A)*a;
        coefs.a1 = -2*((A-1) + (A+1)*cos(w0));
        coefs.a2 = (A+1) + (A-1)*cos(w0) - 2*vdt::details::sqrt(A)*a;
        break;
    }
    case TFilterType::PEAKING:
    {
        coefs.b0 = 1 + a*A;
        coefs.b1 = -2*cos(w0);
        coefs.b2 = 1 - a*A;

        coefs.a0 = 1 + a/A;
        coefs.a1 = -2*cos(w0);
        coefs.a2 = 1 - a/A;
        break;
    }
    case TFilterType::HIGH_SHELF:
    {
        coefs.b0 = A*((A+1) + (A-1)*cos(w0) + 2*vdt::details::sqrt(A)*a);
        coefs.b1 = -2*A*((A-1) + (A+1)*cos(w0));
        coefs.b2 = A*((A+1) + (A-1)*cos(w0) - 2*vdt::details::sqrt(A)*a);

        coefs.a0 = (A+1) - (A-1)*cos(w0) + 2*vdt::details::sqrt(A)*a;
        coefs.a1 = 2*((A-1) - (A+1)*cos(w0));
        coefs.a2 = (A+1) - (A-1)*cos(w0) - 2*vdt::details::sqrt(A)*a;
        break;
    }
    case TFilterType::HIGH_CUT:
    {
        coefs.b0 = (1 - cos(w0))/2;
        coefs.b1 = 1 - cos(w0);
        coefs.b2 = coefs.b0;

        coefs.a0 = 1 + a;
        coefs.a1 = -2*cos(w0);
        coefs.a2 = 1 - a;
        break;
    }
    case TFilterType::BYPASS:
        coefs.b0 = 1;
        coefs.b1 = 1;
        coefs.b2 = 1;

        coefs.a0 = 1;
        coefs.a1 = 1;
        coefs.a2 = 1;
        break;
    }

    return coefs;
}
