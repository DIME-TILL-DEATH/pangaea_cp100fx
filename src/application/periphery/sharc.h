#ifndef SHARC_H_
#define SHARC_H_

#include "appdefs.h"

typedef enum{
	SPI_MASTER = 0,
	SPI_SLAVE
}TSharcSpiMode;

void SHARC_SpiInit(TSharcSpiMode mode);
void SHARC_StartupLoad();
void SHARC_WaitForReady();
void SHARC_SendData(uint16_t data);

#endif /* SHARC_H_ */
