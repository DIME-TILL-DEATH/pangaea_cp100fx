#ifndef SRC_APPLICATION_COMPRESSOR_H_
#define SRC_APPLICATION_COMPRESSOR_H_

#include "appdefs.h"

float COMPR_Out(float efxout);

void COMPR_Init(void);
void COMPR_ChangePreset(int dgui, int npreset);

float GATE_out(float efxout);
void GATE_ChangePreset(void);

void comp_para(uint32_t val);
void gate_par(uint32_t val);

#endif /* SRC_APPLICATION_COMPRESSOR_H_ */
