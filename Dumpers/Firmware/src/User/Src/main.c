//-----------------------------------------------------------------------------
// SNK MultiCart Dumper/Writer v1.00 on STM32H750VBT6
// project started 05.19.2023                                  (c) Vortex '2023
//-----------------------------------------------------------------------------

// 1.00 - (08.16.20xx)

#include "main.h"

// cn de/scramble address in defines

uint32_t Mount_Dump(void)
{
  FRESULT fr;

  fr = f_mount(&SDFatFs, "0:", 1);
  if (fr == FR_OK) {
	fr = f_open(&file, get_dump_filename(), FA_CREATE_ALWAYS | FA_WRITE);
	if (fr == FR_OK) {
      return 0;
    }  else return 2;
  } else return 1;
}

uint32_t Mount_Prog(void)
{
  FRESULT fr;

  fr = f_mount(&SDFatFs, "0:", 1);
  if (fr == FR_OK) {
	fr = f_open(&file, get_prog_filename(), FA_READ);
	if (fr == FR_OK) {
      return 0;
    }  else return 2;
  } else return 1;
}

void doGpioInit(void)
{
  switch (cur_chip) {
    case CHIP_P:
      P_GPIO_Init();
      break;
    case CHIP_S:
    case CHIP_M:
      SM_GPIO_Init();
      break;
    case CHIP_C:
    case CHIP_V:
      CV_GPIO_Init();
      break;
    default:
      break;
  }
}
void doTest(void)
{
  if (flg_test) return;

  switch (cur_chip) {
    case CHIP_P:
      P_Test();
      break;
    case CHIP_S:
    case CHIP_M:
      SM_Test();
      break;
    case CHIP_C:
    case CHIP_V:
      CV_Test();
      break;
    default:
      break;
  }
  
  flg_test = 1;
}

void doDump(void)
{
  UINT bw;

  switch (cur_chip) {
    case CHIP_P:
      P_Dump();
      break;
    case CHIP_S:
    case CHIP_M:
      SM_Dump();
      break;
    case CHIP_C:
    case CHIP_V:
      CV_Dump();
      break;
    default:
      break;
  }

  if (buffer_pos == 0) {
    f_write(&file, &buffer, BUFFER_SIZE, &bw);
    if (bw != BUFFER_SIZE) {
      f_close(&file);
      error = 1;
      cur_menu = MENU_ERROR;
      LCD_Clear();
    }
  }
  if (address >= get_end_address()) {
    f_close(&file);
    cur_menu = MENU_DONE;
    LCD_Clear();
    return;
  }
}

void doProg(void)
{
  UINT br;

  if (buffer_pos == 0) {
    f_read(&file, &buffer, BUFFER_SIZE, &br);
    if (br != BUFFER_SIZE) {
      f_close(&file);
      error = 1;
      cur_menu = MENU_ERROR;
      LCD_Clear();
    }
  }

  switch (cur_chip) {
    case CHIP_P:
      P_Prog();
      break;
    case CHIP_S:
    case CHIP_M:
      SM_Prog();
      break;
    case CHIP_C:
    case CHIP_V:
      CV_Prog();
      break;
    default:
      break;
  }

  if (address >= get_end_address()) {
    f_close(&file);
    cur_menu = MENU_DONE;
    LCD_Clear();
    return;
  }
}

void doVeri(void)
{
  UINT br;

  if (buffer_pos == 0) {
    f_read(&file, &buffer, BUFFER_SIZE, &br);
    if (br != BUFFER_SIZE) {
      f_close(&file);
      error = 1;
      cur_menu = MENU_ERROR;
      LCD_Clear();
    }
  }

  switch (cur_chip) {
    case CHIP_P:
      P_Veri();
      break;
    case CHIP_S:
    case CHIP_M:
      SM_Veri();
      break;
    case CHIP_C:
    case CHIP_V:
      CV_Veri();
      break;
    default:
      break;
  }

  if (address >= get_end_address()) {
    f_close(&file);
    cur_menu = MENU_DONE;
    LCD_Clear();
    return;
  }
}

