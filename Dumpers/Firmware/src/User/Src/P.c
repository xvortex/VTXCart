#include "main.h"
#include "P.h"

// 55LV100S (2xS29GL512P)
#define SECTOR_SIZE 0x20000

#ifdef DE_SCRAMBLE_ADDR_P
static uint32_t p_desc_address(uint32_t addr) // china pinout descramble
{
  uint32_t address = 0;

  if (addr & BIT0) address |= BIT4;
  if (addr & BIT1) address |= BIT3;
  if (addr & BIT2) address |= BIT2;
  if (addr & BIT3) address |= BIT1;
  if (addr & BIT4) address |= BIT0;
  if (addr & BIT5) address |= BIT15;
  if (addr & BIT6) address |= BIT14;
  if (addr & BIT7) address |= BIT13;
  if (addr & BIT8) address |= BIT8;
  if (addr & BIT9) address |= BIT7;
  if (addr & BIT10) address |= BIT5;
  if (addr & BIT11) address |= BIT6;
  if (addr & BIT12) address |= BIT12;
  if (addr & BIT13) address |= BIT10;
  if (addr & BIT14) address |= BIT11;
  if (addr & BIT15) address |= BIT9;
  if (addr & BIT16) address |= BIT20;
  if (addr & BIT17) address |= BIT22;
  if (addr & BIT18) address |= BIT19;
  if (addr & BIT19) address |= BIT16; // ?
  if (addr & BIT20) address |= BIT17; // ?
  if (addr & BIT21) address |= BIT18; // ?
  if (addr & BIT22) address |= BIT21; // ?
  if (addr & BIT23) address |= BIT23; // ?
  if (addr & BIT24) address |= BIT24; // ?
  if (addr & BIT25) address |= BIT25; // ?

  return address;
}

static uint32_t p_scr_address(uint32_t addr) // china pinout scramble
{
  uint32_t address = 0;

  if (addr & BIT0) address |= BIT4;
  if (addr & BIT1) address |= BIT3;
  if (addr & BIT2) address |= BIT2;
  if (addr & BIT3) address |= BIT1;
  if (addr & BIT4) address |= BIT0;
  if (addr & BIT5) address |= BIT10;
  if (addr & BIT6) address |= BIT11;
  if (addr & BIT7) address |= BIT9;
  if (addr & BIT8) address |= BIT8;
  if (addr & BIT9) address |= BIT15;
  if (addr & BIT10) address |= BIT13;
  if (addr & BIT11) address |= BIT14;
  if (addr & BIT12) address |= BIT12;
  if (addr & BIT13) address |= BIT7;
  if (addr & BIT14) address |= BIT6;
  if (addr & BIT15) address |= BIT5;
  if (addr & BIT16) address |= BIT19;
  if (addr & BIT17) address |= BIT20;
  if (addr & BIT18) address |= BIT21;
  if (addr & BIT19) address |= BIT18; // ?
  if (addr & BIT20) address |= BIT16; // ?
  if (addr & BIT21) address |= BIT22; // ?
  if (addr & BIT22) address |= BIT17; // ?
  if (addr & BIT23) address |= BIT23; // ?
  if (addr & BIT24) address |= BIT24; // ?
  if (addr & BIT25) address |= BIT25; // ?

  return address;
}
#endif

static uint32_t p_desc_data(uint32_t dat) // china pinout descramble
{
  uint32_t data = 0;

  if (dat & BIT0)  data |= BIT8;
  if (dat & BIT1)  data |= BIT10;
  if (dat & BIT2)  data |= BIT12;
  if (dat & BIT3)  data |= BIT14;
  if (dat & BIT4)  data |= BIT7;
  if (dat & BIT5)  data |= BIT5;
  if (dat & BIT6)  data |= BIT3;
  if (dat & BIT7)  data |= BIT1;
  if (dat & BIT8)  data |= BIT9;
  if (dat & BIT9)  data |= BIT11;
  if (dat & BIT10) data |= BIT13;
  if (dat & BIT11) data |= BIT15;
  if (dat & BIT12) data |= BIT6;
  if (dat & BIT13) data |= BIT4;
  if (dat & BIT14) data |= BIT2;
  if (dat & BIT15) data |= BIT0;

  return data;
}

static uint32_t p_scr_data(uint32_t dat) // china pinout descramble
{
  uint32_t data = 0;

  if (dat & BIT0)  data |= BIT15;
  if (dat & BIT1)  data |= BIT7;
  if (dat & BIT2)  data |= BIT14;
  if (dat & BIT3)  data |= BIT6;
  if (dat & BIT4)  data |= BIT13;
  if (dat & BIT5)  data |= BIT5;
  if (dat & BIT6)  data |= BIT12;
  if (dat & BIT7)  data |= BIT4;
  if (dat & BIT8)  data |= BIT0;
  if (dat & BIT9)  data |= BIT8;
  if (dat & BIT10) data |= BIT1;
  if (dat & BIT11) data |= BIT9;
  if (dat & BIT12) data |= BIT2;
  if (dat & BIT13) data |= BIT10;
  if (dat & BIT14) data |= BIT3;
  if (dat & BIT15) data |= BIT11;

  return data;
}

static inline void P_nCE(uint32_t st)
{
  GPIOD -> BSRR = st ? BIT0 : BIT0 << 16;
}

static inline void P_nOE(uint32_t st)
{
  GPIOD -> BSRR = st ? BIT1 : BIT1 << 16;
}

static inline void P_nWE(uint32_t st)
{
  GPIOD -> BSRR = st ? BIT3 : BIT3 << 16;
}

