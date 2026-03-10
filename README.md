# FreeRTOS-TFLM
An pico-tflmicro example built on FreeRTOS.  To install, just copy the files and directories to the root directory of pico-tflmicro and re-run cmake from the build folder.

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

The UF2 file is in: examples/freertos_nn/src/freertos_nn.uf2

