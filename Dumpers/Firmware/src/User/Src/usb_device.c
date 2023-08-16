#include "main.h"
#include "usb_device.h"

extern uint8_t UserTxBufferHS[APP_TX_DATA_SIZE];

void BSP_USB_DEVICE_Init(void)
{
  /* Init Device Library, add supported class and start the library. */
  Error_Handler(USBD_Init(&hUsbDeviceHS, &HS_Desc, DEVICE_HS) != USBD_OK);
  Error_Handler(USBD_RegisterClass(&hUsbDeviceHS, &USBD_CDC) != USBD_OK);
  Error_Handler(USBD_CDC_RegisterInterface(&hUsbDeviceHS, &USBD_Interface_fops_HS) != USBD_OK);
  Error_Handler(USBD_Start(&hUsbDeviceHS) != USBD_OK);

  /* USER CODE BEGIN USB_DEVICE_Init_PostTreatment */
  HAL_PWREx_EnableUSBVoltageDetector();
}

void USB_printf(const char *format, ...)
{
  va_list args;
  uint32_t length;
  uint32_t timer;

  timer = HAL_GetTick();
  va_start(args, format);
  length = vsnprintf((char *)UserTxBufferHS, APP_TX_DATA_SIZE, (char *)format, args);
  va_end(args);
  UserTxBufferHS[length++] = 0x0A;
  while(HAL_GetTick() - timer < 10)
  {
    if (CDC_Transmit_HS(UserTxBufferHS, length) == USBD_OK)
    {
      return;
    }
  }
}
