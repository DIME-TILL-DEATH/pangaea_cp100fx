#include "eq_bitmap.h"

#include "bitmaps.h"
#include "icons_bitmap.h"

#include "filter.h"
#include "eepr.h"
#include "modules.h"
#include "preset.h"

const uint8_t eq_bit[] = {0x00,0x80,0xff,0x7f,
                          0x00,0x80,0xff,0x3f,
                          0x00,0x80,0xff,0x1f,
                          0x00,0x80,0xff,0x0f,
                          0x00,0x80,0xff,0x07,
                          0x00,0x80,0xff,0x03,
                          0x00,0x80,0xff,0x01,
                          0x00,0x80,0xff,0x00,
                          0x00,0x80,0x7f,0x00,
                          0x00,0x80,0x3f,0x00,
                          0x00,0x80,0x1f,0x00,
                          0x00,0x80,0x0f,0x00,
                          0x00,0x80,0x07,0x00,
                          0x00,0x80,0x03,0x00,
                          0x00,0x80,0x01,0x00,

                          0x00,0x80,0x00,0x00,

                          0x00,0xc0,0x00,0x00,
                          0x00,0xe0,0x00,0x00,
                          0x00,0xf0,0x00,0x00,
                          0x00,0xf8,0x00,0x00,
                          0x00,0xfc,0x00,0x00,
                          0x00,0xfe,0x00,0x00,
                          0x00,0xff,0x00,0x00,
                          0x80,0xff,0x00,0x00,
                          0xc0,0xff,0x00,0x00,
                          0xe0,0xff,0x00,0x00,
                          0xf0,0xff,0x00,0x00,
                          0xf8,0xff,0x00,0x00,
                          0xfc,0xff,0x00,0x00,
                          0xfe,0xff,0x00,0x00,
                          0xff,0xff,0x00,0x00,
                          0,0,0,0
};
const uint8_t _15[]="15";
const uint8_t __15[]="-15";
const uint8_t db[]="db";

void lo_hi_par(float num , uint8_t type)
{
	uint8_t ewq = 35;
	char q_sym[20];
	ksprintf( q_sym ,"%d", (uint16_t)num );
	Arsys_clean_(23 + ewq, type , 5);
	if(!type)Arsys_line(29 + ewq, 0 , (uint8_t*)q_sym , 0);
	else Arsys_line(23 + ewq, 1 , (uint8_t*)q_sym , 0);
}

uint8_t eq_ind(uint8_t col , uint8_t pag , uint32_t val , uint8_t cur )
{
  uint32_t data;
  for(uint8_t i = 0 ; i < 11 ; i++)
    {
      for(uint8_t j = 0 ; j < 4 ; j++)
        {
          data = eq_bit[val * 4 + j];
          LCD_SetColumnAddress(col + i);
          LCD_SetPageAddress(pag + j);
          if(cur == 1){
          if((i != 2) && (i != 5) && (i != 8) && (i != 11))
            {
              if(j < 2)
              {
                  if((j & 1) == 0)data |= 0x21;
                  else data |= 0x4;
              }
              else {
                  if((j & 1) == 0)data |= 0x10;
                  else data |= 0x42;
                }
           }
          }
          LCD_WriteData(data);
        }
    }
  return col + 14;
}

