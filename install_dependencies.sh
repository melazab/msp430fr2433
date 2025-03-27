#!/bin/bash

# Install open source tools from Texas Instruments for x86-64 Linux systems
#  1) msp430-gcc: to compile C source files to executables for MSP430 MCUs
#  2) msp430-flasher: to flash target MSP430 MCUs

# Check if msp430-gcc is already installed
if [ -d "/opt/ti/msp430-gcc-9.3.1.11_linux64" ] || [ -d "/opt/ti/msp430-gcc" ]; then
    echo "MSP430 GCC toolchain is already installed."
else
    echo "Installing MSP430 GCC toolchain..."
    
    # Create directory if it doesn't exist
    sudo mkdir -p /opt/ti
    
    # Download and install msp430-gcc
    wget https://dr-download.ti.com/software-development/ide-configuration-compiler-or-debugger-compiler-sdk/MD-p4jCxzXtQP/9.3.1.11/msp430-gcc-9.3.1.11_linux64.tar.bz2
    sudo tar xvf msp430-gcc-9.3.1.11_linux64.tar.bz2 --directory=/opt/ti
    rm msp430-gcc-9.3.1.11_linux64.tar.bz2
    
    echo "MSP430 GCC toolchain installed successfully."
fi

# Check if MSP Flasher is already installed
if command -v MSP430Flasher &> /dev/null; then
    echo "MSP430 Flasher is already installed."
else
    echo "Installing MSP430 Flasher..."
    
    # Download and install msp430-flasher
    wget https://dr-download.ti.com/software-development/software-programming-tool/MD-szn5bCveqt/1.03.20.00/MSPFlasher-1_03_20_00-linux-x64-installer.zip
    unzip -o MSPFlasher-1_03_20_00-linux-x64-installer.zip
    
    chmod u+x MSPFlasher-1.3.20-linux-x64-installer.run
    ./MSPFlasher-1.3.20-linux-x64-installer.run
    
    echo "MSP430 Flasher installation completed."
    rm -f MSPFlasher-1.3.20-linux-x64-installer.run
fi

echo "Dependency installation process completed."
