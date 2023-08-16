#include "main.h"
#include "CV.h"

// F0095H0 (8xMT28GU01G)
#define SECTOR_SIZE 0x20000
#define REGION_SIZE 512

// chip: 1, 2, 3, 4
// half: 1, 2

#ifdef DE_SCRAMBLE_ADDR
static uint32_t cv_desc_address(uint32_t addr) // china pinout descramble
{
  uint32_t address = 0;

  address = addr & 0xFFFFFFF0;
  if (addr & BIT0) address |= BIT3;
  if (addr & BIT1) address |= BIT2;
  if (addr & BIT2) address |= BIT1;
  if (addr & BIT3) address |= BIT0;

  return address;
}

static uint32_t cv_scr_address(uint32_t addr) // china pinout scramble
{
  uint32_t address = 0;

  address = addr & 0xFFFFFFF0;
  if (addr & BIT0) address |= BIT3;
  if (addr & BIT1) address |= BIT2;
  if (addr & BIT2) address |= BIT1;
  if (addr & BIT3) address |= BIT0;

  return address;
}
#endif

static uint32_t cv_desc_data(uint32_t dat) // china pinout descramble
{
  uint32_t data = 0;

  if (dat & BIT0)  data |= BIT12;
  if (dat & BIT1)  data |= BIT9;
  if (dat & BIT2)  data |= BIT8;
  if (dat & BIT3)  data |= BIT2;
  if (dat & BIT4)  data |= BIT14;
  if (dat & BIT5)  data |= BIT0;
  if (dat & BIT6)  data |= BIT15;
  if (dat & BIT7)  data |= BIT4;
  if (dat & BIT8)  data |= BIT1;
  if (dat & BIT9)  data |= BIT5;
  if (dat & BIT10) data |= BIT13;
  if (dat & BIT11) data |= BIT3;
  if (dat & BIT12) data |= BIT7;
  if (dat & BIT13) data |= BIT6;
  if (dat & BIT14) data |= BIT11;
  if (dat & BIT15) data |= BIT10;
  if (dat & BIT16) data |= BIT28;
  if (dat & BIT17) data |= BIT25;
  if (dat & BIT18) data |= BIT24;
  if (dat & BIT19) data |= BIT18;
  if (dat & BIT20) data |= BIT30;
  if (dat & BIT21) data |= BIT16;
  if (dat & BIT22) data |= BIT31;
  if (dat & BIT23) data |= BIT20;
  if (dat & BIT24) data |= BIT17;
  if (dat & BIT25) data |= BIT21;
  if (dat & BIT26) data |= BIT29;
  if (dat & BIT27) data |= BIT19;
  if (dat & BIT28) data |= BIT23;
  if (dat & BIT29) data |= BIT22;
  if (dat & BIT30) data |= BIT27;
  if (dat & BIT31) data |= BIT26;
  
  return data;
}

static uint32_t cv_scr_data(uint32_t dat) // china pinout scramble
{
  uint32_t data = 0;

  if (dat & BIT0)  data |= BIT5;
  if (dat & BIT1)  data |= BIT8;
  if (dat & BIT2)  data |= BIT3;
  if (dat & BIT3)  data |= BIT11;
  if (dat & BIT4)  data |= BIT7;
  if (dat & BIT5)  data |= BIT9;
  if (dat & BIT6)  data |= BIT13;
  if (dat & BIT7)  data |= BIT12;
  if (dat & BIT8)  data |= BIT2;
  if (dat & BIT9)  data |= BIT1;
  if (dat & BIT10) data |= BIT15;
  if (dat & BIT11) data |= BIT14;
  if (dat & BIT12) data |= BIT0;
  if (dat & BIT13) data |= BIT10;
  if (dat & BIT14) data |= BIT4;
  if (dat & BIT15) data |= BIT6;
  if (dat & BIT16) data |= BIT21;
  if (dat & BIT17) data |= BIT24;
  if (dat & BIT18) data |= BIT19;
  if (dat & BIT19) data |= BIT27;
  if (dat & BIT20) data |= BIT23;
  if (dat & BIT21) data |= BIT25;
  if (dat & BIT22) data |= BIT29;
  if (dat & BIT23) data |= BIT28;
  if (dat & BIT24) data |= BIT18;
  if (dat & BIT25) data |= BIT17;
  if (dat & BIT26) data |= BIT31;
  if (dat & BIT27) data |= BIT30;
  if (dat & BIT28) data |= BIT16;
  if (dat & BIT29) data |= BIT26;
  if (dat & BIT30) data |= BIT20;
  if (dat & BIT31) data |= BIT22;
  
  return data;
}

