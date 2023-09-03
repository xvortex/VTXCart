#ifndef __VARIABLES_H
#define __VARIABLES_H

#ifdef __cplusplus
 extern "C" {
#endif

// MX Handles
extern UART_HandleTypeDef huart3;
extern SPI_HandleTypeDef hspi4;
extern SD_HandleTypeDef hsd1;
extern PCD_HandleTypeDef hpcd_USB_OTG_FS;
extern USBD_HandleTypeDef hUsbDeviceHS;
extern TIM_HandleTypeDef htim1;

// flags
extern volatile uint32_t flag_mounted;
extern volatile uint32_t flag_lcddraw;
extern volatile uint32_t flag_button;

// FatFs
extern char SDPath[4];
extern FATFS SDFatFs;
extern FIL file;
extern uint8_t winbuf[FF_MAX_SS];
extern uint8_t filbuf[FF_MAX_SS];

// lcd
extern uint8_t lcd_cache[2][LCD_WIDTH+2];

extern char video_mem[2][256];
extern uint32_t video_attr[2];

extern uint32_t scroll_pos[2];
extern uint32_t scroll_delay[2];

// uart
extern uint8_t UARTTxBuffer[UART_BUFFER_SIZE] SRAM_BUFFER;
extern uint32_t UARTTxBuffer_head, UARTTxBuffer_tail, UARTTxBuffer_len;

// menu
extern uint32_t cur_menu;
extern uint32_t cur_chip;
extern uint32_t cur_mode;

// timer
extern uint32_t timLcdCnt;
extern uint32_t timBtnCnt[BUTTONn];

// dump buffer
extern uint8_t buffer [BUFFER_SIZE] SRAM_BUFFER;
extern uint32_t buffer_pos;

// other
extern uint32_t address;
extern uint32_t sector;
extern uint32_t test;
extern uint32_t error;

// flags
extern uint32_t flg_test;
extern uint32_t flg_seek;
                        
#ifdef __cplusplus
}
#endif

#endif /* __VARIABLES_H */
