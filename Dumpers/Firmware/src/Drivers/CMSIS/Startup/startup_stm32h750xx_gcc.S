/*
 * Startup File for gcc / STM32H750XX
 *
 * modified by Maximilian Rehkopf
 * based on startup_ARMCM7.S by ARM
 * see original license below
 */
/**************************************************************************//**
 * @file     startup_ARMCM7.S
 * @brief    CMSIS-Core(M) Device Startup File for Cortex-M7 Device
 * @version  V2.2.0
 * @date     26. May 2021
 ******************************************************************************/
/*
 * Copyright (c) 2009-2021 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

                .syntax  unified
                .arch    armv7e-m

                .section .vectors
                .align   2
                .globl   __Vectors
                .globl   __Vectors_End
                .globl   __Vectors_Size
__Vectors:
                .long    __StackTop                         /*     Top of Stack */
                .long    Reset_Handler                      /*     Reset Handler */
                .long    NMI_Handler                        /* -14 NMI Handler */
                .long    HardFault_Handler                  /* -13 Hard Fault Handler */
                .long    MemManage_Handler                  /* -12 MPU Fault Handler */
                .long    BusFault_Handler                   /* -11 Bus Fault Handler */
                .long    UsageFault_Handler                 /* -10 Usage Fault Handler */
                .long    0                                  /*     Reserved */
                .long    0                                  /*     Reserved */
                .long    0                                  /*     Reserved */
                .long    0                                  /*     Reserved */
                .long    SVC_Handler                        /*  -5 SVC Handler */
                .long    DebugMon_Handler                   /*  -4 Debug Monitor Handler */
                .long    0                                  /*     Reserved */
                .long    PendSV_Handler                     /*  -2 PendSV Handler */
                .long    SysTick_Handler                    /*  -1 SysTick Handler */

  /* External Interrupts */
                .long     WWDG_IRQHandler                   /* Window WatchDog */
                .long     PVD_AVD_IRQHandler                /* PVD/AVD through EXTI Line detection */
                .long     TAMP_STAMP_IRQHandler             /* Tamper and TimeStamps through the EXTI line */
                .long     RTC_WKUP_IRQHandler               /* RTC Wakeup through the EXTI line */
                .long     FLASH_IRQHandler                  /* FLASH */
                .long     RCC_IRQHandler                    /* RCC */
                .long     EXTI0_IRQHandler                  /* EXTI Line0 */
                .long     EXTI1_IRQHandler                  /* EXTI Line1 */
                .long     EXTI2_IRQHandler                  /* EXTI Line2 */
                .long     EXTI3_IRQHandler                  /* EXTI Line3 */
                .long     EXTI4_IRQHandler                  /* EXTI Line4 */
                .long     DMA1_Stream0_IRQHandler           /* DMA1 Stream 0 */
                .long     DMA1_Stream1_IRQHandler           /* DMA1 Stream 1 */
                .long     DMA1_Stream2_IRQHandler           /* DMA1 Stream 2 */
                .long     DMA1_Stream3_IRQHandler           /* DMA1 Stream 3 */
                .long     DMA1_Stream4_IRQHandler           /* DMA1 Stream 4 */
                .long     DMA1_Stream5_IRQHandler           /* DMA1 Stream 5 */
                .long     DMA1_Stream6_IRQHandler           /* DMA1 Stream 6 */
                .long     ADC_IRQHandler                    /* ADC1, ADC2 and ADC3s */
                .long     FDCAN1_IT0_IRQHandler             /* FDCAN1 interrupt line 0 */
                .long     FDCAN2_IT0_IRQHandler             /* FDCAN2 interrupt line 0 */
                .long     FDCAN1_IT1_IRQHandler             /* FDCAN1 interrupt line 1 */
                .long     FDCAN2_IT1_IRQHandler             /* FDCAN2 interrupt line 1 */
                .long     EXTI9_5_IRQHandler                /* External Line[9:5]s */
                .long     TIM1_BRK_IRQHandler               /* TIM1 Break interrupt */
                .long     TIM1_UP_IRQHandler                /* TIM1 Update interrupt */
                .long     TIM1_TRG_COM_IRQHandler           /* TIM1 Trigger and Commutation interrupt */
                .long     TIM1_CC_IRQHandler                /* TIM1 Capture Compare */
                .long     TIM2_IRQHandler                   /* TIM2 */
                .long     TIM3_IRQHandler                   /* TIM3 */
                .long     TIM4_IRQHandler                   /* TIM4 */
                .long     I2C1_EV_IRQHandler                /* I2C1 Event */
                .long     I2C1_ER_IRQHandler                /* I2C1 Error */
                .long     I2C2_EV_IRQHandler                /* I2C2 Event */
                .long     I2C2_ER_IRQHandler                /* I2C2 Error */
                .long     SPI1_IRQHandler                   /* SPI1 */
                .long     SPI2_IRQHandler                   /* SPI2 */
                .long     USART1_IRQHandler                 /* USART1 */
                .long     USART2_IRQHandler                 /* USART2 */
                .long     USART3_IRQHandler                 /* USART3 */
                .long     EXTI15_10_IRQHandler              /* External Line[15:10]s */
                .long     RTC_Alarm_IRQHandler              /* RTC Alarm (A and B) through EXTI Line */
                .long     0                                 /* Reserved */
                .long     TIM8_BRK_TIM12_IRQHandler         /* TIM8 Break and TIM12 */
                .long     TIM8_UP_TIM13_IRQHandler          /* TIM8 Update and TIM13 */
                .long     TIM8_TRG_COM_TIM14_IRQHandler     /* TIM8 Trigger and Commutation and TIM14 */
                .long     TIM8_CC_IRQHandler                /* TIM8 Capture Compare */
                .long     DMA1_Stream7_IRQHandler           /* DMA1 Stream7 */
                .long     FMC_IRQHandler                    /* FMC */
                .long     SDMMC1_IRQHandler                 /* SDMMC1 */
                .long     TIM5_IRQHandler                   /* TIM5 */
                .long     SPI3_IRQHandler                   /* SPI3 */
                .long     UART4_IRQHandler                  /* UART4 */
                .long     UART5_IRQHandler                  /* UART5 */
                .long     TIM6_DAC_IRQHandler               /* TIM6 and DAC1&2 underrun errors */
                .long     TIM7_IRQHandler                   /* TIM7 */
                .long     DMA2_Stream0_IRQHandler           /* DMA2 Stream 0 */
                .long     DMA2_Stream1_IRQHandler           /* DMA2 Stream 1 */
                .long     DMA2_Stream2_IRQHandler           /* DMA2 Stream 2 */
                .long     DMA2_Stream3_IRQHandler           /* DMA2 Stream 3 */
                .long     DMA2_Stream4_IRQHandler           /* DMA2 Stream 4 */
                .long     ETH_IRQHandler                    /* Ethernet */
                .long     ETH_WKUP_IRQHandler               /* Ethernet Wakeup through EXTI line */
                .long     FDCAN_CAL_IRQHandler              /* FDCAN calibration unit interrupt */
                .long     0                                 /* Reserved */
                .long     0                                 /* Reserved */
                .long     0                                 /* Reserved */
                .long     0                                 /* Reserved */
                .long     DMA2_Stream5_IRQHandler           /* DMA2 Stream 5 */
                .long     DMA2_Stream6_IRQHandler           /* DMA2 Stream 6 */
                .long     DMA2_Stream7_IRQHandler           /* DMA2 Stream 7 */
                .long     USART6_IRQHandler                 /* USART6 */
                .long     I2C3_EV_IRQHandler                /* I2C3 event */
                .long     I2C3_ER_IRQHandler                /* I2C3 error */
                .long     OTG_HS_EP1_OUT_IRQHandler         /* USB OTG HS End Point 1 Out */
                .long     OTG_HS_EP1_IN_IRQHandler          /* USB OTG HS End Point 1 In */
                .long     OTG_HS_WKUP_IRQHandler            /* USB OTG HS Wakeup through EXTI */
                .long     OTG_HS_IRQHandler                 /* USB OTG HS */
                .long     DCMI_IRQHandler                   /* DCMI */
                .long     CRYP_IRQHandler                   /* CRYP crypto */
                .long     HASH_RNG_IRQHandler               /* Hash and Rng */
                .long     FPU_IRQHandler                    /* FPU */
                .long     UART7_IRQHandler                  /* UART7 */
                .long     UART8_IRQHandler                  /* UART8 */
                .long     SPI4_IRQHandler                   /* SPI4 */
                .long     SPI5_IRQHandler                   /* SPI5 */
                .long     SPI6_IRQHandler                   /* SPI6 */
                .long     SAI1_IRQHandler                   /* SAI1 */
                .long     LTDC_IRQHandler                   /* LTDC */
                .long     LTDC_ER_IRQHandler                /* LTDC error */
                .long     DMA2D_IRQHandler                  /* DMA2D */
                .long     SAI2_IRQHandler                   /* SAI2 */
                .long     QUADSPI_IRQHandler                /* QUADSPI */
                .long     LPTIM1_IRQHandler                 /* LPTIM1 */
                .long     CEC_IRQHandler                    /* HDMI_CEC */
                .long     I2C4_EV_IRQHandler                /* I2C4 Event */
                .long     I2C4_ER_IRQHandler                /* I2C4 Error */
                .long     SPDIF_RX_IRQHandler               /* SPDIF_RX */
                .long     OTG_FS_EP1_OUT_IRQHandler         /* USB OTG FS End Point 1 Out */
                .long     OTG_FS_EP1_IN_IRQHandler          /* USB OTG FS End Point 1 In */
                .long     OTG_FS_WKUP_IRQHandler            /* USB OTG FS Wakeup through EXTI */
                .long     OTG_FS_IRQHandler                 /* USB OTG FS */
                .long     DMAMUX1_OVR_IRQHandler            /* DMAMUX1 Overrun interrupt */
                .long     HRTIM1_Master_IRQHandler          /* HRTIM Master Timer global Interrupts */
                .long     HRTIM1_TIMA_IRQHandler            /* HRTIM Timer A global Interrupt */
                .long     HRTIM1_TIMB_IRQHandler            /* HRTIM Timer B global Interrupt */
                .long     HRTIM1_TIMC_IRQHandler            /* HRTIM Timer C global Interrupt */
                .long     HRTIM1_TIMD_IRQHandler            /* HRTIM Timer D global Interrupt */
                .long     HRTIM1_TIME_IRQHandler            /* HRTIM Timer E global Interrupt */
                .long     HRTIM1_FLT_IRQHandler             /* HRTIM Fault global Interrupt */
                .long     DFSDM1_FLT0_IRQHandler            /* DFSDM Filter0 Interrupt */
                .long     DFSDM1_FLT1_IRQHandler            /* DFSDM Filter1 Interrupt */
                .long     DFSDM1_FLT2_IRQHandler            /* DFSDM Filter2 Interrupt */
                .long     DFSDM1_FLT3_IRQHandler            /* DFSDM Filter3 Interrupt */
                .long     SAI3_IRQHandler                   /* SAI3 global Interrupt */
                .long     SWPMI1_IRQHandler                 /* Serial Wire Interface 1 global interrupt */
                .long     TIM15_IRQHandler                  /* TIM15 global Interrupt */
                .long     TIM16_IRQHandler                  /* TIM16 global Interrupt */
                .long     TIM17_IRQHandler                  /* TIM17 global Interrupt */
                .long     MDIOS_WKUP_IRQHandler             /* MDIOS Wakeup  Interrupt */
                .long     MDIOS_IRQHandler                  /* MDIOS global Interrupt */
                .long     JPEG_IRQHandler                   /* JPEG global Interrupt */
                .long     MDMA_IRQHandler                   /* MDMA global Interrupt */
                .long     0                                 /* Reserved */
                .long     SDMMC2_IRQHandler                 /* SDMMC2 global Interrupt */
                .long     HSEM1_IRQHandler                  /* HSEM1 global Interrupt */
                .long     0                                 /* Reserved */
                .long     ADC3_IRQHandler                   /* ADC3 global Interrupt */
                .long     DMAMUX2_OVR_IRQHandler            /* DMAMUX Overrun interrupt */
                .long     BDMA_Channel0_IRQHandler          /* BDMA Channel 0 global Interrupt */
                .long     BDMA_Channel1_IRQHandler          /* BDMA Channel 1 global Interrupt */
                .long     BDMA_Channel2_IRQHandler          /* BDMA Channel 2 global Interrupt */
                .long     BDMA_Channel3_IRQHandler          /* BDMA Channel 3 global Interrupt */
                .long     BDMA_Channel4_IRQHandler          /* BDMA Channel 4 global Interrupt */
                .long     BDMA_Channel5_IRQHandler          /* BDMA Channel 5 global Interrupt */
                .long     BDMA_Channel6_IRQHandler          /* BDMA Channel 6 global Interrupt */
                .long     BDMA_Channel7_IRQHandler          /* BDMA Channel 7 global Interrupt */
                .long     COMP1_IRQHandler                  /* COMP1 global Interrupt */
                .long     LPTIM2_IRQHandler                 /* LP TIM2 global interrupt */
                .long     LPTIM3_IRQHandler                 /* LP TIM3 global interrupt */
                .long     LPTIM4_IRQHandler                 /* LP TIM4 global interrupt */
                .long     LPTIM5_IRQHandler                 /* LP TIM5 global interrupt */
                .long     LPUART1_IRQHandler                /* LP UART1 interrupt */
                .long     0                                 /* Reserved */
                .long     CRS_IRQHandler                    /* Clock Recovery Global Interrupt */
                .long     ECC_IRQHandler                    /* ECC diagnostic Global Interrupt */
                .long     SAI4_IRQHandler                   /* SAI4 global interrupt */
                .long     0                                 /* Reserved */
                .long     0                                 /* Reserved */
                .long     WAKEUP_PIN_IRQHandler             /* Interrupt for all 6 wake-up pins */

