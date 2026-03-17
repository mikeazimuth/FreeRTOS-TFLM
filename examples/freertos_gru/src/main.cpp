/*
 * main.cpp - demonstration of running GRU network under FreeRTOS
 *
 */

#include <cstdio>
#include "BlinkAgent.h"
#include "NnetAgent.h"

#define TASK_PRIORITY ( tskIDLE_PRIORITY + 1UL ) //Standard Task priority
#define GP_BLINK 25

// Globals, used for compatibility with Arduino-style sketches.
namespace {
  const tflite::Model* model = nullptr;
  tflite::MicroInterpreter* interpreter = nullptr;
  const int kTensorArenaSize = ARENA_SIZE;
  int num_inputs = NUM_INPUTS;
  uint8_t tensor_arena[kTensorArenaSize];
}  // namespace

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

void setup() {
  tflite::InitializeTarget();

  printf("after initialize\n");
  
  // Map the model into a usable data structure. This doesn't involve any
  // copying or parsing, it's a very lightweight operation.
#ifdef INT8_MODEL
  model = tflite::GetModel(g_int8_model_data);
#else
  model = tflite::GetModel(g_float_model_data);
#endif
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    printf("Model provided is schema version %d not equal "
	   "to supported version %d.",
	   model->version(), TFLITE_SCHEMA_VERSION);
    return;
  }

  printf("after get model\n");
  
  // This pulls in all the operation implementations we need.
  static tflite::MicroMutableOpResolver<10> resolver;
  INSERT_RESOLVER(resolver.AddSlice())
  INSERT_RESOLVER(resolver.AddFullyConnected())
  INSERT_RESOLVER(resolver.AddAdd())
  INSERT_RESOLVER(resolver.AddLogistic())
  INSERT_RESOLVER(resolver.AddMul())
  INSERT_RESOLVER(resolver.AddTanh())
  INSERT_RESOLVER(resolver.AddSub())
  INSERT_RESOLVER(resolver.AddRelu())
  INSERT_RESOLVER(resolver.AddConcatenation())
  INSERT_RESOLVER(resolver.AddReshape())

  // Build an interpreter to run the model with.
  static tflite::MicroInterpreter static_interpreter(model, resolver, tensor_arena, kTensorArenaSize);
  interpreter = &static_interpreter;

  printf("after interpreter creation\n");
  
  // Allocate memory from the tensor_arena for the model's tensors.
  TfLiteStatus allocate_status = interpreter->AllocateTensors();
  if (allocate_status != kTfLiteOk) {
    printf("AllocateTensors() failed");
    return;
  }

  printf("after tensor arena allocation\n");

}
/***
 * Main task to boot the other Agents
 * @param params - unused
 */
void mainTask(void *params){
  BlinkAgent blink(GP_BLINK);
  NnetAgent nnet(interpreter);

  printf("Boot task started\n");

  blink.start("Blink", TASK_PRIORITY);
  nnet.start("Nnet", TASK_PRIORITY);

  for (;;){
    vTaskDelay(2000);
    runTimeStats();
  }
}

/***
 * Launch the tasks and scheduler
 */
void vLaunch( void) {

  printf("vLaunch: creating main task\n");
  
  //Start main task and sub-tasks
  TaskHandle_t task;
  xTaskCreate(mainTask, "MainThread", 500, NULL, TASK_PRIORITY, &task);
  
  printf("vLaunch: starting scheduler\n");

  // Start the tasks and timer running
  vTaskStartScheduler();

  printf("vLaunch: done\n");
}

int main(){

  stdio_init_all();
  sleep_ms(3000);
  setup();
  sleep_ms(500);

  printf("Go\n");

  //Start tasks and scheduler
  const char *rtos_name = "FreeRTOS";
  printf("Starting %s on core 0:\n", rtos_name);
  vLaunch();

  for (;;){
    sleep_ms(3000);
  }
}


