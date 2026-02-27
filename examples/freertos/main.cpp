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
