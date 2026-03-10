export PICOTOOL_FETCH_FROM_GIT_PATH=$PWD/deps/picotool
export FREERTOS_PATH=$PWD/deps/FreeRTOS-Kernel
export PICO_SDK_PATH=$PWD/deps/pico-sdk

echo "add_subdirectory(\"examples/freertos_nn\")" >> deps/pico-tflmicro/CMakeLists.txt
echo "add_subdirectory(\"examples/freertos_gru\")" >> deps/pico-tflmicro/CMakeLists.txt
ln -s $PWD/examples/freertos_nn deps/pico-tflmicro/examples/freertos_nn
ln -s $PWD/examples/freertos_gru deps/pico-tflmicro/examples/freertos_gru

