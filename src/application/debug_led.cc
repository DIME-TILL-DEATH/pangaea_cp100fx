#include "debug_led.h"

//--------------------------------------------------
typedef struct
{
  TimerHandle_t xTimer ;
  TickType_t OnTicks  ;
  TickType_t OffTicks ;
  GPIO_TypeDef* port ;
  uint32_t      pin ;
  TimerCallbackFunction_t fnc ;
} TLedConfig ;

TLedConfig led_conf[1]  ;

void LedTimerCallback( TimerHandle_t pxTimer )
{
  TLedConfig* conf = ( TLedConfig* ) pvTimerGetTimerID( pxTimer );
  if ( conf->port->ODR & conf->pin )
    {
      conf->port->BSRRH = conf->pin ;
      xTimerChangePeriod( pxTimer, conf->OffTicks, 0 ) ;
    }
  else
    {
      conf->port->BSRRL = conf->pin ;
      xTimerChangePeriod( pxTimer, conf->OnTicks, 0 ) ;
    }
}

void DebugLedConfig()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP ;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz ;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    led_conf[0].OffTicks=500 ;
    led_conf[0].OnTicks = 500 ;
    led_conf[0].fnc = LedTimerCallback ;
    led_conf[0].pin = GPIO_Pin_14 ;
    led_conf[0].port = GPIOB ;
    led_conf[0].xTimer = NULL ;

    TimerHandle_t timer = xTimerCreate  ( ( const char * const) "Timer", led_conf[0].OnTicks , pdTRUE, ( void * ) &led_conf[0] , led_conf[0].fnc  );
    xTimerStart( timer, 0 ) ;
}

//-------------------------------------------------------------------------------
