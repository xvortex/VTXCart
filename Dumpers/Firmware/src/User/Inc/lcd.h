#ifndef __LCD_H
#define __LCD_H

#ifdef __cplusplus
 extern "C" {
#endif

extern ST7735_Ctx_t ST7735Ctx;

void LCD_Init(void);
void LCD_Clear(void);
void LCD_Draw(void);
void LCD_ShowChar(uint16_t x, uint16_t y, uint8_t num);
void LCD_ShowString(uint16_t x, uint16_t y, uint8_t *p);

void doLCD(void);

#ifdef __cplusplus
}
#endif

#endif /* __LCD_H */
