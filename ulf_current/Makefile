# Make sure to download the following from Texas instruments:
# 1) msp430-gcc (an open-source compiler for MSP MCUs)
#    which can be found on https://www.ti.com/tool/MSP430-GCC-OPENSOURCE#downloads
# 2) MSP430Flasher (a command-line programmer for the MSP430 & MSP432 MCUs)

OBJECTS=ulf_current.o
MAP=ulf_current.map
MAKEFILE=Makefile

ifeq ($(OS),Windows_NT)
	ifeq ($(shell uname -o),Cygwin)
		RM= rm -rf
	else
		RM= del /q
	endif
else
	RM= rm -rf
endif

TI_SOFTWARE_DIR = /opt/ti/msp430-gcc
GCC_DIR = $(TI_SOFTWARE_DIR)/bin
SUPPORT_FILE_DIR = $(TI_SOFTWARE_DIR)/include
FLASHER_DIR = /opt/ti/MSPFlasher_1.3.20

#Enter the Texas Instruments MCU 
DEVICE  = MSP430FR2433

CC      = $(GCC_DIR)/msp430-elf-gcc
GDB     = $(GCC_DIR)/msp430-elf-gdb
OBJCOPY = $(GCC_DIR)/msp430-elf-objcopy
FLASHER = $(FLASHER_DIR)/MSP430Flasher

CFLAGS = -I $(SUPPORT_FILE_DIR) -mmcu=$(DEVICE) -mlarge -mdata-region=lower -mhwmult=f5series -Og -Wall -g
LFLAGS = -L $(SUPPORT_FILE_DIR) -Wl,-Map,$(MAP),--gc-sections

# Default target
all: ${DEVICE}.hex

# Compile objects
${OBJECTS}: %.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Link to create .out file
${DEVICE}.out: ${OBJECTS}
	$(CC) $(CFLAGS) $(LFLAGS) $^ -o $@

# Convert .out to .hex
${DEVICE}.hex: ${DEVICE}.out
	$(OBJCOPY) -O ihex $< $@

clean: 
	$(RM) $(OBJECTS)
	$(RM) $(MAP)
	$(RM) *.out
	$(RM) *.hex

debug: ${DEVICE}.out
	$(GDB) $

# Add flash target to program the device
flash: ${DEVICE}.hex
	$(FLASHER) -w $< -v -z [VCC]
