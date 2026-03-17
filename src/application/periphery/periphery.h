#ifndef PERIPHERY_H_
#define PERIPHERY_H_

#include "appdefs.h"

#define NVIC_PRIORITY_MIDI_UART 0
#define NVIC_PRIORITY_ENCODER_READ 1
#define NVIC_PRIORITY_KEYS_READ 2

// Subpriority group 4
#define NVIC_PRIORITY_CODEC_I2S 2

//inline void __attribute__ ((always_inline))
void HW_delay(uint64_t delayTime);

#endif /* PERIPHERY_H_ */
