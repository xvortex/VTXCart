# Hey Emacs, this is a -*- makefile -*-

#----------------------------------------------------------------------------
# WinAVR Makefile Template written by Eric B. Weddington, Joerg Wunsch, et al.
#
# Released to the Public Domain
#
# Additional material for this makefile was written by:
# Peter Fleury
# Tim Henigan
# Colin O'Flynn
# Reiner Patommel
# Markus Pfaff
# Sander Pool
# Frederik Rouleau
# Carlos Lamas
#
#
# Extensively modified for sd2iec and later adapted for ARM by Ingo Korb
# Further modifications for programming adapters and multiple targets
# by Maximilian Rehkopf for sd2snes
#
# Stripped down version for VTXCart
#
# To rebuild project do "make clean" then "make all".
#----------------------------------------------------------------------------

# Enable verbose compilation with "make V=1"
ifdef V
 Q :=
 E := @:
else
 Q := @
 E := @echo
endif

# Directory for all generated files
OBJDIR := obj
DEPDIR := .dep

# Linker script
LINKERSCRIPT = gcc_arm.ld

# Target file name (without extension).
TARGET = $(OBJDIR)/vtxdumper

# Version
-include VERSION

# List C source files here. (C dependencies are automatically generated.)
SRC  = $(wildcard User/Src/*.c)
SRC += $(wildcard Drivers/BSP/Src/*.c)
SRC += $(wildcard Libraries/FatFs/*.c)
SRC += $(wildcard Libraries/USB/Src/*.c)
SRC += Drivers/HAL/Src/stm32h7xx_hal.c
SRC += Drivers/HAL/Src/stm32h7xx_hal_cortex.c
SRC += Drivers/HAL/Src/stm32h7xx_hal_dma.c
SRC += Drivers/HAL/Src/stm32h7xx_hal_dma_ex.c
SRC += Drivers/HAL/Src/stm32h7xx_hal_exti.c
SRC += Drivers/HAL/Src/stm32h7xx_hal_gpio.c
SRC += Drivers/HAL/Src/stm32h7xx_hal_i2s.c
SRC += Drivers/HAL/Src/stm32h7xx_hal_i2s_ex.c
SRC += Drivers/HAL/Src/stm32h7xx_hal_ospi.c
SRC += Drivers/HAL/Src/stm32h7xx_hal_pcd.c
SRC += Drivers/HAL/Src/stm32h7xx_hal_pcd_ex.c
SRC += Drivers/HAL/Src/stm32h7xx_hal_pwr.c
SRC += Drivers/HAL/Src/stm32h7xx_hal_pwr_ex.c
SRC += Drivers/HAL/Src/stm32h7xx_hal_rcc.c
SRC += Drivers/HAL/Src/stm32h7xx_hal_rcc_ex.c
SRC += Drivers/HAL/Src/stm32h7xx_hal_rtc.c
SRC += Drivers/HAL/Src/stm32h7xx_hal_rtc_ex.c
SRC += Drivers/HAL/Src/stm32h7xx_hal_sd.c
SRC += Drivers/HAL/Src/stm32h7xx_hal_spi.c
SRC += Drivers/HAL/Src/stm32h7xx_hal_tim.c
SRC += Drivers/HAL/Src/stm32h7xx_hal_tim_ex.c
SRC += Drivers/HAL/Src/stm32h7xx_hal_uart.c
SRC += Drivers/HAL/Src/stm32h7xx_hal_uart_ex.c
SRC += Drivers/HAL/Src/stm32h7xx_ll_sdmmc.c
SRC += Drivers/HAL/Src/stm32h7xx_ll_usb.c

# List Assembler source files here.
#     Make them always end in a capital .S.  Files ending in a lowercase .s
#     will not be considered source files but generated files (assembler
#     output from the compiler), and will be deleted upon "make clean"!
#     Even though the DOS/Win* filesystem matches both .s and .S the same,
#     it will preserve the spelling of the filenames, and gcc itself does
#     care about how the name is spelled on its command-line.
ASRC = Drivers/CMSIS/Startup/startup_stm32h750xx_gcc.S

# Optimization level, can be [0, 1, 2, 3, s].
#     0 = turn off optimization. s = optimize for size.
#     (Note: 3 is not always the best optimization level. See avr-libc FAQ.)
# Use s -mcall-prologues when you really need size...
OPT = s

# Debugging format.
DEBUG = dwarf-2


# List any extra directories to look for include files here.
#     Each directory must be separated by a space.
#     Use forward slashes for directory separators.
#     For a directory that has spaces, enclose it in quotes.
EXTRAINCDIRS = Drivers/BSP/Inc Drivers/CMSIS/Inc Drivers/HAL/Inc Libraries/USB/Inc
EXTRAINCDIRS += Libraries/FatFs User/Inc


# Compiler flag to set the C Standard level.
#     c89   = "ANSI" C
#     gnu89 = c89 plus GCC extensions
#     c99   = ISO C99 standard (not yet fully implemented)
#     gnu99 = c99 plus GCC extensions
CSTANDARD = -std=gnu99


# Place -D or -U options here
CDEFS = -DSTM32H750xx -DUSE_HAL_DRIVER -DNDEBUG -DHSE_CLOCK=25000000L


# Place -I options here
CINCS =


# CPU-specific flags
ifndef CPUFLAGS
  CPUFLAGS := -mcpu=cortex-m7 -mfloat-abi=hard -mfpu=fpv5-d16
endif

ifndef ARCH
  ARCH := arm-none-eabi
endif

# Define programs and commands.
# CC must be defined here to generate the correct CFLAGS
SHELL = sh
CC = $(ARCH)-gcc
OBJCOPY = $(ARCH)-objcopy
OBJDUMP = $(ARCH)-objdump
SIZE = $(ARCH)-size
NM = $(ARCH)-nm
REMOVE = rm -f
COPY = cp
AWK = gawk
BIN2C =

#---------------- Compiler Options ----------------
#  -g*:          generate debugging information
#  -O*:          optimization level
#  -f...:        tuning, see GCC manual and avr-libc documentation
#  -Wall...:     warning level
#  -Wa,...:      tell GCC to pass this to the assembler.
#    -adhlns...: create assembler listing
CFLAGS = -g$(DEBUG)
CFLAGS += $(CDEFS) $(CINCS)
CFLAGS += -O$(OPT)
CFLAGS += $(CPUFLAGS) -nostartfiles
CFLAGS += -Wall -Wstrict-prototypes -Werror -Wno-strict-aliasing
CFLAGS += -Wa,-adhlns=$(OBJDIR)/$(<:.c=.lst)
CFLAGS += -I$(OBJDIR)
CFLAGS += $(patsubst %,-I%,$(EXTRAINCDIRS))
CFLAGS += $(CSTANDARD)
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += --specs=nano.specs --specs=nosys.specs


#---------------- Assembler Options ----------------
#  -Wa,...:   tell GCC to pass this to the assembler.
#  -ahlms:    create listing
#  -gstabs:   have the assembler create line number information; note that
#             for use in COFF files, additional information about filenames
#             and function names needs to be present in the assembler source
#             files -- see avr-libc docs [FIXME: not yet described there]
ASFLAGS = $(CPUFLAGS) -Wa,-adhlns=$(OBJDIR)/$(<:.S=.lst),-gstabs -I$(OBJDIR)


#---------------- Linker Options ----------------
#  -Wl,...:     tell GCC to pass this to linker.
#    -Map:      create map file
#    --cref:    add cross reference to  map file
LDFLAGS = -Wl,-Map=$(TARGET).map,--cref
LDFLAGS += -T$(LINKERSCRIPT)
LDFLAGS += -Wl,--gc-sections
ifeq ($(CONFIG_LINKER_RELAX),y)
  LDFLAGS += -Wl,-O9,--relax
endif

#============================================================================


# De-dupe the list of C source files
CSRC := $(sort $(SRC))

# Define all object files.
OBJ := $(patsubst %,$(OBJDIR)/%,$(CSRC:.c=.o) $(ASRC:.S=.o))

# Define all listing files.
LST := $(patsubst %,$(OBJDIR)/%,$(CSRC:.c=.lst) $(ASRC:.S=.lst))

# Generate list of obj dirs
OBJDIRS := $(sort $(dir $(OBJ)))

# Compiler flags to generate dependency files.
GENDEPFLAGS = -MMD -MP -MF $(DEPDIR)/$(@F).d


# Combine all necessary flags and optional flags.
# Add target processor to flags.
ALL_CFLAGS = -I. $(CFLAGS) $(GENDEPFLAGS)
ALL_ASFLAGS = -I. -x assembler-with-cpp $(ASFLAGS) $(CDEFS)


# Default target.
all: build

build: elf bin hex
	$(E) "  SIZE   $(TARGET).elf"
	$(Q)$(ELFSIZE)|grep -v debug

elf: $(TARGET).elf
bin: $(TARGET).bin
hex: $(TARGET).hex
eep: $(TARGET).eep
lss: $(TARGET).lss
sym: $(TARGET).sym


# Display size of file.
ELFSIZE = $(SIZE) -x -A $(TARGET).elf


# Create final output files from ELF output file.
# --gap-fill 0xff is important to match the padding values as they end up
# in physical flash; CRC calculation is wrong otherwise.
$(TARGET).bin: $(TARGET).elf
	$(E) "  BIN    $@"
	$(Q)$(OBJCOPY) --gap-fill 0xff -O binary $< $@

$(OBJDIR)/%.hex: $(OBJDIR)/%.elf
	$(E) "  HEX    $@"
	$(Q)$(OBJCOPY) -O ihex $< $@

# Create extended listing file from ELF output file.
$(OBJDIR)/%.lss: $(OBJDIR)/%.elf
	$(E) "  LSS    $<"
	$(Q)$(OBJDUMP) -h -S $< > $@

# Create a symbol table from ELF output file.
$(OBJDIR)/%.sym: $(OBJDIR)/%.elf
	$(E) "  SYM    $<"
	$(E)$(NM) -n $< > $@


# Link: create ELF output file from object files.
.PRECIOUS : $(OBJ)
$(TARGET).elf: .EXTRA_PREREQS = $(LINKERSCRIPT)
$(TARGET).elf : $(OBJ)
	$(E) "  LINK   $@"
	$(Q)$(CC) $(ALL_CFLAGS) $^ --output $@ $(LDFLAGS)

# Compile: create object files from C source files.
$(OBJDIR)/%.o : %.c | $(OBJDIRS)
	$(E) "  CC     $<"
	$(Q)$(CC) -c $(ALL_CFLAGS) $< -o $@


# Compile: create assembler files from C source files.
$(OBJDIR)/%.s : %.c | $(OBJDIR)
	$(CC) -S $(ALL_CFLAGS) $< -o $@


# Assemble: create object files from assembler source files.
$(OBJDIR)/%.o : %.S | $(OBJDIRS)
	$(E) "  AS     $<"
	$(Q)$(CC) -c $(ALL_ASFLAGS) $< -o $@

# Create preprocessed source for use in sending a bug report.
$(OBJDIR)/%.i : %.c | $(OBJDIR)
	$(CC) -E -mmcu=$(MCU) -I. $(CFLAGS) $< -o $@

# Create the output directory
$(OBJDIRS) :
	$(E) "  MKDIR  $(OBJDIRS)"
	$(Q)mkdir -p $(OBJDIRS)

# Target: clean project.
clean: begin clean_list end

clean_list :
	$(E) "  CLEAN"
	$(Q)$(REMOVE) $(TARGET).hex
	$(Q)$(REMOVE) $(TARGET).bin
	$(Q)$(REMOVE) $(TARGET).elf
	$(Q)$(REMOVE) $(TARGET).map
	$(Q)$(REMOVE) $(TARGET).sym
	$(Q)$(REMOVE) $(TARGET).lss
	$(Q)$(REMOVE) $(OBJ)
	$(Q)$(REMOVE) $(OBJDIR)/.DS_Store
	$(Q)$(REMOVE) $(LST)
	$(Q)$(REMOVE) $(CSRC:.c=.s)
	$(Q)$(REMOVE) $(CSRC:.c=.d)
	$(Q)$(REMOVE) $(DEPDIR)/*
	-$(Q)rmdir --ignore-fail-on-non-empty -p $(OBJDIRS)

# Include the dependency files.
-include $(info $(DEPDIR)) $(shell mkdir $(DEPDIR) 2>/dev/null) $(wildcard $(DEPDIR)/*)

# Listing of phony targets.
.PHONY : all begin finish end sizebefore sizeafter \
build elf hex lss sym clean clean_list