static inline void CV_nCE(uint32_t st)
{
  GPIOD -> BSRR = (st & BIT0) ? BIT0 : BIT0 << 16;
  GPIOD -> BSRR = (st & BIT1) ? BIT1 : BIT1 << 16;
  GPIOD -> BSRR = (st & BIT2) ? BIT3 : BIT3 << 16;
  GPIOD -> BSRR = (st & BIT3) ? BIT4 : BIT4 << 16;
}

static inline void CV_nOE(uint32_t st)
{
  GPIOD -> BSRR = (st & BIT0) ? BIT5 : BIT5 << 16;
  GPIOD -> BSRR = (st & BIT1) ? BIT6 : BIT6 << 16;
  GPIOD -> BSRR = (st & BIT2) ? BIT7 : BIT7 << 16;
  GPIOD -> BSRR = (st & BIT3) ? BIT8 : BIT8 << 16;
}

static inline void CV_nRP(uint32_t st)
{
  GPIOD -> BSRR = st ? BIT9 : BIT9 << 16;
}

static inline void CV_nWE(uint32_t st)
{
  GPIOD -> BSRR = st ? BIT10 : BIT10 << 16;
}

static inline void CV_nWP(uint32_t st)
{
  GPIOD -> BSRR = st ? BIT11 : BIT11 << 16;
}

static inline void CV_OUT(uint32_t out)
{
  GPIOA -> MODER &= ~(0xffff);
  GPIOC -> MODER &= ~(0xffff);
  if (out)
  {
	GPIOA -> MODER |= 0x5555;
    GPIOC -> MODER |= 0x5555;
  }
}

static inline void CV_SetAddress(uint32_t addr)
{
  GPIOB -> ODR &= ~(0xffff);
  GPIOE -> ODR &= ~(0x03ff);
  GPIOB -> ODR |= addr & 0xffff;
  GPIOE -> ODR |= (addr >> 16) & 0x3ff;
  GPIOE -> BSRR = (addr & BIT26) ? BIT15 : BIT15 << 16;
}

static inline uint32_t CV_GetData(void)
{
  uint32_t data = ((GPIOA -> IDR) & 0xff);
	data |= ((GPIOC -> IDR) & 0xff) << 8;

  return data;
}

static inline void CV_SetData(uint32_t data)
{
  GPIOA -> ODR &= ~(0xff);
  GPIOC -> ODR &= ~(0xff);
  GPIOA -> ODR |= data & 0xff;
  GPIOC -> ODR |= (data >> 8) & 0xff;
}

void CV_GPIO_Init(void)
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
  
  // PD0-PD1, PD3-PD11: CE1#, CE2#, CE3#, CE4#, OE1#, OE2#, OE3#, OE4#, RP#, WE#, WP#
  __HAL_RCC_GPIOD_CLK_ENABLE();
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|
	                    GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
  
  // PE0-PE9,PE15: A16-A26
  __HAL_RCC_GPIOE_CLK_ENABLE();
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|
	                    GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  CV_nCE(0x0F);
  CV_nOE(0x0F);
  CV_nRP(1);
  CV_nWE(1);
  CV_nWP(1);
}

uint32_t CV_ADDR2ST(uint32_t half, uint32_t addr)
{
  uint32_t st = 0x0F;
  if (half & BIT0)
  {
	if (addr & BIT27) st &= ~BIT1; else st &= ~BIT0;
  }
  if (half & BIT1)
  {
	if (addr & BIT27) st &= ~BIT3; else st &= ~BIT2;
  }
  return st;
}

uint32_t CV_ReadData(uint32_t half, uint32_t addr)
{
  uint32_t data;
  
  CV_SetAddress(addr);
  CV_nCE(CV_ADDR2ST(half, addr));
  CV_nOE(CV_ADDR2ST(half, addr));
  Delay_cycles(50); // 100ns
  data = CV_GetData();
  CV_nOE(0x0F);
  CV_nCE(0x0F);

  return data;
}

void CV_WriteData(uint32_t half, uint32_t addr, uint32_t data)
{
  CV_SetAddress(addr);
  CV_SetData(data);
  CV_nCE(CV_ADDR2ST(half, addr));
  CV_nOE(0x0F);
  CV_OUT(1);
  CV_nWE(0);
  Delay_cycles(50); // 100ns
  CV_nWE(1);
  CV_OUT(0);
  CV_nCE(0x0F);
}

void CV_ReadID(uint32_t chip)
{
  uint32_t half = chip & 0x03;
  uint32_t addr = 0;
  
  if (chip > 2)
  {
	chip -= 2;
	addr = BIT27;
  }
  CV_WriteData(half, addr, 0x90); // read ID
  test = CV_ReadData(half, addr + 0x01); // ID reg.
  CV_WriteData(half, addr, 0xFF); // exit
}

