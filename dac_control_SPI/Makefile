# Makefile for MSP430 DAC SPI Project
# Make sure to download the following from Texas instruments:
# 1) msp430-gcc (an open-source compiler for MSP MCUs)
#    which can be found on https://www.ti.com/tool/MSP430-GCC-OPENSOURCE#downloads
# 2) MSP430Flasher (a command-line programmer for the MSP430 & MSP432 MCUs)

# Directories
SRC_DIR := src
INC_DIR := include
OBJ_DIR := obj
BIN_DIR := bin

# Source subdirectories 
SRC_SUBDIRS := hardware
OBJ_SUBDIRS := $(addprefix $(OBJ_DIR)/,$(SRC_SUBDIRS))

# Device and compiler settings
DEVICE := MSP430FR2433
MAP := $(OBJ_DIR)/dac_spi.map

# Define compiler and related tools
TI_SOFTWARE_DIR = /opt/ti/msp430-gcc
GCC_DIR = $(TI_SOFTWARE_DIR)/bin
SUPPORT_FILE_DIR = $(TI_SOFTWARE_DIR)/include
FLASHER_DIR = /opt/ti/MSPFlasher_1.3.20

CC = $(GCC_DIR)/msp430-elf-gcc
GDB = $(GCC_DIR)/msp430-elf-gdb
OBJCOPY = $(GCC_DIR)/msp430-elf-objcopy
FLASHER = $(FLASHER_DIR)/MSP430Flasher

# Define flags
CFLAGS = -I$(SUPPORT_FILE_DIR) -I$(INC_DIR) -mmcu=$(DEVICE) -mlarge -mdata-region=lower -mhwmult=f5series -Og -Wall -g
LFLAGS = -L$(SUPPORT_FILE_DIR) -Wl,-Map,$(MAP),--gc-sections

# Define OS-specific commands
ifeq ($(OS),Windows_NT)
    ifeq ($(shell uname -o),Cygwin)
        RM = rm -rf
    else
        RM = del /q
    endif
else
    RM = rm -rf
endif

# Find all source files
SRCS := $(wildcard $(SRC_DIR)/*.c) \
        $(foreach dir,$(SRC_SUBDIRS),$(wildcard $(SRC_DIR)/$(dir)/*.c))

# Generate object file paths
OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

# Main target
TARGET := $(BIN_DIR)/$(DEVICE).hex

# Default target
.PHONY: all clean debug flash

all: $(TARGET)

# Create necessary directories before any other operation
$(shell mkdir -p $(BIN_DIR))
$(shell mkdir -p $(OBJ_DIR))
$(shell mkdir -p $(OBJ_SUBDIRS))

# Link the out file
$(BIN_DIR)/$(DEVICE).out: $(OBJS)
	@echo "Linking $@..."
	$(CC) $(CFLAGS) $(LFLAGS) $^ -o $@

# Generate hex file
$(TARGET): $(BIN_DIR)/$(DEVICE).out
	@echo "Creating hex file $@..."
	$(OBJCOPY) -O ihex $< $@

# Compile source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "Compiling $<..."
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	@echo "Cleaning..."
	$(RM) $(OBJ_DIR)
	$(RM) $(BIN_DIR)
	mkdir -p $(BIN_DIR)
	mkdir -p $(OBJ_DIR)
	mkdir -p $(OBJ_SUBDIRS)

# Debug target
debug: $(BIN_DIR)/$(DEVICE).out
	$(GDB) $<

# Flash target
flash: $(TARGET)
	$(FLASHER) -w $< -v -z [VCC]

# Generate dependency files
DEPS := $(OBJS:.o=.d)
CFLAGS += -MMD -MP
-include $(DEPS)
