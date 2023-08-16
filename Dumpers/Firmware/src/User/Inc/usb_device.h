#ifndef __USB_DEVICE__H__
#define __USB_DEVICE__H__

#ifdef __cplusplus
 extern "C" {
#endif

void BSP_USB_DEVICE_Init(void);
void USB_printf(const char *format, ...);

#ifdef __cplusplus
}
#endif

#endif /* __USB_DEVICE__H__ */
