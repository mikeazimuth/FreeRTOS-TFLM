# FreeRTOS-TFLM
This project presents a set of pico-tflmicro C++ examples built on FreeRTOS.  This project depends on:
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

### Using Command Line

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

### Using Visual Studio Code

* Run setup.sh as in the command line instructions to set up CMakeLists.txt and environmental variables.
* Open your project in VS Code from WSL:
  * Open an WSL terminal.
  * Navigate to deps/pico-tflmicro
  * Run the command: code .
    * If VS Code is properly installed (with CMake and WSL extensions), the VS Code window will open and you'll see "WSL: Ubuntu" (or your distro name) in the bottom-left corner of the status bar, indicating a remote connection.
* Select a CMake Kit:
  * Open the Command Palette (Ctrl+Shift+P) and run the command "CMake: Select a Kit".
  * Select the GCC arm-none-eabi compiler.
* Configure the project:
  * Open the Command Palette (Ctrl+Shift+P) and run "CMake: Configure". This will generate the build files in a build directory within your project folder in WSL. The output will appear in the "CMake" pane of the Output window.
* Build the project:
  * Open the Command Palette (Ctrl+Shift+P) and run "CMake: Build" or select the Build button from the status bar. The compiled executable will be located in the build directory.
* Run and Debug:
  * Ensure that usbipd-win has been installed under Windows and the RP2350 device is mapped.
    * It may be necessary to map the device the first time it is used
      * curl https://raw.githubusercontent.com/raspberrypi/picotool/master/udev/60-picotool.rules | sudo tee /etc/udev/rules.d/60-picotool.rules
      * sudo usermod -a -G dialout $USER
      * sudo udevadm control --reload-rules
      * sudo udevadm trigger
    * From admin cmd prompt:  usbipd list
      * Find the BUSID of the RP2350 and substitute below
    * From admin cmd prompt:  usbipd bind --busid BUSID
    * From admin cmd prompt:  usbipd attach --wsl Ubuntu --busid BUSID
  * Ensure that the C/C++ extension and Cortex-Debug extension are installed in VS Code.
  * Ensure that the debug probe (separate board) is setup correctly and connected to the RP2350.
  * Set a breakpoint in your source code by clicking in the editor margin.
  * Select the appropriate debug target from the debug dropdown menu in the status bar or the Run and Debug view.
  * Press F5 or select "Run > Start Debugging" to build and launch your application with the debugger attached. Program output will be displayed in the debug console or the integrated terminal.


## Installation Instructions

### Using Command Line

Simply plug in the RP2350 board, press and release both buttons, and an RP2350 drive will appear under the Windows filesystem.  Then just copy the UF2 file onto the RP2350 drive.  When the file copy finishes, the RP2350 will reboot and the firmware will begin running (LED light will start blinking, NN results will be written to console).

### Using Visual Studio Code

When the device is properly mapped using usbipd (from a Windows Admin Cmd shell -- see above), it is only necessary to click the Run Project line under RASPBERRY PI PICO PROJECT: QUICK ACCESS to flash the firmware and run the project.

## Serial monitor

A serial console such as PuTTY may be used to view serial console output.  There is some delay built into the example to allow for connecting the serial console right after the UF2 file copy completes.  If it is connected in time, no serial messages will be lost.

Note:  While there is a serial monitor built into VS Code, it cannot be used under WSL2.  WSL2 does not allow access to serial devices.  So it is necessary to use PuTTY from Windows even when developing with VS Code under WSL2.

## Acknowlegments

The hello world example is based on the pico-tflmicro hello world example ([Pete Warden](https://github.com/raspberrypi/pico-tflmicro)) and a FreeRTOS hello world example ([Jon Durrant](https://github.com/jondurrant/RPIPico2-Exp)).
