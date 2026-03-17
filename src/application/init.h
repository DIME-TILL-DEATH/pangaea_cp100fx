#ifndef __INIT_H__
#define __INIT_H__

#include "appdefs.h"

extern uint8_t tun_del_val;

extern uint8_t cab_type;


extern uint32_t sram_point;
//extern uint8_t usart_buf[4];


void send_codec(uint16_t data);
void init(void);
void adc_init(uint8_t stat);
void adc_calib(void);

//inline void __attribute__ ((always_inline)) dela(uint32_t i)
//{
//	for(uint32_t ii = 0; ii<i; ii++)
//		NOP();
//}
static inline void __attribute__ ((always_inline)) bubble_sort(float *p, size_t n)
{
	if(n<2)
		return;

	for(size_t i = 0; i<n; i++)
	{
		for(size_t j = 1; j<(n-i); j++)
		{
			if(p[j-1]>p[j])
			{
				float temp = p[j-1];
				p[j-1] = p[j];
				p[j] = temp;
			}
		}
	}
}
static inline uint32_t __attribute__ ((always_inline)) high_sort(uint32_t *p, size_t n)
{
	uint32_t temp = 0;
	for(uint16_t i = 0; i<n; i++)
	{
		if(temp<p[i])
			temp = p[i];
	}
	return temp;
}



void start_usb(uint8_t type);

#endif /*__INIT_H__*/