static inline void P_OUT(uint32_t out)
{
  GPIOA -> MODER &= ~(0xffff);
  GPIOC -> MODER &= ~(0xffff);
  if (out)
  {
	GPIOA -> MODER |= 0x5555;
    GPIOC -> MODER |= 0x5555;
  }
}

static inline void P_SetAddress(uint32_t addr)
{
  GPIOB -> ODR &= ~(0xffff);
  GPIOE -> ODR &= ~(0x03ff);
  GPIOB -> ODR |= addr & 0xffff;
  GPIOE -> ODR |= (addr >> 16) & 0x3ff;
}

static inline void P_SetData(uint32_t data)
{
  GPIOA -> ODR &= ~(0xff);
  GPIOC -> ODR &= ~(0xff);
  GPIOA -> ODR |= data & 0xff;
  GPIOC -> ODR |= (data >> 8) & 0xff;
}

static inline uint32_t P_GetData(void)
{
  uint32_t data = ((GPIOA -> IDR) & 0xff);
	data |= ((GPIOC -> IDR) & 0xff) << 8;

  return data;
}

void P_GPIO_Init(void)
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
  
  // PD0, PD1, PD3: CE# OE# WE#
  __HAL_RCC_GPIOD_CLK_ENABLE();
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_3;
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

  P_nCE(0);
  P_nOE(1);
  P_nWE(1);
}

uint32_t P_ReadData(uint32_t addr)
{
  uint32_t data;
  
  P_SetAddress(addr);
  P_nOE(0);
  Delay_cycles(50); // 100ns
  data = P_GetData();
  P_nOE(1);

  return data;
}

void P_WriteData(uint32_t addr, uint32_t data)
{
  P_SetAddress(addr);
  P_SetData(data);
  P_OUT(1);
  P_nWE(0);
  Delay_cycles(50); // 100ns
  P_nWE(1);
  P_OUT(0);
}

void P_ReadID(void)
{
  P_WriteData(0xAAA, 0xAAAA); // unlock 1
  P_WriteData(0x555, 0x5555); // unlock 2
  P_WriteData(0xAAA, 0x9090); // autoselect
  test = P_ReadData(0x0E << 1); // read ID
  P_WriteData(0x000, 0xF0F0); //  reset
}

void P_SectorErase(uint32_t addr)
{
  P_WriteData(0xAAA, 0xAAAA); // unlock 1
  P_WriteData(0x555, 0x5555); // unlock 2
  P_WriteData(0xAAA, 0x8080); // setup
  P_WriteData(0xAAA, 0xAAAA); // unlock 1
  P_WriteData(0x555, 0x5555); // unlock 2
  P_WriteData(addr,  0x3030); // sector erase
  do
  {
    test = P_ReadData(addr);
  }
  while ((test | 0x7F7F) != 0xFFFF);
}

uint32_t P_Read(void)
{
  uint32_t addr;
  uint32_t data;
  uint32_t d[2];

  addr = address++;
#ifdef DE_SCRAMBLE_ADDR_P
  addr = p_desc_address(addr);
#endif
  P_SetAddress(addr);
  P_nOE(0);
  Delay_cycles(50);
  d[0] = P_GetData();
  Delay_cycles(10);
  d[1] = P_GetData();
  while (d[0] != d[1])
  {
	d[0] = d[1];
    Delay_cycles(10);
    d[1] = P_GetData();
    error++;
  }
  P_nOE(1);
  data = d[0];
  data = p_desc_data(data);
  
  return data;
}

void P_Dump(void)
{
  uint32_t data = P_Read();

  buffer[buffer_pos++] = data;
  buffer[buffer_pos++] = data >> 8;
  if (buffer_pos >= BUFFER_SIZE) {
	buffer_pos = 0;
  }
}

void P_Prog(void)
{
  uint32_t addr, saddr, laddr;
  uint32_t data;
  uint32_t f;

  addr = address;
  address += 32;
#ifdef DE_SCRAMBLE_ADDR_P
  addr = p_scr_address(addr);
#endif
  saddr = addr & ~(SECTOR_SIZE-1);
  if ((addr & (SECTOR_SIZE-1)) == 0) {
    P_SectorErase(saddr);
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
    P_WriteData(0xAAA, 0xAAAA); // unlock 1
    P_WriteData(0x555, 0x5555); // unlock 2
    P_WriteData(saddr, 0x2525); // write buffer load
    P_WriteData(saddr, 0x1F1F); // count
    for (uint32_t i = 0; i < 32; i++)
    {
      data = buffer[buffer_pos++];
      data |= buffer[buffer_pos++] << 8;
      data = p_scr_data(data);
      laddr = addr + i;
      P_WriteData(laddr, data);
    }
    P_WriteData(saddr, 0x2929);  // write buffer to flash
    do
    {
      test = P_ReadData(laddr);
    }
    while ((test | 0x7F7F) != (data | 0x7F7F));
  }
  if (buffer_pos >= BUFFER_SIZE) {
	buffer_pos = 0;
  }
}

void P_Veri(void)
{
  uint32_t data = P_Read();
  uint32_t err = 0;
  
  if (buffer[buffer_pos++] != (data & 0xFF)) err++;
  if (buffer[buffer_pos++] != ((data >> 8) & 0xFF)) err++;
  
  if (err) error++;
  
  if (buffer_pos >= BUFFER_SIZE) {
	buffer_pos = 0;
  }
}

void P_Test(void)
{
  P_ReadID();
}
