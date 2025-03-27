#!/bin/bash

# Check if firmware file argument is provided
if [ $# -eq 0 ]; then
    echo "Error: No firmware file specified"
    echo "Usage: $0 <firmware_file.hex>"
    exit 1
fi

# Set path to MSP430Flasher
FLASHER_PATH="/opt/ti/MSPFlasher_1.3.20/"

# Check if MSP430Flasher exists
if [ ! -f "$FLASHER_PATH" ]; then
    echo "Error: MSP430Flasher not found at $FLASHER_PATH"
    exit 1
fi

# Add the MSPFlasher directory to LD_LIBRARY_PATH
export LD_LIBRARY_PATH="../MSPFlasher_1.3.20:$LD_LIBRARY_PATH"

clear
"$FLASHER_PATH" -w "$1" -v -g -z [VCC]
read -p "Press any key to continue..."
