#ifndef __WEACT_BSP_H
#define __WEACT_BSP_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32h7xx_hal.h"
#include "system_stm32h7xx.h"

/** @brief Led_TypeDef
  *  WEACT board leds definitions.
  */
typedef enum
{
  LED0       = 0,
  LED_BLUE   = LED0
} Led_TypeDef;

/** @brief Button_TypeDef
  *  WEACT board Buttons definitions.
  */
typedef enum
{
  BUTTON0     = 0,
  BUTTON_BRD  = BUTTON0

} Button_TypeDef;

/** @brief ButtonMode_TypeDef
  *  WEACT board Buttons Modes definitions.
  */
typedef enum
{
  BUTTON_MODE_GPIO = 0,
  BUTTON_MODE_EXTI = 1

} ButtonMode_TypeDef;

/** @addtogroup Exported_types
  * @{
  */ 
typedef enum 
{
  PB_SET = 0, 
  PB_RESET = !PB_SET

} ButtonValue_TypeDef;

/** @brief COM_TypeDef
  *  WEACT board COM ports.
  */
typedef enum
{
  COM1 = 0

} COM_TypeDef;

/** @brief WEACT_Status_TypeDef
  *  WEACT board Status return possible values.
  */
typedef enum
{
  WEACT_OK    = 0,
  WEACT_ERROR = 1

} WEACT_Status_TypeDef;

#define LEDn                             ((uint8_t)1)

/* LEDs */

// onboard blue
#define LED0_PIN                         ((uint32_t)GPIO_PIN_3)
#define LED0_GPIO_PORT                   ((GPIO_TypeDef*)GPIOE)
#define LED0_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOE_CLK_ENABLE()
#define LED0_GPIO_CLK_DISABLE()          __HAL_RCC_GPIOE_CLK_DISABLE()

/* buttons */
#define BUTTONn                          ((uint8_t)1)

/**
  * @brief User push-buttons
  */
#define BUTTON0_PIN                  GPIO_PIN_13
#define BUTTON0_GPIO_PORT            GPIOC
#define BUTTON0_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOC_CLK_ENABLE()
#define BUTTON0_GPIO_CLK_DISABLE()   __HAL_RCC_GPIOC_CLK_DISABLE()
#define BUTTON0_EXTI_IRQn            EXTI15_10_IRQn

/** @defgroup WEACT_LOW_LEVEL_COM WEACT LOW LEVEL COM
  * @{
  */
#define COMn                             ((uint8_t)1)

/**
 * @brief Definition for COM port1, connected to USART3
 */
#define WEACT_COM1                       USART3
#define WEACT_COM1_CLK_ENABLE()          __HAL_RCC_USART3_CLK_ENABLE()
#define WEACT_COM1_CLK_DISABLE()         __HAL_RCC_USART3_CLK_DISABLE()

#define WEACT_COM1_TX_PIN                GPIO_PIN_10
#define WEACT_COM1_RX_PIN                GPIO_PIN_11
#define WEACT_COM1_TX_GPIO_PORT          GPIOB
#define WEACT_COM1_RX_GPIO_PORT          GPIOB

#define WEACT_COM1_TX_AF                 GPIO_AF7_USART3
#define WEACT_COM1_RX_AF                 GPIO_AF7_USART3

#define WEACT_COM1_TX_RX_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOB_CLK_ENABLE()
#define WEACT_COM1_TX_RX_GPIO_CLK_DISABLE() __HAL_RCC_GPIOB_CLK_DISABLE()

#define WEACT_COM1_IRQn                  USART3_IRQn

#define WEACT_COMx_CLK_ENABLE(__INDEX__)               do { if((__INDEX__) == 0) {WEACT_COM1_CLK_ENABLE();} } while(0)
#define WEACT_COMx_CLK_DISABLE(__INDEX__)              (((__INDEX__) == 0) ? WEACT_COM1_CLK_DISABLE() : 0)

#define WEACT_COMx_TX_RX_GPIO_CLK_ENABLE(__INDEX__)    do { if((__INDEX__) == 0) {WEACT_COM1_TX_RX_GPIO_CLK_ENABLE();} } while(0)
#define WEACT_COMx_TX_RX_GPIO_CLK_DISABLE(__INDEX__)   (((__INDEX__) == 0) ? WEACT_COM1_TX_RX_GPIO_CLK_DISABLE() : 0)

#define LCD_LED_PIN                      GPIO_PIN_10
#define LCD_LED_GPIO_PORT                GPIOE

#define LCD_CS_PIN                       GPIO_PIN_11
#define LCD_CS_GPIO_PORT                 GPIOE

#define LCD_WR_RS_PIN                    GPIO_PIN_13
#define LCD_WR_RS_GPIO_PORT              GPIOE

/** @defgroup WEACT_LOW_LEVEL_Exported_Functions WEACT LOW LEVEL Exported Functions
  * @{
  */
uint32_t         BSP_GetVersion(void);
void             BSP_LED_Init(Led_TypeDef Led);
void             BSP_LED_DeInit(Led_TypeDef Led);
void             BSP_LED_On(Led_TypeDef Led);
void             BSP_LED_Off(Led_TypeDef Led);
void             BSP_LED_Toggle(Led_TypeDef Led);
void             BSP_PB_Init(Button_TypeDef Button, ButtonMode_TypeDef Button_Mode);
void             BSP_PB_DeInit(Button_TypeDef Button);
uint32_t         BSP_PB_GetState(Button_TypeDef Button);
void             BSP_COM_Init(COM_TypeDef COM, UART_HandleTypeDef *husart);
void             BSP_COM_DeInit(COM_TypeDef COM, UART_HandleTypeDef *huart);

#ifdef __cplusplus
}
#endif

#endif /* __WEACT_BSP_H */
