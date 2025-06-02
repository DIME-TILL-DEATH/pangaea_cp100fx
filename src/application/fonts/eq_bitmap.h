
extern const uint8_t eq_list [][4];
extern volatile uint8_t eq_num;

uint8_t eq_ind(uint8_t col , uint8_t pag , uint32_t val , uint8_t cur );
void eq_init(void);
void lo_hi_par(float num , uint8_t type);
void eq_par(uint8_t col , uint8_t pag , int16_t num , uint8_t type , uint8_t band);

