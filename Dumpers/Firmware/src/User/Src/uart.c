#include "main.h"
#include "uart.h"

void UART_printf(const char *format, ...)
{
  va_list args;
  uint32_t length;

  va_start(args, format);
  length = vsnprintf((char *)&UARTTxBuffer[UARTTxBuffer_head], UART_BUFFER_SIZE, (char *)format, args);
  va_end(args);
  UARTTxBuffer[UARTTxBuffer_head + length++] = 0x0A;
  if (UARTTxBuffer_head == UARTTxBuffer_tail) {
    HAL_UART_Transmit_IT(&huart3, (uint8_t *)&UARTTxBuffer[UARTTxBuffer_head], length);
	UARTTxBuffer_len = length;
  };
  UARTTxBuffer_head += length;
  Error_Handler(UARTTxBuffer_head >= UART_BUFFER_SIZE); // Oops
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  uint32_t length;
  
  UARTTxBuffer_tail += UARTTxBuffer_len;
  if (UARTTxBuffer_head > UARTTxBuffer_tail) {
	length = UARTTxBuffer_head - UARTTxBuffer_tail;
    HAL_UART_Transmit_IT(&huart3, (uint8_t *)&UARTTxBuffer[UARTTxBuffer_tail], length);
	UARTTxBuffer_len = length;
  } else {
    UARTTxBuffer_head = 0;
    UARTTxBuffer_tail = 0;
  }
}
