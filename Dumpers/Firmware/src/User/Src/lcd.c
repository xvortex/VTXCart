#include "main.h"
#include "lcd.h"

// LCD_RS
#define LCD_RS_SET HAL_GPIO_WritePin(LCD_WR_RS_GPIO_PORT, LCD_WR_RS_PIN, GPIO_PIN_SET)
#define LCD_RS_RESET HAL_GPIO_WritePin(LCD_WR_RS_GPIO_PORT, LCD_WR_RS_PIN, GPIO_PIN_RESET)

// LCD_CS
#define LCD_CS_SET HAL_GPIO_WritePin(LCD_CS_GPIO_PORT, LCD_CS_PIN, GPIO_PIN_SET)
#define LCD_CS_RESET HAL_GPIO_WritePin(LCD_CS_GPIO_PORT, LCD_CS_PIN, GPIO_PIN_RESET)

// SPI Driver
#define SPI spi4
#define SPI_Drv (&hspi4)

static int32_t lcd_init(void);
static int32_t lcd_gettick(void);
static int32_t lcd_writereg(uint8_t reg, uint8_t *pdata, uint32_t length);
static int32_t lcd_readreg(uint8_t reg, uint8_t *pdata);
static int32_t lcd_senddata(uint8_t *pdata, uint32_t length);
static int32_t lcd_recvdata(uint8_t *pdata, uint32_t length);

ST7735_IO_t st7735_pIO = {
	lcd_init,
	NULL,
	NULL,
	lcd_writereg,
	lcd_readreg,
	lcd_senddata,
	lcd_recvdata,
	lcd_gettick};

ST7735_Object_t st7735_pObj;

static int32_t lcd_init(void)
{
	int32_t result = ST7735_OK;
	return result;
}

static int32_t lcd_gettick(void)
{
	return HAL_GetTick();
}

static int32_t lcd_writereg(uint8_t reg, uint8_t *pdata, uint32_t length)
{
	int32_t result;
	LCD_CS_RESET;
	LCD_RS_RESET;
	result = HAL_SPI_Transmit(SPI_Drv, &reg, 1, 100);
	LCD_RS_SET;
	if (length > 0)
		result += HAL_SPI_Transmit(SPI_Drv, pdata, length, 500);
	LCD_CS_SET;
	if (result > 0)
	{
		result = -1;
	}
	else
	{
		result = 0;
	}
	return result;
}

static int32_t lcd_readreg(uint8_t reg, uint8_t *pdata)
{
	int32_t result;
	LCD_CS_RESET;
	LCD_RS_RESET;

	result = HAL_SPI_Transmit(SPI_Drv, &reg, 1, 100);
	LCD_RS_SET;
	result += HAL_SPI_Receive(SPI_Drv, pdata, 1, 500);
	LCD_CS_SET;
	if (result > 0)
	{
		result = -1;
	}
	else
	{
		result = 0;
	}
	return result;
}

static int32_t lcd_senddata(uint8_t *pdata, uint32_t length)
{
	int32_t result;
	LCD_CS_RESET;
	// LCD_RS_SET;
	result = HAL_SPI_Transmit(SPI_Drv, pdata, length, 100);
	LCD_CS_SET;
	if (result > 0)
	{
		result = -1;
	}
	else
	{
		result = 0;
	}
	return result;
}

static int32_t lcd_recvdata(uint8_t *pdata, uint32_t length)
{
	int32_t result;
	LCD_CS_RESET;
	// LCD_RS_SET;
	result = HAL_SPI_Receive(SPI_Drv, pdata, length, 500);
	LCD_CS_SET;
	if (result > 0)
	{
		result = -1;
	}
	else
	{
		result = 0;
	}
	return result;
}


extern const uint16_t font32 [];
uint16_t lcd_buf[32*16] SRAM_BUFFER;
uint8_t prev_lm, prev_attr[2];

void LCD_ShowChar(uint16_t x, uint16_t y, uint8_t num)
{
    uint16_t temp;
	uint8_t  xx, yy;
    uint16_t ix = 0;

    if ((num < 0x20) || (num > 0x7f)) return;
	num -= 0x20;

	for (yy = 0; yy < 32; yy++)
	{
        temp = font32[num * 32 + yy];

		for (xx = 0; xx < 16; xx++)
		{
			if (temp & 0x8000)
                lcd_buf[ix++] = 0xFFFF;
			else
                lcd_buf[ix++] = 0x0000;

			temp <<= 1;
		}
	}
	ST7735_FillRGBRect(&st7735_pObj, x, y, (uint8_t *)&lcd_buf, 16, 32);
}

void LCD_ShowString(uint16_t x, uint16_t y, uint8_t *p)
{
	while ((*p <= 0x7f) && (*p >= 0x20))
	{
		LCD_ShowChar(x, y, *p++);
		x += 16;
	}
}

void LCD_Clear(void)
{
    ST7735_FillRect(&st7735_pObj, 0, 0, 160, 80, 0x0000);
    memset(lcd_cache, 0x20, sizeof(lcd_cache));
    scroll_delay[0] = scroll_delay[1] = 0;
    scroll_pos[0]   = scroll_pos[1] = 0;
}

