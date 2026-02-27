
cmake_minimum_required(VERSION 3.12)

project(freertos C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)
#set(PICO_BOARD pico2)
#set(PICO_PLATFORM rp2350)

include(FreeRTOS_Kernel_import.cmake)

add_executable(freertos_test "")

target_include_directories(freertos_test
  PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/.
)

set_target_properties(
  freertos_test
  PROPERTIES
  COMPILE_FLAGS -fno-rtti
  COMPILE_FLAGS -fno-exceptions
  COMPILE_FLAGS -fno-threadsafe-statics
  COMPILE_FLAGS -nostdlib
)

target_sources(freertos_test
  PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/float_model_data.cpp
  ${CMAKE_CURRENT_LIST_DIR}/int8_model_data.cpp
  ${CMAKE_CURRENT_LIST_DIR}/freertos_test.cpp
)

target_link_libraries(
  freertos_test
  pico-tflmicro
  hardware_pwm
  pico-tflmicro_test
  FreeRTOS-Kernel
  FreeRTOS-Kernel-Heap4
)

pico_enable_stdio_usb(freertos_test 1)
pico_enable_stdio_uart(freertos_test 0)

pico_add_extra_outputs(freertos_test)


add_executable(freertos "")

target_include_directories(freertos
  PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/.
)

set_target_properties(
  freertos
  PROPERTIES
  COMPILE_FLAGS -fno-rtti
  COMPILE_FLAGS -fno-exceptions
  COMPILE_FLAGS -fno-threadsafe-statics
  COMPILE_FLAGS -nostdlib
)

target_sources(freertos
  PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/constants.cpp
  ${CMAKE_CURRENT_LIST_DIR}/float_model_data.cpp
  ${CMAKE_CURRENT_LIST_DIR}/int8_model_data.cpp
  ${CMAKE_CURRENT_LIST_DIR}/main.cpp
  ${CMAKE_CURRENT_LIST_DIR}/main_functions.cpp
  ${CMAKE_CURRENT_LIST_DIR}/output_handler.cpp
  ${CMAKE_CURRENT_LIST_DIR}/constants.h
  ${CMAKE_CURRENT_LIST_DIR}/main_functions.h
  ${CMAKE_CURRENT_LIST_DIR}/output_handler.h
)

target_link_libraries(
  freertos
  pico-tflmicro
  hardware_pwm
  FreeRTOS-Kernel
  FreeRTOS-Kernel-Heap4
)

pico_enable_stdio_usb(freertos 1)
pico_enable_stdio_uart(freertos 0)

pico_add_extra_outputs(freertos)
