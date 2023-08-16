#include "weact_bsp.h"

#define __WEACT_BSP_VERSION_MAIN   (0x01) /*!< [31:24] main version */
#define __WEACT_BSP_VERSION_SUB1   (0x00) /*!< [23:16] sub1 version */
#define __WEACT_BSP_VERSION_SUB2   (0x00) /*!< [15:8]  sub2 version */
#define __WEACT_BSP_VERSION_RC     (0x00) /*!< [7:0]  release candidate */
#define __WEACT_BSP_VERSION        ((__WEACT_BSP_VERSION_MAIN << 24)\
                                                 |(__WEACT_BSP_VERSION_SUB1 << 16)\
                                                 |(__WEACT_BSP_VERSION_SUB2 << 8 )\
                                                 |(__WEACT_BSP_VERSION_RC))

/** @defgroup WEACT_LOW_LEVEL_Private_Variables WEACT LOW LEVEL Private Variables
  * @{
  */
uint32_t GPIO_PIN[LEDn] = {LED0_PIN};

GPIO_TypeDef* GPIO_PORT[LEDn] = {LED0_GPIO_PORT};

GPIO_TypeDef* BUTTON_PORT[BUTTONn] = {BUTTON0_GPIO_PORT};

const uint16_t BUTTON_PIN[BUTTONn] = {BUTTON0_PIN};

const uint16_t BUTTON_IRQn[BUTTONn] = {BUTTON0_EXTI_IRQn};

USART_TypeDef* COM_USART[COMn] = {WEACT_COM1};

GPIO_TypeDef* COM_TX_PORT[COMn] = {WEACT_COM1_TX_GPIO_PORT};

GPIO_TypeDef* COM_RX_PORT[COMn] = {WEACT_COM1_RX_GPIO_PORT};

const uint16_t COM_TX_PIN[COMn] = {WEACT_COM1_TX_PIN};

const uint16_t COM_RX_PIN[COMn] = {WEACT_COM1_RX_PIN};

const uint16_t COM_TX_AF[COMn] = {WEACT_COM1_TX_AF};

const uint16_t COM_RX_AF[COMn] = {WEACT_COM1_RX_AF};

/**
  * @brief  This method returns the WEACT BSP Driver revision
  * @retval version: 0xXYZR (8bits for each decimal, R for RC)
  */
uint32_t BSP_GetVersion(void)
{
  return __WEACT_BSP_VERSION;
}

/**
  * @brief  Configures LED GPIO.
  * @param  Led: LED to be configured.
  *          This parameter can be one of the following values:
  *            @arg  LED0-LED2
  */
void BSP_LED_Init(Led_TypeDef Led)
{
  GPIO_InitTypeDef  gpio_init_structure = {0};

  if (Led <= LED0)
  {
    LED0_GPIO_CLK_ENABLE();

    /* Configure the GPIO_LED pin */
    gpio_init_structure.Pin   = GPIO_PIN[Led];
    gpio_init_structure.Mode  = GPIO_MODE_OUTPUT_PP;
    gpio_init_structure.Pull  = GPIO_PULLUP;
    gpio_init_structure.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(GPIO_PORT[Led], &gpio_init_structure);

    /* By default, turn off LED by setting a low level on corresponding GPIO */
    BSP_LED_Off(Led);

  } /* of if (Led <= LED2) */
}

/**
  * @brief  DeInit LEDs.
  * @param  Led: LED to be configured.
  *          This parameter can be one of the following values:
  *            @arg  LED0-LED2
  * @note Led DeInit does not disable the GPIO clock nor disable the Mfx
  */
void BSP_LED_DeInit(Led_TypeDef Led)
{
  GPIO_InitTypeDef  gpio_init_structure = {0};

  if (Led <= LED0)
  {
    /* DeInit the GPIO_LED pin */
    gpio_init_structure.Pin = GPIO_PIN[Led];

    HAL_GPIO_DeInit(GPIO_PORT[Led], gpio_init_structure.Pin);
  }
}

/**
  * @brief  Turns selected LED On.
  * @param  Led: LED to be set on
  *          This parameter can be one of the following values:
  *            @arg  LED0-LED2
  */
void BSP_LED_On(Led_TypeDef Led)
{
  switch (Led)
  {
    case LED0:
      HAL_GPIO_WritePin(GPIO_PORT[Led], GPIO_PIN[Led], GPIO_PIN_SET);
      break;
    default:
      break;
 }
}

/**
  * @brief  Turns selected LED Off.
  * @param  Led: LED to be set off
  *          This parameter can be one of the following values:
  *            @arg  LED0-LED2
  */
void BSP_LED_Off(Led_TypeDef Led)
{
  switch (Led)
  {
    case LED0:
      HAL_GPIO_WritePin(GPIO_PORT[Led], GPIO_PIN[Led], GPIO_PIN_RESET);
      break;
    default:
      break;
 }
}

/**
  * @brief  Toggles the selected LED.
  * @param  Led: LED to be toggled
  *          This parameter can be one of the following values:
  *            @arg  LED0-LED2
  */
void BSP_LED_Toggle(Led_TypeDef Led)
{
  if (Led <= LED0)
  {
     HAL_GPIO_TogglePin(GPIO_PORT[Led], GPIO_PIN[Led]);
  }
}

