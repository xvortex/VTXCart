#ifndef __DEFINES_H
#define __DEFINES_H

#ifdef __cplusplus
 extern "C" {
#endif

// de/scramble address
//#define DE_SCRAMBLE_ADDR_P

#define CHIP_NAME_P     "P-ROM"
#define DUMP_FILENAME_P "prom.bin"
#define PROG_FILENAME_P "prom"
#define END_ADDRESS_P   (0x8000000 / 2) // 128 meg

#define CHIP_NAME_S     "S-ROM"
#define DUMP_FILENAME_S "srom.bin"
#define PROG_FILENAME_S "srom"
//#define END_ADDRESS_S   (0x2000000 / 2) // 32 meg
#define END_ADDRESS_S   (0x4000000 / 2) // 64 meg

#define CHIP_NAME_M     "M-ROM"
#define DUMP_FILENAME_M "mrom.bin"
#define PROG_FILENAME_M "mrom"
#define END_ADDRESS_M   (0x4000000 / 2) // 64 meg

#define CHIP_NAME_C     "C-ROM"
#define DUMP_FILENAME_C "crom.bin"
#define PROG_FILENAME_C "crom"
#define END_ADDRESS_C   (0x40000000 / 4) // 1 gig

#define CHIP_NAME_V     "V-ROM"
#define DUMP_FILENAME_V "vrom.bin"
#define PROG_FILENAME_V "vrom"
#define END_ADDRESS_V   (0x40000000 / 4) // 1 gig

#define VERSION "1"

//#define DEBUG_PRINT(...) do{ UART_printf( __VA_ARGS__ ); } while( 0 )
//#define DEBUG_PRINT(...) do{ USB_printf( __VA_ARGS__ ); } while( 0 )
#define DEBUG_PRINT(...) do{ } while ( 0 )

#define BIT0  (1UL << 0)
#define BIT1  (1UL << 1)
#define BIT2  (1UL << 2)
#define BIT3  (1UL << 3)
#define BIT4  (1UL << 4)
#define BIT5  (1UL << 5)
#define BIT6  (1UL << 6)
#define BIT7  (1UL << 7)
#define BIT8  (1UL << 8)
#define BIT9  (1UL << 9)
#define BIT10 (1UL << 10)
#define BIT11 (1UL << 11)
#define BIT12 (1UL << 12)
#define BIT13 (1UL << 13)
#define BIT14 (1UL << 14)
#define BIT15 (1UL << 15)
#define BIT16 (1UL << 16)
#define BIT17 (1UL << 17)
#define BIT18 (1UL << 18)
#define BIT19 (1UL << 19)
#define BIT20 (1UL << 20)
#define BIT21 (1UL << 21)
#define BIT22 (1UL << 22)
#define BIT23 (1UL << 23)
#define BIT24 (1UL << 24)
#define BIT25 (1UL << 25)
#define BIT26 (1UL << 26)
#define BIT27 (1UL << 27)
#define BIT28 (1UL << 28)
#define BIT29 (1UL << 29)
#define BIT30 (1UL << 30)
#define BIT31 (1UL << 31)

// flags
#define FLAG_BTN_BRD       (1 << 0)
#define FLAG_BTN_BRD_LONG  (1 << 1)

// uart
#define UART_BUFFER_SIZE 8192

// lcd
#define LCD_WIDTH 10

#define MENU_CSEL    0
#define MENU_MSEL    1
#define MENU_TEST    2
#define MENU_DUMP    3
#define MENU_PROG    4
#define MENU_VERI    5
#define MENU_DONE    6
#define MENU_ERROR   7

#define CHIP_P       0
#define CHIP_S       1
#define CHIP_M       2
#define CHIP_C       3
#define CHIP_V       4
   
#define MODE_DUMP    1
#define MODE_PROG    2
#define MODE_VERI    3

#define MODE_TEST    0
#define MODE_DUMP    1
#define MODE_PROG    2
#define MODE_VERI    3

// dump buffer
#define BUFFER_SIZE 0x10000

#define SRAM_BUFFER __attribute__((section(".sram"))) __attribute__ ((aligned (4)))

#ifdef __cplusplus
}
#endif

#endif /* __DEFINES_H */
