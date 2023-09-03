#include "main.h"
#include "variables.h"

// MX Handles
UART_HandleTypeDef huart3;
SPI_HandleTypeDef hspi4;
SD_HandleTypeDef hsd1;
USBD_HandleTypeDef hUsbDeviceHS;
TIM_HandleTypeDef htim1;

// flags
volatile uint32_t flag_mounted;
volatile uint32_t flag_lcddraw;
volatile uint32_t flag_button;

// FatFs
char SDPath[4];
FATFS SDFatFs;
FIL file;
uint8_t winbuf[FF_MAX_SS]                SRAM_BUFFER;
uint8_t filbuf[FF_MAX_SS]                SRAM_BUFFER;

// lcd
uint8_t lcd_cache[2][LCD_WIDTH+2];

char video_mem[2][256];
uint32_t video_attr[2];

uint32_t scroll_pos[2];
uint32_t scroll_delay[2];

// uart
uint8_t UARTTxBuffer[UART_BUFFER_SIZE] SRAM_BUFFER;
uint32_t UARTTxBuffer_head, UARTTxBuffer_tail, UARTTxBuffer_len;

// menu
uint32_t cur_menu = MENU_CSEL;
uint32_t cur_chip = CHIP_P;
uint32_t cur_mode = MODE_TEST;

// timer
uint32_t timLcdCnt;
uint32_t timBtnCnt[BUTTONn];

// dump buffer
uint8_t buffer [BUFFER_SIZE] SRAM_BUFFER;
uint32_t buffer_pos;

// other
uint32_t address;
uint32_t sector;
uint32_t test;
uint32_t error;

// flags
uint32_t flg_test;
uint32_t flg_seek;
