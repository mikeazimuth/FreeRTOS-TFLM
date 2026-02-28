/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include <FreeRTOS.h>
#include <task.h>
#include "pico/stdlib.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_log.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/system_setup.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "main_functions.h"

/* Required if configSUPPORT_STATIC_ALLOCATION is 1 */
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
				   StackType_t **ppxIdleTaskStackBuffer,
				   uint32_t *pulIdleTaskStackSize)
{
    /* If the idle task is created using statically allocated memory then the 
       created task function must provide all the necessary memory for the task 
       to function. */
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE];

    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

/* Required if configSUPPORT_STATIC_ALLOCATION is 1 and configUSE_TIMERS is 1 */
extern "C" {
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
				    StackType_t **ppxTimerTaskStackBuffer,
				    uint32_t *pulTimerTaskStackSize)
{
    /* If the buffers have already been provided, they must be null. */
    configASSERT( ( *ppxTimerTaskTCBBuffer == NULL ) && ( *ppxTimerTaskStackBuffer == NULL ) && ( *pulTimerTaskStackSize == NULL ) );

    /* The buffers and the stack size are defined here. The buffers must be declared
       with the MPU_ATTRIBUTE_ section attribute, and be aligned to the cache
       line size when using an MPU version of FreeRTOS. */
    static StaticTask_t xTimerTaskTCB;
    static StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH];

    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
    /* Run time stack overflow checking is performed if
    configCHECK_FOR_STACK_OVERFLOW is set to 1 or 2 in FreeRTOSConfig.h.
    This hook function is called if a stack overflow is detected. */
    
    // Log the error or halt the system
    printf("Stack overflow in task: %s\r\n", pcTaskName);
    
    // Disable interrupts and loop forever
    for (;;);
}

// This is the default main used on systems that have the standard C entry
// point. Other devices (for example FreeRTOS or ESP32) that have different
// requirements for entry code (like an app_main function) should specialize
// this main.cc file in a target-specific subfolder.
//int main(int argc, char* argv[]) {
//  setup();
//  while (true) {
//    loop();
//  }
//}

//void tflm_task(void *pvParameters) {
//    // 1. Initialize Model
//    const tflite::Model* model = tflite::GetModel(g_model_data);
//    
//    // 2. Setup Interpreter
//    static tflite::MicroMutableOpResolver<10> resolver;
//    // Add required ops (e.g., resolver.AddFullyConnected())
//    
//    tflite::MicroInterpreter interpreter(model, resolver, tensor_arena, kArenaSize);
//    interpreter.AllocateTensors();
//
//    while (true) {
//        // 3. Run Inference
//        interpreter.Invoke();
//        // 4. Process results and delay to allow other tasks to run
//        vTaskDelay(pdMS_TO_TICKS(100));
//    }
//}

int main() {
    stdio_init_all();
    setup();
    //xTaskCreate(tflm_task, "TFLM_Task", 4096, NULL, 1, NULL);
    xTaskCreate(loop, "TFLM_Task", 4096, NULL, 1, NULL);
    vTaskStartScheduler();
}