void LCD_Init(void)
{
	ST7735Ctx.Orientation = ST7735_ORIENTATION_LANDSCAPE_ROT180;
	ST7735Ctx.Panel = HannStar_Panel;
	ST7735Ctx.Type = ST7735_0_9_inch_screen;
	ST7735_RegisterBusIO(&st7735_pObj, &st7735_pIO);
	ST7735_LCD_Driver.Init(&st7735_pObj, ST7735_FORMAT_RBG565, &ST7735Ctx);
}

void LCD_Draw(void)
{
  uint8_t i, f, ln, lm, xx, yy, ch;

  if (strlen (video_mem[1])) {
    lm = 2;
    yy  = 8;
  } else {
    lm = 1;
    yy  = 24;
  }

  f = 0;
  if (prev_lm != lm) f = 1;
  if (prev_attr[0] != video_attr[0]) f = 1;
  if (prev_attr[1] != video_attr[1]) f = 1;
  prev_lm = lm;
  prev_attr[0] = video_attr[0];
  prev_attr[1] = video_attr[1];
  if (f) LCD_Clear();

  for (ln = 0; ln < lm; ln++) {
    if (strlen (video_mem[ln]) > LCD_WIDTH) {
      
      if (scroll_delay[ln])
        scroll_delay[ln]--;
      else {
        if (++scroll_pos[ln] > strlen (video_mem[ln]))
          scroll_pos[ln] = 0;
      }

      xx = 0;
      f = 0;
      for (i = 0; i < LCD_WIDTH; i++) {
        if (!f) {
          if (video_mem[ln][scroll_pos[ln] + i])
            ch = video_mem[ln][scroll_pos[ln] + i];
          else
            f = 1;
        }
        if (f) {
          if (f == 1)
            ch = 0x20;
          else
            ch = video_mem[ln][f - 2];
          f++;
        }
        if (lcd_cache[ln][i] != ch) {
          lcd_cache[ln][i] = ch;
          LCD_ShowChar(xx, ln ? 40 : yy, ch);
        }
        xx += 16;
      }
    }
    else
    {
      xx  = (video_attr[ln] & 0x01) ? 80 - strlen (video_mem[ln]) * 8 : 0;
      f = 0;
      for (i = 0; i < LCD_WIDTH; i++) {
        if (!f) {
          if (video_mem[ln][i])
            ch = video_mem[ln][i];
          else
            f = 1;
        }
        if (f) {
          ch = 0x20;
        }
        if (lcd_cache[ln][i] != ch) {
          lcd_cache[ln][i] = ch;
          LCD_ShowChar(xx, ln ? 40 : yy, ch);
        }
        xx += 16;
      }
    }
  }
}

extern uint32_t address;
extern uint32_t test;
extern uint32_t error;

void doLCD(void)
{
  memset(video_mem, 0, sizeof(video_mem));
  video_attr[0] = video_attr[1] = 0;

  switch (cur_menu) {
    case MENU_CSEL:
      sprintf(video_mem[0], get_chip_name());
      sprintf(video_mem[1], "VTX V%s", VERSION);
      video_attr[0] = video_attr[1] = 1;
      break;
    case MENU_MSEL:
      switch (cur_mode) {
        case MODE_TEST:
          sprintf(video_mem[0], "TEST");
          break;
        case MODE_DUMP:
          sprintf(video_mem[0], "DUMP");
          break;
        case MODE_PROG:
          sprintf(video_mem[0], "PROG");
          break;
        case MODE_VERI:
          sprintf(video_mem[0], "VERI");
          break;
        default:
          break;
      }
      sprintf(video_mem[1], get_chip_name());
      video_attr[0] = video_attr[1] = 1;
      break;
    case MENU_TEST:
      sprintf(video_mem[0], "E:%d", error);
      sprintf(video_mem[1], "T:%08lX", test);
      break;
    case MENU_DUMP:
      sprintf(video_mem[0], "D:%02d%% E:%d", address / (get_end_address() / 100), error);
      sprintf(video_mem[1], "A:%08lX", address);
      break;
    case MENU_PROG:
      sprintf(video_mem[0], "P:%02d%% E:%d", address / (get_end_address() / 100), error);
      sprintf(video_mem[1], "A:%08lX", address);
      break;
    case MENU_VERI:
      sprintf(video_mem[0], "V:%02d%% E:%d", address / (get_end_address() / 100), error);
      sprintf(video_mem[1], "A:%08lX", address);
      break;
    case MENU_DONE:
      sprintf(video_mem[0], "DONE");
      sprintf(video_mem[1], "%d", error);
      video_attr[0] = video_attr[1] = 1;
      break;
    case MENU_ERROR:
      sprintf(video_mem[0], "ERROR");
      sprintf(video_mem[1], "%d", error);
      video_attr[0] = video_attr[1] = 1;
      break;
    default:
      break;
  }
}
