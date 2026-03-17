#include "periphery.h"

void HW_delay(uint64_t delayTime)
{
	for(uint64_t i=0; i < delayTime; i++) NOP();
}