void CV_SectorErase(uint32_t addr)
{
  uint32_t st1, st2;
  
  CV_WriteData(3, 0,    0x50); // clear status
  CV_WriteData(3, addr, 0x60); // block unlock
  CV_WriteData(3, addr, 0xD0); // unlock confirm
  CV_WriteData(3, addr, 0x20); // block erase
  CV_WriteData(3, addr, 0xD0); // erase confirm
  do
  {
    st1 = CV_ReadData(1, addr); // read ststus 1
    st2 = CV_ReadData(2, addr); // read ststus 2
  }
  while (((st1 & BIT7) == 0) || ((st2 & BIT7) == 0));
  if ((st1 != 0x80) || (st2 != 0x80)) error++;
  CV_WriteData(3, addr, 0xFF); // exit
}

uint32_t CV_Read(void)
{
  uint32_t addr;
  uint32_t data;
  uint32_t data1;
  uint32_t data2;
  uint32_t d[2];
  
  if (address == 0) CV_nCE(0x00);

  addr = address++;
#ifdef DE_SCRAMBLE_ADDR
  addr = cv_desc_address(addr);
#endif
  CV_SetAddress(addr);

  CV_nOE(CV_ADDR2ST (1, addr));
  Delay_cycles(50);
  d[0] = CV_GetData();
  Delay_cycles(10);
  d[1] = CV_GetData();

  while (d[0] != d[1])
  {
	d[0] = d[1];
    Delay_cycles(10);
    d[1] = CV_GetData();
    error++;
  }
  CV_nOE(0x0F);
  data1 = d[0];

  CV_nOE(CV_ADDR2ST (2, addr));
  Delay_cycles(50);
  d[0] = CV_GetData();
  Delay_cycles(10);
  d[1] = CV_GetData();
  
  while (d[0] != d[1])
  {
	d[0] = d[1];
    Delay_cycles(10);
    d[1] = CV_GetData();
    error++;
  }
  CV_nOE(0x0F);
  data2 = d[0];
  
  data = (data2 << 16) + data1;
  data = cv_desc_data (data);
  
  return data;
}

void CV_Dump(void)
{
  uint32_t data = CV_Read ();

  buffer[buffer_pos++] = data;
  buffer[buffer_pos++] = data >> 8;
  buffer[buffer_pos++] = data >> 16;
  buffer[buffer_pos++] = data >> 24;

  if (buffer_pos >= BUFFER_SIZE) {
	buffer_pos = 0;
  }
}

void CV_Prog(void)
{
  uint32_t addr;
  uint32_t data, st1, st2;
  uint32_t f;

  addr = address;
  address += REGION_SIZE;
#ifdef DE_SCRAMBLE_ADDR
  addr = cv_scr_address(addr);
#endif
  if ((addr & (SECTOR_SIZE-1)) == 0) {
    CV_SectorErase(addr);
  }
  f = 1;
  for (uint32_t i = 0; i < (REGION_SIZE * 4); i++)
  {
	if (buffer[buffer_pos + i] != 0xFF)
	{
	  f = 0;
	  break;
	}
  }
  if (f)
  {
	buffer_pos += REGION_SIZE * 4;
  }
  else
  {
    CV_WriteData(3, 0,    0x50); // clear status
    CV_WriteData(3, addr, 0xE9); // buffered program setup
    CV_WriteData(3, addr, REGION_SIZE-1);
    for (uint32_t i = 0; i < REGION_SIZE; i++)
    {
      data = buffer[buffer_pos++];
      data |= buffer[buffer_pos++] << 8;
      data |= buffer[buffer_pos++] << 16;
      data |= buffer[buffer_pos++] << 24;
      data = cv_scr_data(data);
      CV_WriteData(1, addr + i, data);
      CV_WriteData(2, addr + i, data >> 16);
    }
    CV_WriteData(3, addr, 0xD0); // buffered program confirm
    do
    {
      st1 = CV_ReadData(1, addr); // read ststus 1
      st2 = CV_ReadData(2, addr); // read ststus 2
    }
    while (((st1 & BIT7) == 0) || ((st2 & BIT7) == 0));
    if ((st1 != 0x80) || (st2 != 0x80)) error++;
    CV_WriteData(3, addr, 0xFF); // exit
  }
  if (buffer_pos >= BUFFER_SIZE) {
	buffer_pos = 0;
  }
}

void CV_Veri(void)
{
  uint32_t data = CV_Read ();
  uint32_t err = 0;

  if (buffer[buffer_pos++] != (data & 0xFF)) err++;
  if (buffer[buffer_pos++] != ((data >> 8) & 0xFF)) err++;
  if (buffer[buffer_pos++] != ((data >> 16) & 0xFF)) err++;
  if (buffer[buffer_pos++] != ((data >> 24) & 0xFF)) err++;
  
  if (err) error++;

  if (buffer_pos >= BUFFER_SIZE) {
	buffer_pos = 0;
  }
}

void CV_Test(void)
{
  CV_ReadID(1);
}