__Vectors_End:
                .equ     __Vectors_Size, __Vectors_End - __Vectors
                .size    __Vectors, . - __Vectors


                .thumb
                .section .text
                .align   2

                .thumb_func
                .type    Reset_Handler, %function
                .globl   Reset_Handler
                .fnstart
Reset_Handler:
                bl       SystemInit

                ldr      r4, =__copy_table_start__
                ldr      r5, =__copy_table_end__

.L_loop0:
                cmp      r4, r5
                bge      .L_loop0_done
                ldr      r1, [r4]                /* source address */
                ldr      r2, [r4, #4]            /* destination address */
                ldr      r3, [r4, #8]            /* word count */
                lsls     r3, r3, #2              /* byte count */

.L_loop0_0:
                subs     r3, #4                  /* decrement byte count */
                ittt     ge
                ldrge    r0, [r1, r3]
                strge    r0, [r2, r3]
                bge      .L_loop0_0

                adds     r4, #12
                b        .L_loop0
.L_loop0_done:

                ldr      r3, =__zero_table_start__
                ldr      r4, =__zero_table_end__

.L_loop2:
                cmp      r3, r4
                bge      .L_loop2_done
                ldr      r1, [r3]                /* destination address */
                ldr      r2, [r3, #4]            /* word count */
                lsls     r2, r2, #2              /* byte count */
                movs     r0, 0

.L_loop2_0:
                subs     r2, #4                  /* decrement byte count */
                itt      ge
                strge    r0, [r1, r2]
                bge      .L_loop2_0

                adds     r3, #8
                b        .L_loop2
.L_loop2_done:
                bl       main

                .fnend
                .size    Reset_Handler, . - Reset_Handler

/* The default macro is not used for HardFault_Handler
 * because this results in a poor debug illusion.
 */
                .thumb_func
                .type    HardFault_Handler, %function
                .weak    HardFault_Handler
                .fnstart
HardFault_Handler:
                b        .
                .fnend
                .size    HardFault_Handler, . - HardFault_Handler

                .thumb_func
                .type    Default_Handler, %function
                .weak    Default_Handler
                .fnstart
Default_Handler:
                b        .
                .fnend
                .size    Default_Handler, . - Default_Handler

/* Macro to define default exception/interrupt handlers.
 * Default handler are weak symbols with an endless loop.
 * They can be overwritten by real handlers.
 */
                .macro   Set_Default_Handler  Handler_Name
                .weak    \Handler_Name
                .set     \Handler_Name, Default_Handler
                .endm


/* Default exception/interrupt handler */

                Set_Default_Handler  NMI_Handler
                Set_Default_Handler  MemManage_Handler
                Set_Default_Handler  BusFault_Handler
                Set_Default_Handler  UsageFault_Handler
                Set_Default_Handler  SVC_Handler
                Set_Default_Handler  DebugMon_Handler
                Set_Default_Handler  PendSV_Handler
                Set_Default_Handler  SysTick_Handler

                Set_Default_Handler  WWDG_IRQHandler
                Set_Default_Handler  PVD_AVD_IRQHandler
                Set_Default_Handler  TAMP_STAMP_IRQHandler
                Set_Default_Handler  RTC_WKUP_IRQHandler
                Set_Default_Handler  FLASH_IRQHandler
                Set_Default_Handler  RCC_IRQHandler
                Set_Default_Handler  EXTI0_IRQHandler
                Set_Default_Handler  EXTI1_IRQHandler
                Set_Default_Handler  EXTI2_IRQHandler
                Set_Default_Handler  EXTI3_IRQHandler
                Set_Default_Handler  EXTI4_IRQHandler
                Set_Default_Handler  DMA1_Stream0_IRQHandler
                Set_Default_Handler  DMA1_Stream1_IRQHandler
                Set_Default_Handler  DMA1_Stream2_IRQHandler
                Set_Default_Handler  DMA1_Stream3_IRQHandler
                Set_Default_Handler  DMA1_Stream4_IRQHandler
                Set_Default_Handler  DMA1_Stream5_IRQHandler
                Set_Default_Handler  DMA1_Stream6_IRQHandler
                Set_Default_Handler  ADC_IRQHandler
                Set_Default_Handler  FDCAN1_IT0_IRQHandler
                Set_Default_Handler  FDCAN2_IT0_IRQHandler
                Set_Default_Handler  FDCAN1_IT1_IRQHandler
                Set_Default_Handler  FDCAN2_IT1_IRQHandler
                Set_Default_Handler  EXTI9_5_IRQHandler
                Set_Default_Handler  TIM1_BRK_IRQHandler
                Set_Default_Handler  TIM1_UP_IRQHandler
                Set_Default_Handler  TIM1_TRG_COM_IRQHandler
                Set_Default_Handler  TIM1_CC_IRQHandler
                Set_Default_Handler  TIM2_IRQHandler
                Set_Default_Handler  TIM3_IRQHandler
                Set_Default_Handler  TIM4_IRQHandler
                Set_Default_Handler  I2C1_EV_IRQHandler
                Set_Default_Handler  I2C1_ER_IRQHandler
                Set_Default_Handler  I2C2_EV_IRQHandler
                Set_Default_Handler  I2C2_ER_IRQHandler
                Set_Default_Handler  SPI1_IRQHandler
                Set_Default_Handler  SPI2_IRQHandler
                Set_Default_Handler  USART1_IRQHandler
                Set_Default_Handler  USART2_IRQHandler
                Set_Default_Handler  USART3_IRQHandler
                Set_Default_Handler  EXTI15_10_IRQHandler
                Set_Default_Handler  RTC_Alarm_IRQHandler
                Set_Default_Handler  TIM8_BRK_TIM12_IRQHandler
                Set_Default_Handler  TIM8_UP_TIM13_IRQHandler
                Set_Default_Handler  TIM8_TRG_COM_TIM14_IRQHandler
                Set_Default_Handler  TIM8_CC_IRQHandler
                Set_Default_Handler  DMA1_Stream7_IRQHandler
                Set_Default_Handler  FMC_IRQHandler
                Set_Default_Handler  SDMMC1_IRQHandler
                Set_Default_Handler  TIM5_IRQHandler
                Set_Default_Handler  SPI3_IRQHandler
                Set_Default_Handler  UART4_IRQHandler
                Set_Default_Handler  UART5_IRQHandler
                Set_Default_Handler  TIM6_DAC_IRQHandler
                Set_Default_Handler  TIM7_IRQHandler
                Set_Default_Handler  DMA2_Stream0_IRQHandler
                Set_Default_Handler  DMA2_Stream1_IRQHandler
                Set_Default_Handler  DMA2_Stream2_IRQHandler
                Set_Default_Handler  DMA2_Stream3_IRQHandler
                Set_Default_Handler  DMA2_Stream4_IRQHandler
                Set_Default_Handler  ETH_IRQHandler
                Set_Default_Handler  ETH_WKUP_IRQHandler
                Set_Default_Handler  FDCAN_CAL_IRQHandler
                Set_Default_Handler  DMA2_Stream5_IRQHandler
                Set_Default_Handler  DMA2_Stream6_IRQHandler
                Set_Default_Handler  DMA2_Stream7_IRQHandler
                Set_Default_Handler  USART6_IRQHandler
                Set_Default_Handler  I2C3_EV_IRQHandler
                Set_Default_Handler  I2C3_ER_IRQHandler
                Set_Default_Handler  OTG_HS_EP1_OUT_IRQHandler
                Set_Default_Handler  OTG_HS_EP1_IN_IRQHandler
                Set_Default_Handler  OTG_HS_WKUP_IRQHandler
                Set_Default_Handler  OTG_HS_IRQHandler
                Set_Default_Handler  DCMI_IRQHandler
                Set_Default_Handler  CRYP_IRQHandler
                Set_Default_Handler  HASH_RNG_IRQHandler
                Set_Default_Handler  FPU_IRQHandler
                Set_Default_Handler  UART7_IRQHandler
                Set_Default_Handler  UART8_IRQHandler
                Set_Default_Handler  SPI4_IRQHandler
                Set_Default_Handler  SPI5_IRQHandler
                Set_Default_Handler  SPI6_IRQHandler
                Set_Default_Handler  SAI1_IRQHandler
                Set_Default_Handler  LTDC_IRQHandler
                Set_Default_Handler  LTDC_ER_IRQHandler
                Set_Default_Handler  DMA2D_IRQHandler
                Set_Default_Handler  SAI2_IRQHandler
                Set_Default_Handler  QUADSPI_IRQHandler
                Set_Default_Handler  LPTIM1_IRQHandler
                Set_Default_Handler  CEC_IRQHandler
                Set_Default_Handler  I2C4_EV_IRQHandler
                Set_Default_Handler  I2C4_ER_IRQHandler
                Set_Default_Handler  SPDIF_RX_IRQHandler
                Set_Default_Handler  OTG_FS_EP1_OUT_IRQHandler
                Set_Default_Handler  OTG_FS_EP1_IN_IRQHandler
                Set_Default_Handler  OTG_FS_WKUP_IRQHandler
                Set_Default_Handler  OTG_FS_IRQHandler
                Set_Default_Handler  DMAMUX1_OVR_IRQHandler
                Set_Default_Handler  HRTIM1_Master_IRQHandler
                Set_Default_Handler  HRTIM1_TIMA_IRQHandler
                Set_Default_Handler  HRTIM1_TIMB_IRQHandler
                Set_Default_Handler  HRTIM1_TIMC_IRQHandler
                Set_Default_Handler  HRTIM1_TIMD_IRQHandler
                Set_Default_Handler  HRTIM1_TIME_IRQHandler
                Set_Default_Handler  HRTIM1_FLT_IRQHandler
                Set_Default_Handler  DFSDM1_FLT0_IRQHandler
                Set_Default_Handler  DFSDM1_FLT1_IRQHandler
                Set_Default_Handler  DFSDM1_FLT2_IRQHandler
                Set_Default_Handler  DFSDM1_FLT3_IRQHandler
                Set_Default_Handler  SAI3_IRQHandler
                Set_Default_Handler  SWPMI1_IRQHandler
                Set_Default_Handler  TIM15_IRQHandler
                Set_Default_Handler  TIM16_IRQHandler
                Set_Default_Handler  TIM17_IRQHandler
                Set_Default_Handler  MDIOS_WKUP_IRQHandler
                Set_Default_Handler  MDIOS_IRQHandler
                Set_Default_Handler  JPEG_IRQHandler
                Set_Default_Handler  MDMA_IRQHandler
                Set_Default_Handler  SDMMC2_IRQHandler
                Set_Default_Handler  HSEM1_IRQHandler
                Set_Default_Handler  ADC3_IRQHandler
                Set_Default_Handler  DMAMUX2_OVR_IRQHandler
                Set_Default_Handler  BDMA_Channel0_IRQHandler
                Set_Default_Handler  BDMA_Channel1_IRQHandler
                Set_Default_Handler  BDMA_Channel2_IRQHandler
                Set_Default_Handler  BDMA_Channel3_IRQHandler
                Set_Default_Handler  BDMA_Channel4_IRQHandler
                Set_Default_Handler  BDMA_Channel5_IRQHandler
                Set_Default_Handler  BDMA_Channel6_IRQHandler
                Set_Default_Handler  BDMA_Channel7_IRQHandler
                Set_Default_Handler  COMP1_IRQHandler
                Set_Default_Handler  LPTIM2_IRQHandler
                Set_Default_Handler  LPTIM3_IRQHandler
                Set_Default_Handler  LPTIM4_IRQHandler
                Set_Default_Handler  LPTIM5_IRQHandler
                Set_Default_Handler  LPUART1_IRQHandler
                Set_Default_Handler  CRS_IRQHandler
                Set_Default_Handler  ECC_IRQHandler
                Set_Default_Handler  SAI4_IRQHandler
                Set_Default_Handler  WAKEUP_PIN_IRQHandler

                .end
