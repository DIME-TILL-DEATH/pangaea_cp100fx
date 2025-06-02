// ���� ������������ ���������
// ��������� ����������� �����
// ����� ���������� ������� �� � ���������� ����� � ��.
// �������� ����������� ���������� ������

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PLATFORM_CONFIG_H__
#define __PLATFORM_CONFIG_H__

/* Includes ------------------------------------------------------------------*/

                #define RT_COUNTER_TIMirq  TIM7
                #define RT_COUNTER_TIMfree TIM2
                #define RT_COUNTER_TIM_IRQ_HANDLER TIM7_IRQHandler
                #define RT_COUNTER_RCC_APB1Periph_TIMirq RCC_APB1Periph_TIM7
                #define RT_COUNTER_RCC_APB1Periph_TIMfree RCC_APB1Periph_TIM2
                #define RT_COUNTER_TIMirq_IRQn TIM7_IRQn
                #define RT_COUNTER_TIM_DEBUG_STOP  DBGMCU_TIM7_STOP | DBGMCU_TIM7_STOP
                #define RT_COUNTER_TIMirq_CLOCK (0.5f*GetCpuClock())
                #define RT_COUNTER_TIMfree_CLOCK (0.5f*GetCpuClock())

                #define SD_DETECT_PIN                    GPIO_Pin_8
                #define SD_DETECT_GPIO_PORT              GPIOA
                #define SD_DETECT_EXTI_PORT_SOURCE       EXTI_PortSourceGPIOA
                #define SD_DETECT_EXTI_PIN_SOURCE        EXTI_PinSource8
                #define SD_DETECT_EXTI_LINE              EXTI_Line8
                #define SD_DETECT_IRQ                    EXTI9_5_IRQn
                #define SD_DETECT_GPIO_CLK               RCC_AHB1Periph_GPIOA
/*
#if defined __VOLKOV_STM32F405_V2__
                #define DEBUG_LED_PORT                                  GPIOA
                #define DEBUG_LED_PIN                                   GPIO_Pin_8
                #define DEBUG_LED_GPIO_MODE                             GPIO_Mode_OUT
                #define DEBUG_LED_RCC_GPIO                              RCC_AHB1Periph_GPIOA


                #define SD_DETECT_PIN                    GPIO_Pin_13
                #define SD_DETECT_GPIO_PORT              GPIOH
                #define SD_DETECT_GPIO_CLK               RCC_AHB1Periph_GPIOH



#elif defined __TE_STM32F405__

                #define DEBUG_LED_PORT                   GPIOA
                #define DEBUG_LED_PIN                    GPIO_Pin_6
                #define DEBUG_LED_GPIO_MODE              GPIO_Mode_OUT
                #define DEBUG_LED_RCC_GPIO               RCC_AHB1Periph_GPIOA

                #define SD_SOCKET_POWER_PIN              GPIO_Pin_0
                #define SD_SOCKET_POWER_GPIO_PORT        GPIOB
                #define SD_SOCKET_POWER_GPIO_CLK         RCC_AHB1Periph_GPIOB

                #define SD_DETECT_PIN                    GPIO_Pin_1
                #define SD_DETECT_GPIO_PORT              GPIOB
                #define SD_DETECT_GPIO_CLK               RCC_AHB1Periph_GPIOB


#endif


                #define SD_DETECT_PIN                    GPIO_Pin_1
                #define SD_DETECT_GPIO_PORT              GPIOD
                #define SD_DETECT_GPIO_CLK               RCC_AHB1Periph_GPIOD

                #define USB_CONNECT_GPIO_PIN             GPIO_Pin_1
                #define USB_CONNECT_GPIO                 GPIOC
                #define USB_CONNECT_GPIO_CLOCK           RCC_AHB1Periph_GPIOC

                #define SD_SOCKET_POWER_PIN              GPIO_Pin_0
                #define SD_SOCKET_POWER_GPIO_PORT        GPIOD
                #define SD_SOCKET_POWER_GPIO_CLK         RCC_AHB1Periph_GPIOD

                #define MIDI_PIN                         GPIO_Pin_10
                #define MIDI_PORT                        GPIOA
                #define MIDI_CLK                         RCC_AHB1Periph_GPIOA
                #define MIDI_PIN_AFIO                    GPIO_PinSource10
                #define MIDI_PIN_AF_FUN                  GPIO_AF_USART1
                #define MIDI_USART                       USART1
                #define MIDI_USART_CK                    RCC_APB2Periph_USART1
                #define MIDI_DMA_CH                      DMA_Channel_4
                #define MIDI_DMA_STREAM                  DMA2_Stream2
                #define MIDI_DMA_CK                      RCC_AHB1Periph_DMA2
                #define MIDI_DMA_NVIC                    DMA2_Stream2_IRQn

                #define ENC_A_PIN                          GPIO_Pin_14
                #define ENC_A_PORT                         GPIOC
                #define ENC_A_CK                           RCC_AHB1Periph_GPIOC
                #define ENC_B_PIN                          GPIO_Pin_15
                #define ENC_B_PORT                         GPIOC
                #define ENC_B_CK                           RCC_AHB1Periph_GPIOC
                #define ENC_C_PIN                          GPIO_Pin_13
                #define ENC_C_PORT                         GPIOC
                #define ENC_C_CK                           RCC_AHB1Periph_GPIOC

                #define KN_A_PIN                           GPIO_Pin_0
                #define KN_A_PORT                          GPIOC
                #define KN_A_CK                            RCC_AHB1Periph_GPIOC
                #define KN_B_PIN                           GPIO_Pin_3
                #define KN_B_PORT                          GPIOC
                #define KN_B_CK                            RCC_AHB1Periph_GPIOC
*/
#endif /* __PLATFORM_CONFIG_H__ */

