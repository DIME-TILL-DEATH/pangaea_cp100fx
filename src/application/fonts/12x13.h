#include "ER_OLEDM023-1B.h"
#include "appdefs.h"

uint32_t t12x13_sym(uint8_t col , uint8_t pag , uint16_t sym , uint8_t curs);
void t12x13_line_name(uint32_t col , uint8_t pag , uint8_t* adr );
void t12x13_line(uint32_t col , uint8_t pag , uint8_t* adr , uint8_t curs);
void t12x13_clear(uint8_t col , uint8_t pag , uint8_t size);
