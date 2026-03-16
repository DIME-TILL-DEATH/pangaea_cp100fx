#ifndef _COMPRESSOR_H_
#define _COMPRESSOR_H_

#include "appdefs.h"

float COMP_Out(float efxout);

void COMP_Init(void);
void COMP_ChangePreset(int dgui, int npreset);

void COMP_SetParam(uint32_t val);

#endif /* _COMPRESSOR_H_ */
