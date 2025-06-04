#include "appdefs.h"
#include "cs.h"
#include "cc.h"
#include "init.h"
#include "filt.h"
#include "fonts/allFonts.h"
#include "fs.h"
#include "eepr.h"
#include "display.h"
//#include "gu.h"
#include "enc.h"
#include "BF706_send.h"

float m_vol = 1.0;
float p_vol = 1.0;
uint8_t tempo_fl = 0;
TCSTask* CSTask ;

void oled023_1_disp_init(void);
void led_disp_write(void);
extern uint8_t led_buf[];
volatile uint16_t mas_eq_fr;




TCSTask::TCSTask () : TTask()
 {
             //DispalyAccess = true ;
 }

volatile uint32_t saa;
volatile FRESULT res;
char mes[512]="";
UINT d ;

#include "diskio.h"
#include "sdcard_diskio.h"

void TCSTask::Code()
{
	Delay(100);
	sem = new TSemaphore (TSemaphore::fstCounting, 4 , 0 ) ;
	saa = GetCpuClock();

	Gate_Change_Preset ();
	Compressor_init();
	Compressor_Change_Preset(0,0);

	CSTask->DisplayAccess(false);

    prog = prog1 = sys_para[31];

    DisplayTask->Start_screen(1);

	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);
	TIM_Cmd(TIM4,ENABLE);

	while(!ind_en);

  	USART_Cmd(USART1,ENABLE);

    if(sys_para[3] & 0x80)
    {
 	   adc_init(1);
    }

   	CSTask->DisplayAccess(true);

  	gui_send(14,0); // global cab on off
  	gui_send(15,0); // indicator source
  	gui_send(26,sys_para[spdif_t]);
  	gui_send(33,sys_para[tap_typ] | (sys_para[tap_hi] << 8)); //global temp
	tempo_fl = 1;
	gui_send(28,sys_para[2]); // left cab bypass
	if(!sys_para[125])sys_para[125] = 127;
  	DisplayTask->Pot_Write();
	gui_send(1,sys_para[126]); //master volum
	if(!sys_para[508] && !sys_para[509])
	{
		sys_para[508] = 2;
		sys_para[509] = 0x6a;
	}
	mas_eq_fr = sys_para[508] << 8;
	mas_eq_fr |= sys_para[509];
	uint16_t f = mas_eq_fr;
	f *= mas_eq_fr * (20.0f/20000.0f) + 1;
	sys_para[510] = f >> 8;
	sys_para[511] = f;
	for(uint8_t i = 0 ; i < 4 ; i++)gui_send(25,i);
	gui_send(18, 14 | (sys_para[120] << 8)); // master eq
	tun_del_val = (127 - sys_para[speed_tun]) * (90.0f/127.0f) + 10.0f;
	Delay(500);
	prog_ch();
	eepr_read_imya(prog1);
	DisplayTask->Main_scr();
	DisplayTask->Prog_ind(prog1);
    for(uint8_t i = 0 ; i < 3 ; i++)
    {
    	contr_kn[i] = prog_data[fo1 + i];
    	contr_kn1[i] = prog_data[fo11 + i];
    }
	if((sys_para[fs1] == 1) || ((sys_para[fs11] == 1) && sys_para[fsm1]))DisplayTask->IndFoot(0,contr_kn[0]);
	if((sys_para[fs2] == 1) || ((sys_para[fs21] == 1) && sys_para[fsm2]))DisplayTask->IndFoot(1,contr_kn[1]);
	if((sys_para[fs3] == 1) || ((sys_para[fs31] == 1) && sys_para[fsm3]))DisplayTask->IndFoot(2,contr_kn[2]);
	send_codec(0xa301);
   	ENCTask->SetEnc(1);

  while(1)
     {
	  sem->Take(portMAX_DELAY);
	  if (DisplayAccess())
	  {
		  gui();
	  }

     }
}
//------------------------------------------------------------------------------------
extern ad_data_t adc_data[];
extern da_data_t dac_data[];
int32_t ccl;
int32_t ccr;
volatile uint32_t cl;
volatile uint32_t cr;
uint8_t dma_ht_fl;
uint8_t tun_del;
uint8_t tun_del_val;

#include "spectrum.h"

extern "C" void DMA1_Stream2_IRQHandler()
{
  if (tap_temp < 131071)tap_temp += 1;
  if(DMA_GetITStatus(DMA1_Stream2, DMA_IT_HTIF2))
  {
	  DMA_ClearITPendingBit ( DMA1_Stream2 , DMA_IT_HTIF2);
	  dma_ht_fl = 0;
  }
  else {
	  DMA_ClearITPendingBit ( DMA1_Stream2 , DMA_IT_TCIF2);
	  dma_ht_fl = 1;
  }
  //GPIO_SetBits(GPIOC,GPIO_Pin_5);

  cr = ror16(adc_data[dma_ht_fl].left.sample);
  cl = ror16(adc_data[dma_ht_fl].right.sample);
  ccl = cl;
  ccl = ccl >> 8;
  ccr = cr;
  ccr = ccr >> 8;
//-------------------------------------------------------
  float in = ccl * 0.000000119f;
//--------------------Tuner------------------------------
  if(tuner_use)
  {
	  SpectrumBuffsUpdate( compr_out(in) );
	  if(tun_del > tun_del_val)
	  {
	    tun_del = 0;
	    inline void strel_tun(void);
	    strel_tun();
	  }
	  tun_del++;
  }
  else GPIOB->BSRRH |= GPIO_Pin_11;

//-------------------------------------------------------

  ind_out_l[0] = abs(ccl);
  if(ind_out_l[0] > ind_out_l[1])ind_out_l[1] = ind_out_l[0];

  if((ind_poin++ == 4000) && ((condish == volume) || (condish == cabinet_sim)  ||
     (condish == power_amplif) || (condish == pre_menu) || (condish == att_menu)) && (!tuner_use))
    {
      DisplayTask->Indicator();
      ind_poin = 0;
    }

  dac_data[dma_ht_fl].left.sample  = ror16(cr);
  dac_data[dma_ht_fl].right.sample = ror16(cr);
  //GPIO_ResetBits(GPIOC,GPIO_Pin_5);
}





