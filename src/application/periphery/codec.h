#ifndef CODEC_H_
#define CODEC_H_

#include "appdefs.h"

typedef union
{
	uint32_t sample;
	struct
	{
		uint16_t sampleL;
		uint16_t sampleH;
	};
} ad_channel_t;

typedef union
{
	uint32_t sample;
	struct
	{
		uint16_t sampleL;
		uint16_t sampleH;
	};
} da_channel_t;

typedef union
{
	int64_t val;
	struct
	{
		ad_channel_t left;
		ad_channel_t right;
	};
} ad_data_t;

typedef union
{
	int64_t val;
	struct
	{
		da_channel_t left;
		da_channel_t right;
	};
} da_data_t;

void CODEC_start();
void CODEC_send(uint16_t data);

#endif /* CODEC_H_ */
