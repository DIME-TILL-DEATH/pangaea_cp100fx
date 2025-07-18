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

#include "system.h"

TCSTask *CSTask;

void oled023_1_disp_init(void);
void led_disp_write(void);
extern uint8_t led_buf[];

AbstractMenu *currentMenu;
MainMenu *mainMenu;
UsbMenu *usbMenu;

TCSTask::TCSTask() :
		TTask()
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
	sem = new TSemaphore(TSemaphore::fstCounting, 4, 0);
	GetCpuClock();

	GATE_ChangePreset();
	COMPR_Init();
	COMPR_ChangePreset(0, 0);

	CSTask->DisplayAccess(false);

	currentPresetNumber = sys_para[System::LAST_PRESET_NUM];

	DisplayTask->StartScreen(1);

	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM4, ENABLE);

	while(!ind_en);

	USART_Cmd(USART1, ENABLE);

	if(sys_para[3] & 0x80)
	{
		adc_init(1);
	}

	CSTask->DisplayAccess(true);

	DSP_GuiSendParameter(DSP_ADDRESS_CAB_DRY_MUTE, sys_para[System::CAB_SIM_DISABLED], 0);
	DisplayTask->SetVolIndicator(TDisplayTask::VOL_INDICATOR_OFF, DSP_INDICATOR_OUT);

	DSP_GuiSendParameter(DSP_ADDRESS_SPDIF, sys_para[System::SPDIF_OUT_TYPE], 0);
	DSP_GuiSendParameter(DSP_ADDRESS_GLOBAL_TEMPO, sys_para[System::TAP_TYPE], sys_para[System::TAP_HIGH]);
	DSP_GuiSendParameter(DSP_ADDRESS_CAB_CONFIG, sys_para[System::CAB_SIM_CONFIG], 0); // left cab bypass

	if(!sys_para[System::PHONES_VOLUME]) sys_para[System::PHONES_VOLUME] = 127;

	DisplayTask->Pot_Write(); // phones, attenuator

	DSP_GuiSendParameter(DSP_ADDRESS_MASTER, sys_para[System::MASTER_VOLUME], 0);

	if(!sys_para[System::MASTER_EQ_FREQ_LO] && !sys_para[System::MASTER_EQ_FREQ_HI])
	{
		mstEqMidFreq = 1000;
	}
	else
	{
		mstEqMidFreq = sys_para[System::MASTER_EQ_FREQ_LO] << 8;
		mstEqMidFreq |= sys_para[System::MASTER_EQ_FREQ_HI];
	}

	DSP_GuiSendParameter(DSP_ADDRESS_EQ, EQ_MASTER_LOW_GAIN_POS, sys_para[System::MASTER_EQ_LOW] + 24);
	DSP_GuiSendParameter(DSP_ADDRESS_EQ, EQ_MASTER_MID_GAIN_POS, sys_para[System::MASTER_EQ_MID] + 24);
	DSP_GuiSendParameter(DSP_ADDRESS_EQ, EQ_MASTER_MID_FREQ_POS, sys_para[System::MASTER_EQ_FREQ_LO]);
	DSP_GuiSendParameter(DSP_ADDRESS_EQ, EQ_MASTER_MID_FREQ_POS, sys_para[System::MASTER_EQ_FREQ_HI]);
	DSP_GuiSendParameter(DSP_ADDRESS_EQ, EQ_MASTER_HIGH_GAIN_POS, sys_para[System::MASTER_EQ_HIGH] + 24);

	DSP_GuiSendParameter(DSP_ADDRESS_MODULES_ENABLE, ENABLE_MASTER_EQ, sys_para[System::MASTER_EQ_ON]);

	tun_del_val = (127 - sys_para[System::TUNER_SPEED]) * (90.0f / 127.0f) + 10.0f;
	Delay(500);
	prog_ch();

	for(uint8_t i = 0; i < 3; i++)
	{
		contr_kn[i] = currentPreset.modules.rawData[fo1 + i];
		contr_kn1[i] = currentPreset.modules.rawData[fo11 + i];
	}

	send_codec(0xa301);
	ENCTask->SetEnc(1);

	sem->Take(portMAX_DELAY);
	if(DisplayAccess())
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
int32_t __CCM_BSS__ ccl;
int32_t __CCM_BSS__ ccr;
volatile uint32_t __CCM_BSS__ cl;
volatile uint32_t __CCM_BSS__ cr;
uint8_t dma_ht_fl;
uint8_t tun_del;
uint8_t tun_del_val;

#include "spectrum.h"

extern "C" void DMA1_Stream2_IRQHandler()
{
	if(tap_temp < 131071) tap_temp += 1;

	if(DMA_GetITStatus(DMA1_Stream2, DMA_IT_HTIF2))
	{
		DMA_ClearITPendingBit(DMA1_Stream2, DMA_IT_HTIF2);
		dma_ht_fl = 0;
	}
	else
	{
		DMA_ClearITPendingBit(DMA1_Stream2, DMA_IT_TCIF2);
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
		SpectrumBuffsUpdate(COMPR_Out(in));
		if(tun_del > tun_del_val)
		{
			tun_del = 0;
			inline void strel_tun(void);
			strel_tun();
		}
		tun_del++;
	}
	else
		GPIOB->BSRRH |= GPIO_Pin_11;

//---------------------Vol indicator----------------------------

	ind_out_l[0] = abs(ccl);
	if(ind_out_l[0] > ind_out_l[1])
		ind_out_l[1] = ind_out_l[0];


	DisplayTask->VolIndicatorTask();


	dac_data[dma_ht_fl].left.sample = ror16(cr);
	dac_data[dma_ht_fl].right.sample = ror16(cr);
}

