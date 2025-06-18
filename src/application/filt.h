#ifndef __FILT_H__
#define __FILT_H__
#include "appdefs.h"

extern float fil_lp_in[];
extern float fil_lp_out[];
extern float fil_a0;
extern float fil_a1;
extern float fil_b1;

inline void Set_filt_LPF(float fCut)
{
	float w = 2.0*8000.0;
	float Norm;
	fCut *= 2.0*3.14159265359f;
	Norm = 1.0/(fCut+w);
	fil_b1 = (w-fCut)*Norm;
	fil_a0 = fil_a1 = fCut*Norm;
}
inline float filt_lp(float in)
{
	fil_lp_in[0] = in;
	fil_lp_out[0] = fil_lp_in[0]*fil_a0+fil_lp_in[1]*fil_a1+fil_lp_out[1]*fil_b1;
	fil_lp_in[1] = fil_lp_in[0];
	fil_lp_out[1] = fil_lp_out[0];
	return fil_lp_out[0];
}
#endif /*__FILT_H__*/
