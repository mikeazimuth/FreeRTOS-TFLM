# FreeRTOS-TFLM
A set of pico-tflmicro examples built on FreeRTOS.  This project depends on:
* Linux (WSL Ubuntu used for building the image)
* Cmake (see [pico-sdk build instructions](https://github.com/raspberrypi/pico-sdk?tab=readme-ov-file#unix-command-line))
* ARM cross-compiler (see [pico-sdk build instructions](https://github.com/raspberrypi/pico-sdk?tab=readme-ov-file#unix-command-line))
* Dependencies incorporated as Git submodules:
    * [pico-sdk](https://github.com/raspberrypi/pico-sdk)
    * [pico-tflmicro](https://github.com/raspberrypi/pico-tflmicro)
    * [FreeRTOS-Kernel](https://github.com/raspberrypi/FreeRTOS-Kernel)
* picotool (gets pulled during pico-tflmicro build process)

The following files are included:
* README.md - this file
* setup.sh - a bash script to set environmental variables and modify CMakeLists.txt to add new examples
* examples/freertos_nn - hello world example that blinks the LED, runs a simple NN, and reports to console

## Build Instructions

```
git submodule update --init
source setup.sh
cd deps/pico-sdk
git submodule update --init
cd ../pico-tflmicro/
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DPICO_PLATFORM=rp2350 -DPICO_BOARD=pico2 ..
make -j 4
```

The UF2 file for freertos_nn is in: examples/freertos_nn/src/freertos_nn.uf2

## Installation Instructions

Simply plug in the RP2350 board, press and release both buttons, and an RP2350 drive will appear under the Windows filesystem.  Then just copy the UF2 file onto the RP2350 drive.  When the file copy finishes, the RP2350 will reboot and the firmware will begin running (LED light will start blinking, NN results will be written to console).

A serial console such as PuTTY may be used to view serial console output.  There is some delay built into the example to allow for connecting the serial console right after the UF2 file copy completes.  If it is connected in time, no serial messages will be lost.

## Acknowlegments

The hello world example is based on the pico-tflmicro hello world example ([Pete Warden](https://github.com/raspberrypi/pico-tflmicro)) and a FreeRTOS hello world example ([Jon Durrant](https://github.com/jondurrant/RPIPico2-Exp)).
