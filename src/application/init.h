#ifndef __INIT_H__
#define __INIT_H__

extern uint32_t sram_point;
extern uint8_t tim5_fl;
extern uint8_t usart_buf[4];

typedef union
{
	uint32_t sample;
	struct
	{
		uint16_t sampleL;
		uint16_t sampleH;
	};
} ad_chanal_t;
typedef union
{
	uint32_t sample;
	struct
	{
		uint16_t sampleL;
		uint16_t sampleH;
	};
} da_chanal_t;
typedef union
{
	int64_t val;
	struct
	{
		ad_chanal_t left;
		ad_chanal_t right;
	};
} ad_data_t;
typedef union
{
	int64_t val;
	struct
	{
		da_chanal_t left;
		da_chanal_t right;
	};
} da_data_t;

ad_data_t& init_get_adc_data();
da_data_t& init_get_dac_data();

void send_codec(uint16_t data);
void init(void);
void adc_init(uint8_t stat);
void adc_calib(void);
void adc_pin_init(void);

extern const uint16_t bpm_time[];

inline void __attribute__ ((always_inline)) dela(uint32_t i)
{
	for(uint32_t ii = 0; ii<i; ii++)
		NOP();
}
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

inline uint8_t drebezg(uint32_t line)
{
	uint8_t sss;
	TIM_Cmd(TIM9, DISABLE);
	if((EXTI->FTSR&line)!=0)
	{
		if(TIM_GetFlagStatus(TIM9, TIM_FLAG_Update)==1)
		{
			sss = 1;
			EXTI->FTSR &= ~line;
			EXTI->RTSR |= line;
		}
		else
			sss = 0;
	}
	else
	{
		if(TIM_GetFlagStatus(TIM9, TIM_FLAG_Update)==1)
		{
			EXTI->RTSR &= ~line;
			EXTI->FTSR |= line;
			sss = 2;
		}
		else
			sss = 0;
	}
	TIM_SetCounter(TIM9, 0);
	TIM_ClearFlag(TIM9, TIM_FLAG_Update);
	TIM_Cmd(TIM9, ENABLE);
	return sss;
}

void start_usb(uint8_t type);

#endif /*__INIT_H__*/
