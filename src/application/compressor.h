#ifndef SRC_APPLICATION_COMPRESSOR_H_
#define SRC_APPLICATION_COMPRESSOR_H_

#include "appdefs.h"

float COMPR_Out(float efxout);

void COMPR_Init(void);
void COMPR_ChangePreset(int dgui, int npreset);

float GATE_out(float efxout);
void GATE_ChangePreset(void);

#endif /* SRC_APPLICATION_COMPRESSOR_H_ */
