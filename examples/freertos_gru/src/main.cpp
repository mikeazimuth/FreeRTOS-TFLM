/*
 * main.cpp - demonstration of running GRU network under FreeRTOS
 *
 */

#include <cstdio>
#include "BlinkAgent.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_log.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/system_setup.h"
#include "tensorflow/lite/schema/schema_generated.h"
#ifdef INT8_MODEL
#include "model_int8.h"
#define ARENA_SIZE 16384
#else
#include "model_fp32.h"
#define ARENA_SIZE 32768
#endif
#define NUM_INPUTS 4

// Globals, used for compatibility with Arduino-style sketches.
namespace {
  const tflite::Model* model = nullptr;
  tflite::MicroInterpreter* interpreter = nullptr;
  TfLiteTensor* input = nullptr;
  TfLiteTensor* output = nullptr;
  unsigned int cycle_count = 0;
  unsigned int inference_count = 0;
  const int kInferencesPerCycle = 100;
  constexpr int kTensorArenaSize = ARENA_SIZE;
  int num_inputs = NUM_INPUTS;
  uint8_t tensor_arena[kTensorArenaSize];
}  // namespace


#define TASK_PRIORITY ( tskIDLE_PRIORITY + 1UL ) //Standard Task priority
#define GP_BLINK 25


/***
 * Debug function to look at Task Stats
 */
void runTimeStats() {
  TaskStatus_t *pxTaskStatusArray;
  volatile UBaseType_t uxArraySize, x;
  unsigned long ulTotalRunTime;

  // Get number of takss
  uxArraySize = uxTaskGetNumberOfTasks();
  printf("Number of tasks %d\n", uxArraySize);

  //Allocate a TaskStatus_t structure for each task.
  pxTaskStatusArray = (TaskStatus_t *)pvPortMalloc( uxArraySize * sizeof( TaskStatus_t ) );

  if( pxTaskStatusArray != NULL ){
    // Generate raw status information about each task.
    uxArraySize = uxTaskGetSystemState( pxTaskStatusArray, uxArraySize, &ulTotalRunTime );

    // Print stats
    for( x = 0; x < uxArraySize; x++ ) {
      printf("Task: %d \t cPri:%d \t bPri:%d \t hw:%d \t%s\n",
	     pxTaskStatusArray[ x ].xTaskNumber ,
	     pxTaskStatusArray[ x ].uxCurrentPriority ,
	     pxTaskStatusArray[ x ].uxBasePriority ,
	     pxTaskStatusArray[ x ].usStackHighWaterMark ,
	     pxTaskStatusArray[ x ].pcTaskName
	     );
    }

    // Free array
    vPortFree( pxTaskStatusArray );
  } else {
    printf("Failed to allocate space for stats\n");
  }

  //Get heap allocation information
  HeapStats_t heapStats;
  vPortGetHeapStats(&heapStats);
  printf("HEAP avl: %d, blocks %d, alloc: %d, free: %d\n",
	 heapStats.xAvailableHeapSpaceInBytes,
	 heapStats.xNumberOfFreeBlocks,
	 heapStats.xNumberOfSuccessfulAllocations,
	 heapStats.xNumberOfSuccessfulFrees
	 );
}


/***
 * Main task to boot the other Agents
 * @param params - unused
 */
void mainTask(void *params){
  BlinkAgent blink(GP_BLINK);

  printf("Boot task started\n");
  blink.start("Blink", TASK_PRIORITY);

  for (;;){
    
#ifdef INT8_MODEL
    for(int i = 0; i < num_inputs; i++) {
      input->data.int8[i] = (char)i;
    }
#else
    for(int i = 0; i < num_inputs; i++) {
      input->data.f[i] = (float)i;
    }
#endif
    
    // Run inference, and report any error
    TfLiteStatus invoke_status = interpreter->Invoke();
    if (invoke_status != kTfLiteOk) {
      printf("Invoke %u failed\n", inference_count);
      return;
    }

    // Read output SOC estimate and print to console
    float soc = 0.0;
#ifdef INT8_MODEL
    soc = (float) output->data.int8[0];
#else
    soc = output->data.f[0];
#endif

    // Increment the inference_counter, dump results once per cycle
    if ((inference_count % kInferencesPerCycle) == 0) {
      printf("soc=%f, count=%d\n", soc, inference_count);
      cycle_count += 1;
    }
    inference_count += 1;
    
    vTaskDelay(2000);
    runTimeStats();
  }
}


/***
 * Launch the tasks and scheduler
 */
void vLaunch( void) {

	//Start blink task
    TaskHandle_t task;
    xTaskCreate(mainTask, "MainThread", 500, NULL, TASK_PRIORITY, &task);

    /* Start the tasks and timer running. */
    vTaskStartScheduler();
}


// The name of this function is important for Arduino compatibility.
void setup() {
  tflite::InitializeTarget();

  // Map the model into a usable data structure. This doesn't involve any
  // copying or parsing, it's a very lightweight operation.
  model = tflite::GetModel(g_float_model_data);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    printf("Model provided is schema version %d not equal "
	   "to supported version %d.",
	   model->version(), TFLITE_SCHEMA_VERSION);
    return;
  }

  // This pulls in all the operation implementations we need.
  // NOLINTNEXTLINE(runtime-global-variables)
  static tflite::MicroMutableOpResolver<1> resolver;
  TfLiteStatus resolve_status = resolver.AddFullyConnected();
  if (resolve_status != kTfLiteOk) {
    printf("Op resolution failed");
    return;
  }

  // Build an interpreter to run the model with.
  static tflite::MicroInterpreter static_interpreter(
      model, resolver, tensor_arena, kTensorArenaSize);
  interpreter = &static_interpreter;

  // Allocate memory from the tensor_arena for the model's tensors.
  TfLiteStatus allocate_status = interpreter->AllocateTensors();
  if (allocate_status != kTfLiteOk) {
    printf("AllocateTensors() failed");
    return;
  }

  // Obtain pointers to the model's input and output tensors.
  input = interpreter->input(0);
  output = interpreter->output(0);

  // Keep track of how many inferences we have performed.
  cycle_count = 0;
  inference_count = 0;

  printf("Setup complete\n");
}



int main(){

  stdio_init_all();
  sleep_ms(3000);
  setup();
  sleep_ms(3000);

  printf("Go\n");

  //Start tasks and scheduler
  const char *rtos_name = "FreeRTOS";
  printf("Starting %s on core 0:\n", rtos_name);
  vLaunch();

  for (;;){
    sleep_ms(3000);
  }
}


