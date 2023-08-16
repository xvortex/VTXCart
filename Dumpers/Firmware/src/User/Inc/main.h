#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "defines.h"

#include "stm32h7xx_hal.h"
#include "stm32h7xx_it.h"

#include "weact_bsp.h"
#include "weact_sd.h"

#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_cdc.h"
#include "usbd_cdc_if.h"
#include "usb_device.h"
#include "uart.h"

#include "ff.h"
#include "ff_gen_drv.h"
#include "sd_diskio.h"

#include "fatfs.h"

#include "st7735.h"
#include "lcd.h"

#include "tools.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

#include "variables.h"

#include "P.h"
#include "SM.h"
#include "CV.h"

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
