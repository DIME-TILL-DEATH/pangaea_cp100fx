#ifndef SHARC_H_
#define SHARC_H_

#include "appdefs.h"

typedef enum{
	SPI_MASTER = 0,
	SPI_SLAVE
}TSharcSpiMode;

void SHARC_spi_init(TSharcSpiMode mode);
void SHARC_startup_load();
void SHARC_wait_for_ready();
void SHARC_send_data(uint16_t data);
void SHARC_dma_send_data(uint16_t data, uint16_t size);


#endif /* SHARC_H_ */
