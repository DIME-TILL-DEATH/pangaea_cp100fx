#include "cs.h"

#include "appdefs.h"
#include "cc.h"
#include "init.h"
#include "allFonts.h"
#include "fs.h"
#include "eepr.h"
#include "display.h"
#include "enc.h"
#include "BF706_send.h"

#include "mainmenu.h"
#include "usbmenu.h"

#include "preset.h"

#include "compressor.h"

uint8_t tempo_fl = 0;
TCSTask* CSTask ;

void oled023_1_disp_init(void);
void led_disp_write(void);
extern uint8_t led_buf[];

AbstractMenu* currentMenu;
MainMenu* mainMenu;
UsbMenu* usbMenu;

TCSTask::TCSTask() : TTask()
{
	queue = new TQueue(4, sizeof(TResponse));
}

TCSTask::~TCSTask()
{
	delete queue;
}

void TCSTask::Code()
{
	Delay(100);
	sem = new TSemaphore (TSemaphore::fstCounting, 4 , 0 ) ;
	GetCpuClock();

	GATE_ChangePreset();
	COMPR_Init();
	COMPR_ChangePreset(0,0);

	CSTask->DisplayAccess(false);

    currentPresetNumber = preselectedPresetNumber = sys_para[LAST_PRESET_NUM];

    DisplayTask->StartScreen(1);

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
  	DisplayTask->SetVolIndicator(TDisplayTask::VOL_INDICATOR_OFF, DSP_INDICATOR_OUT);

  	DSP_GuiSendParameter(DSP_ADDRESS_SPDIF, sys_para[SPDIF_OUT_TYPE], 0);
  	DSP_GuiSendParameter(DSP_ADDRESS_GLOBAL_TEMPO, sys_para[TAP_TYPE], sys_para[TAP_HIGH]);
	tempo_fl = 1;
	DSP_GuiSendParameter(DSP_ADDRESS_CAB_CONFIG, sys_para[CAB_SIM_CONFIG], 0); // left cab bypass

	if(!sys_para[PHONES_VOLUME]) sys_para[PHONES_VOLUME] = 127;
  	DisplayTask->Pot_Write();

  	DSP_GuiSendParameter(DSP_ADDRESS_MASTER, sys_para[MASTER_VOLUME], 0);

	if(!sys_para[MASTER_EQ_FREQ_LO] && !sys_para[MASTER_EQ_FREQ_HI])
	{
		mstEqMidFreq = 1000;
	}
	else
	{
		mstEqMidFreq = sys_para[MASTER_EQ_FREQ_LO] << 8;
		mstEqMidFreq |= sys_para[MASTER_EQ_FREQ_HI];
	}

	for(uint8_t i = 0 ; i < 4 ; i++) gui_send(25,i);

	gui_send(18, 14 | (sys_para[120] << 8)); // master eq
	tun_del_val = (127 - sys_para[TUNER_SPEED]) * (90.0f/127.0f) + 10.0f;
	Delay(500);
	prog_ch();
	eepr_read_imya(preselectedPresetNumber);
	DisplayTask->Main_scr();
	DisplayTask->Prog_ind(preselectedPresetNumber);
    for(uint8_t i = 0 ; i < 3 ; i++)
    {
    	contr_kn[i] = currentPreset.modules.rawData[fo1 + i];
    	contr_kn1[i] = currentPreset.modules.rawData[fo11 + i];
    }
	if((sys_para[FSW1_PRESS_TYPE] == 1) || ((sys_para[FSW1_HOLD_TYPE] == 1) && sys_para[FSW1_MODE]))DisplayTask->IndFoot(0,contr_kn[0]);
	if((sys_para[FSW2_PRESS_TYPE] == 1) || ((sys_para[FSW2_HOLD_TYPE] == 1) && sys_para[FSW2_MODE]))DisplayTask->IndFoot(1,contr_kn[1]);
	if((sys_para[FSW3_PRESS_TYPE] == 1) || ((sys_para[FSW3_HOLD_TYPE] == 1) && sys_para[FSW3_MODE]))DisplayTask->IndFoot(2,contr_kn[2]);
	send_codec(0xa301);
   	ENCTask->SetEnc(1);

   	sem->Take(portMAX_DELAY);
	if (DisplayAccess())
	{
		mainMenu = new MainMenu();
		usbMenu = new UsbMenu();

		currentMenu = mainMenu;
		mainMenu->show();
	}

   	while(1)
   	{
		sem->Take(portMAX_DELAY);
		if(DisplayAccess())
		{
			gui();
		}
   	}
}

void TCSTask::Restart(void)
{
	currentMenu = mainMenu;
	clean_flag();
	//DisplayTask->Menu_init(1,0,prog_data,prog1);
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
	  SpectrumBuffsUpdate( COMPR_Out(in) );
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

	if((ind_poin++ == 4000) && (!tuner_use)
//			&& ((current_menu == MENU_VOLUME) || (current_menu == MENU_CABSIM)
//			|| (current_menu == MENU_PA) || (current_menu == MENU_PREAMP)
//			|| (current_menu == MENU_ATTENUATOR))
			)
	{
	  DisplayTask->VolIndicator();
	  ind_poin = 0;
	}

  dac_data[dma_ht_fl].left.sample  = ror16(cr);
  dac_data[dma_ht_fl].right.sample = ror16(cr);
  //GPIO_ResetBits(GPIOC,GPIO_Pin_5);
}