/**
  * @brief  Configures button GPIO and EXTI Line.
  * @param  Button: Button to be configured
  *          This parameter can be one of the following values:
  *            @arg  BUTTON_USER: User Push Button
  * @param  Button_Mode: Button mode
  *          This parameter can be one of the following values:
  *            @arg  BUTTON_MODE_GPIO: Button will be used as simple IO
  *            @arg  BUTTON_MODE_EXTI: Button will be connected to EXTI line
  *                                    with interrupt generation capability
  */
void BSP_PB_Init(Button_TypeDef Button, ButtonMode_TypeDef Button_Mode)
{
  GPIO_InitTypeDef gpio_init_structure = {0};

  /* Enable the BUTTONS clocks */
  BUTTON0_GPIO_CLK_ENABLE();

  if(Button_Mode == BUTTON_MODE_GPIO)
  {
    /* Configure Button pin as input */
    gpio_init_structure.Pin = BUTTON_PIN[Button];
    gpio_init_structure.Mode = GPIO_MODE_INPUT;
    gpio_init_structure.Pull = Button ? GPIO_PULLUP : GPIO_PULLDOWN;
    gpio_init_structure.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(BUTTON_PORT[Button], &gpio_init_structure);
  }

  if(Button_Mode == BUTTON_MODE_EXTI)
  {
    /* Configure Button pin as input with External interrupt */
    gpio_init_structure.Pin = BUTTON_PIN[Button];
    gpio_init_structure.Mode = Button ? GPIO_MODE_IT_FALLING : GPIO_MODE_IT_RISING;
    gpio_init_structure.Pull = Button ? GPIO_PULLUP : GPIO_PULLDOWN;

    HAL_GPIO_Init(BUTTON_PORT[Button], &gpio_init_structure);

    /* Enable and set Button EXTI Interrupt to the lowest priority */
    HAL_NVIC_SetPriority((IRQn_Type)(BUTTON_IRQn[Button]), 0x0F, 0x00);
    HAL_NVIC_EnableIRQ((IRQn_Type)(BUTTON_IRQn[Button]));
  }
}

/**
  * @brief  Push Button DeInit.
  * @param  Button: Button to be configured
  *          This parameter can be one of the following values:
  *            @arg  BUTTON_USER: User Push Button
  * @note PB DeInit does not disable the GPIO clock
  */
void BSP_PB_DeInit(Button_TypeDef Button)
{
  GPIO_InitTypeDef gpio_init_structure = {0};

  gpio_init_structure.Pin = BUTTON_PIN[Button];
  HAL_NVIC_DisableIRQ((IRQn_Type)(BUTTON_IRQn[Button]));
  HAL_GPIO_DeInit(BUTTON_PORT[Button], gpio_init_structure.Pin);
}

/**
  * @brief  Returns the selected button state.
  * @param  Button: Button to be checked
  *          This parameter can be one of the following values:
  *            @arg  BUTTON_USER: User Push Button
  * @retval The Button GPIO pin value
  */
uint32_t BSP_PB_GetState(Button_TypeDef Button)
{
  return HAL_GPIO_ReadPin(BUTTON_PORT[Button], BUTTON_PIN[Button]);
}

/**
  * @brief  Configures COM port.
  * @param  COM: COM port to be configured.
  *          This parameter can be one of the following values:
  *            @arg  COM1
  * @param  huart: Pointer to a UART_HandleTypeDef structure that contains the
  *                configuration information for the specified USART peripheral.
  */
void BSP_COM_Init(COM_TypeDef COM, UART_HandleTypeDef *huart)
{
  GPIO_InitTypeDef gpio_init_structure;

  /* Enable GPIO clock of GPIO port supporting COM_TX and COM_RX pins */
  WEACT_COMx_TX_RX_GPIO_CLK_ENABLE(COM);

  /* Enable USART clock */
  WEACT_COMx_CLK_ENABLE(COM);

  /* Configure USART Tx as alternate function */
  gpio_init_structure.Pin = COM_TX_PIN[COM];
  gpio_init_structure.Mode = GPIO_MODE_AF_PP;
  gpio_init_structure.Pull = GPIO_PULLUP;
  gpio_init_structure.Speed = GPIO_SPEED_FREQ_LOW;
  gpio_init_structure.Alternate = COM_TX_AF[COM];
  HAL_GPIO_Init(COM_TX_PORT[COM], &gpio_init_structure);

  /* Configure USART Rx as alternate function */
  gpio_init_structure.Pin = COM_RX_PIN[COM];
  HAL_GPIO_Init(COM_RX_PORT[COM], &gpio_init_structure);

  /* USART configuration */
  huart->Instance = COM_USART[COM];
  HAL_UART_Init(huart);
}

/**
  * @brief  DeInit COM port.
  * @param  COM: COM port to be configured.
  *          This parameter can be one of the following values:
  *            @arg  COM1 
  * @param  huart: Pointer to a UART_HandleTypeDef structure that contains the
  *                configuration information for the specified USART peripheral.
  */
void BSP_COM_DeInit(COM_TypeDef COM, UART_HandleTypeDef *huart)
{
  /* USART configuration */
  huart->Instance = COM_USART[COM];
  HAL_UART_DeInit(huart);

  /* Enable USART clock */
  WEACT_COMx_CLK_DISABLE(COM);

  /* DeInit GPIO pins can be done in the application 
     (by surcharging this __weak function) */

  /* GPOI pins clock, FMC clock and DMA clock can be shut down in the applic 
     by surcgarging this __weak function */ 
}
