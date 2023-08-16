#include "main.h"
#include "SM.h"

// JS28F512
#define SECTOR_SIZE 0x10000

static inline void SM_nBYTE(uint32_t st)
{
  GPIOD -> BSRR = st ? BIT1 : BIT1 << 16;
}

static inline void SM_nCE(uint32_t st)
{
  GPIOD -> BSRR = st ? BIT3 : BIT3 << 16;
}

static inline void SM_nOE(uint32_t st)
{
  GPIOD -> BSRR = st ? BIT4 : BIT4 << 16;
}

static inline void SM_nRST(uint32_t st)
{
  GPIOD -> BSRR = st ? BIT5 : BIT5 << 16;
}

static inline void SM_nWE(uint32_t st)
{
  GPIOD -> BSRR = st ? BIT6 : BIT6 << 16;
}

static inline void SM_nWP(uint32_t st)
{
  GPIOD -> BSRR = st ? BIT7 : BIT7 << 16;
}

static inline void SM_OUT(uint32_t out)
{
  GPIOA -> MODER &= ~(0xffff);
  GPIOC -> MODER &= ~(0xffff);
  if (out)
  {
	GPIOA -> MODER |= 0x5555;
    GPIOC -> MODER |= 0x5555;
  }
}

static inline void SM_SetAddress(uint32_t addr)
{
  GPIOB -> ODR &= ~(0xffff);
  GPIOE -> ODR &= ~(0x03ff);
  GPIOB -> ODR |= addr & 0xffff;
  GPIOE -> ODR |= (addr >> 16) & 0x3ff;
}

static inline void SM_SetData(uint32_t data)
{
  GPIOA -> ODR &= ~(0xff);
  GPIOC -> ODR &= ~(0xff);
  GPIOA -> ODR |= data & 0xff;
  GPIOC -> ODR |= (data >> 8) & 0xff;
}

static inline uint32_t SM_GetData(void)
{
  uint32_t data = ((GPIOA -> IDR) & 0xff);
	data |= ((GPIOC -> IDR) & 0xff) << 8;

  return data;
}

void SM_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  // PA0-PA7: D0-D7
  __HAL_RCC_GPIOA_CLK_ENABLE();
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  
  // PB0-PB15: A0-A15
  __HAL_RCC_GPIOB_CLK_ENABLE();
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|
	                    GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  // PC0-PC7: D8-D15
  __HAL_RCC_GPIOC_CLK_ENABLE();
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  // PD0: BY#
  __HAL_RCC_GPIOD_CLK_ENABLE();
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
  
  // PD1, PD3, PD4, PD5, PD6, PD7: BY#, BYTE#, CE#, OE#, RST#, WE#, WP#
  __HAL_RCC_GPIOD_CLK_ENABLE();
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
  
  // PE0-PE9: A16-A25
  __HAL_RCC_GPIOE_CLK_ENABLE();
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|
	                    GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
  
  SM_nBYTE(1);
  SM_nCE(0);
  SM_nOE(1);
  SM_nWE(1);
  SM_nWP(1);
  SM_nRST(1);
}

uint32_t SM_ReadData(uint32_t addr)
{
  uint32_t data;
  
  SM_SetAddress(addr);
  SM_nOE(0);
  Delay_cycles(50); // 100ns
  data = SM_GetData();
  SM_nOE(1);

  return data;
}

void SM_WriteData(uint32_t addr, uint32_t data)
{
  SM_SetAddress(addr);
  SM_SetData(data);
  SM_OUT(1);
  SM_nWE(0);
  Delay_cycles(50); // 100ns
  SM_nWE(1);
  SM_OUT(0);
}

void SM_ReadID(void)
{
  SM_WriteData(0x555, 0xAA); // unlock 1
  SM_WriteData(0x2AA, 0x55); // unlock 2
  SM_WriteData(0x555, 0x90); // autoselect
  test = SM_ReadData(0x0E);  // read ID
  SM_WriteData(0x000, 0xF0); //  reset
}

void SM_SectorErase(uint32_t addr)
{
  SM_WriteData(0x555, 0xAA); // unlock 1
  SM_WriteData(0x2AA, 0x55); // unlock 2
  SM_WriteData(0x555, 0x80); // setup
  SM_WriteData(0x555, 0xAA); // unlock 1
  SM_WriteData(0x2AA, 0x55); // unlock 2
  SM_WriteData(addr,  0x30); // sector erase
  do
  {
    test = SM_ReadData(addr);
  }
  while ((test | 0xFF7F) != 0xFFFF);
}

uint32_t SM_Read(void)
{
  uint32_t addr;
  uint32_t data;
  uint32_t d[2];

  addr = address++;
  SM_SetAddress(addr);
  SM_nOE(0);
  Delay_cycles(50);
  d[0] = SM_GetData();
  Delay_cycles(10);
  d[1] = SM_GetData();
  while (d[0] != d[1])
  {
	d[0] = d[1];
    Delay_cycles(10);
    d[1] = SM_GetData();
    error++;
  }
  SM_nOE(1);
  data = d[0];
  
  return data;
}

void SM_Dump(void)
{
  uint32_t data = SM_Read();

  buffer[buffer_pos++] = data;
  buffer[buffer_pos++] = data >> 8;
  if (buffer_pos >= BUFFER_SIZE) {
	buffer_pos = 0;
  }
}

void SM_Prog(void)
{
  uint32_t addr, saddr, laddr;
  uint32_t data;
  uint32_t f;

  addr = address;
  address += 32;
  saddr = addr & ~(SECTOR_SIZE-1);
  if ((addr & (SECTOR_SIZE-1)) == 0) {
    SM_SectorErase(saddr);
  }
  f = 1;
  for (uint32_t i = 0; i < 64; i++)
  {
	if (buffer[buffer_pos + i] != 0xFF)
	{
	  f = 0;
	  break;
	}
  }
  if (f)
  {
	buffer_pos += 64;
  }
  else
  {
    SM_WriteData(0x555, 0xAA); // unlock 1
    SM_WriteData(0x2AA, 0x55); // unlock 2
    SM_WriteData(saddr, 0x25); // write buffer load
    SM_WriteData(saddr, 0x1F); // count
    for (uint32_t i = 0; i < 32; i++)
    {
      data = buffer[buffer_pos++];
      data |= buffer[buffer_pos++] << 8;
      laddr = addr + i;
      SM_WriteData(laddr, data);
    }
    SM_WriteData(saddr, 0x29);  // write buffer to flash
    do
    {
      test = SM_ReadData(laddr);
    }
    while ((test | 0xFF7F) != (data | 0xFF7F));
  }
  if (buffer_pos >= BUFFER_SIZE) {
	buffer_pos = 0;
  }
}

void SM_Veri(void)
{
  uint32_t data = SM_Read();
  uint32_t err = 0;
  
  if (buffer[buffer_pos++] != (data & 0xFF)) err++;
  if (buffer[buffer_pos++] != ((data >> 8) & 0xFF)) err++;
  
  if (err) error++;
  
  if (buffer_pos >= BUFFER_SIZE) {
	buffer_pos = 0;
  }
}

void SM_Test(void)
{
  SM_ReadID();
}