int main()
{
  /* MPU Configuration */
  MPU_Config();

  /* Enable I-Cache */
  SCB_EnableICache();

  /* Enable D-Cache */
  SCB_EnableDCache();

  /* STM32H7xx HAL library initialization */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();
  Delay_Init();

  /* Initialize GPIO */
  MX_GPIO_Init();

  /* Initialize LED/BTN  */
  BSP_LED_Init(LED_BLUE);
  BSP_PB_Init(BUTTON_BRD,  BUTTON_MODE_GPIO);
  
  /* Initialize Timers */
  MX_TIM1_Init();

  /* Initialize SD-Card */
  BSP_SD_Init();
  MX_FATFS_Init();

  /* Init USB device Library */
  BSP_USB_DEVICE_Init();

  /* Initialize SPI */
  MX_SPI4_Init();

  /* Initialize UART */
  // MX_USART3_UART_Init();

  /* Initialize LCD */
  LCD_Init();
  LCD_Clear();
  
  /* Infinite loop */
  while (1)
  {
    if (flag_button & FLAG_BTN_BRD)
    {
      flag_button &= ~FLAG_BTN_BRD;
      switch (cur_menu) {
        case MENU_CSEL:
          if (++cur_chip > CHIP_V) {
            cur_chip = CHIP_P;
          }
          break;
        case MENU_MSEL:
          if (++cur_mode > MODE_VERI) {
            cur_mode = MODE_TEST;
          }
          break;
        case MENU_PROG:
        case MENU_DUMP:
        case MENU_VERI:
          // do nothing
          break;
        case MENU_TEST:
        case MENU_DONE:
        case MENU_ERROR:
          cur_menu = MENU_MSEL;
          LCD_Clear();
          break;
        default:
          break;
      }
    }

    if (flag_button & FLAG_BTN_BRD_LONG)
    {
      flag_button &= ~FLAG_BTN_BRD_LONG;
      switch (cur_menu) {
        case MENU_CSEL:
          cur_menu = MENU_MSEL;
          LCD_Clear();
		  doGpioInit();
          break;
        case MENU_MSEL:
          switch (cur_mode) {
            case MODE_TEST:
              cur_menu = MENU_TEST;
              LCD_Clear();
              break;
            case MODE_DUMP:
              if (Mount_Dump() == 0) {
                cur_menu = MENU_DUMP;
                LCD_Clear();
              }
			  else {
				error = 1;
                cur_menu = MENU_ERROR;
                LCD_Clear();
			  }
              break;
            case MODE_PROG:
              if (Mount_Prog() == 0) {
                cur_menu = MENU_PROG;
                LCD_Clear();
              }
			  else {
				error = 1;
                cur_menu = MENU_ERROR;
                LCD_Clear();
			  }
              break;
            case MODE_VERI:
              if (Mount_Prog() == 0) {
                cur_menu = MENU_VERI;
                LCD_Clear();
              }
			  else {
				error = 1;
                cur_menu = MENU_ERROR;
                LCD_Clear();
			  }
              break;
            default:
              break;
          }
          flg_test   = 0;
          buffer_pos = 0;
          address = 0;
          test    = 0;
          error   = 0;
          break;
        case MENU_PROG:
        case MENU_DUMP:
        case MENU_VERI:
          f_close(&file);
          cur_menu = MENU_MSEL;
          LCD_Clear();
          break;
        case MENU_TEST:
        case MENU_DONE:
        case MENU_ERROR:
          cur_menu = MENU_MSEL;
          LCD_Clear();
          break;
        default:
          break;
      }
    }

    switch (cur_menu) {
      case MENU_TEST:
        doTest();
        break;
      case MENU_DUMP:
        doDump();
        break;
      case MENU_PROG:
        doProg();
        break;
      case MENU_VERI:
        doVeri();
        break;
      default:
        break;
    }

    if (flag_lcddraw) {
      flag_lcddraw = 0;
      doLCD();
      LCD_Draw();
    }
  }
}

// Main (0.01s) Timer
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if(htim->Instance!=TIM1) return;
  
  if (++timLcdCnt >= 20) {
    timLcdCnt = 0;
    flag_lcddraw = 1;
  }
  if (BSP_PB_GetState(BUTTON_BRD) == GPIO_PIN_SET)
    timBtnCnt[BUTTON_BRD]++;
  else {
    if (timBtnCnt[BUTTON_BRD] >= 50) // BTN: 0.5 ñ.
      flag_button |= FLAG_BTN_BRD_LONG;
    else
    if (timBtnCnt[BUTTON_BRD] >= 10) // BTN: 0.1 ñ.
      flag_button |= FLAG_BTN_BRD;
    timBtnCnt[BUTTON_BRD] = 0;
  }
}