void eq_init(void)
{
	LCD_Clear();
    Arsys_line(6,0,(uint8_t*)_15,0);
    Arsys_line(0,3,(uint8_t*)__15,0);
    Arsys_sym_up(9,1,48,0);
    Arsys_sym_down(9,2,48,0);
    LCD_SetPageAddress(1);
    LCD_SetColumnAddress(20);
    LCD_WriteData(0x80, 63);

    for(uint8_t i = 0 ; i < 4 ; i++)
      {
        LCD_SetPageAddress(i);
        for(uint8_t j = 0 ; j < 4 ; j++)
          {
            LCD_SetColumnAddress(20 + j);
            if(i < 2)
              {
                if(j > 1)
                  {
                    if((i & 1) == 0)LCD_WriteData(0x21);
                    else LCD_WriteData(0x84);
                  }
                else {
                    if((i & 1) == 0)LCD_WriteData(0x1);
                    else LCD_WriteData(0x80);
                }
              }
            else {
                if(j > 1)
                  {
                    if((i & 1) == 0)LCD_WriteData(0x10);
                    else LCD_WriteData(0x42);
                  }
                else {
                    if((i & 1) == 0)LCD_WriteData(0x0);
                    else LCD_WriteData(0x40);
                }
            }
            if(j == 3)
              {
                if(i < 3)LCD_WriteData(0xff);
                else LCD_WriteData(0x7f);
              }
          }
      }
   uint8_t curs = 27;
   for(uint8_t i = 0 ; i < 5 ; i++)curs = eq_ind(curs , 0 , currentPreset.modulesBuf[EQ_G0 + i],0);
   Arsys_sym_up(96,1,100,0);
   Arsys_sym_down(96,2,100,0);
   Arsys_sym_up(102,1,66,0);
   Arsys_sym_down(102,2,66,0);
   icon_print(ICON_EQ, STRELKA_DOWN);
}

const uint16_t fr_l[5] = {120,360,800,2000,6000};
void eq_par(uint8_t col , uint8_t pag , int16_t num , uint8_t type , uint8_t band)
{
	char q_sym[8];
	Arsys_clean_(col ,pag , 7);
	if(!type)
	{
		switch (band){
		case 0:case 1:num = fr_l[band] + num;break;
		case 2:num = fr_l[band] + num*2;break;
		case 3:num = fr_l[band] + num*10;break;
		case 4:num = fr_l[band] + num*50;break;
		case 5: num = num*(980.0/127.0)+20.0;break; //HPF
		case 6: num = powf(127-num, 2.0)*(19000.0/powf(127.0, 2.0))+1000.0;break; //LPF
		}
		ksprintf(q_sym , "%dHz" , num);
	}
	else {
		float a;
		if(num <= 30)
		{
			a = num * 0.01f + 0.701f;
			ksprintf(q_sym , "%2f" , a);
			if(num == 30)ksprintf(q_sym , "%1f" , a);
		}
		else {
			a = (num - 20) * 0.1f + 0.001f;
			ksprintf(q_sym , "%1f" , a);
		}
	}
	Arsys_line(col, pag, (uint8_t*)q_sym , 0);
}

void eq_response()
{
	// Response
	uint8_t graphCenter = 16;
	uint8_t prevY = graphCenter;

	for(uint8_t i = 0; i<Filter::EqPointsNumber; i++)
	{
		LCD_SetColumnAddress(9*6 + i);
		uint8_t yPosition = graphCenter - std::round(Filter::eqResponsePoints[i]);
		if(std::abs(prevY - yPosition) > 1)
		{
			int8_t sign = (prevY - yPosition) > 0 ? -1 : 1;

			uint8_t pageData[8] = {0, 0, 0, 0, 0, 0, 0, 0};

			for(uint8_t a=0; a<std::abs(prevY - yPosition); a++)
			{
				if((prevY+a*sign)/8 > 3 || (prevY+a*sign)/8<0) continue;

				pageData[(prevY+a*sign)/8] |= 1 << (prevY+a*sign)%8;
			}

			for(uint8_t a=0; a<4; a++)
			{
				if(pageData[a] == 0) continue;

				LCD_SetColumnAddress(9*6 + i);
				LCD_SetPageAddress(a);
				LCD_WriteData(pageData[a]);
			}
		}
		else
		{
			LCD_SetPageAddress(yPosition/8);
			LCD_WriteData(1 << yPosition%8);
		}

		prevY = yPosition;
	}

	// Delimiter
	for(uint8_t i=0; i < 4; i++)
	{
		LCD_SetColumnAddress(9*6);
		LCD_SetPageAddress(i);
		LCD_WriteData(0xFF);
	}
}
