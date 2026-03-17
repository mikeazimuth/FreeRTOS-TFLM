/*
 * NnAgent.h
 *
 * Active agent to run as task and perform neural network inference
 *
 */

#ifndef NNETAGENT_H_
#define NNETAGENT_H_

#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_log.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/system_setup.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"

#include "Agent.h"

#ifdef INT8_MODEL
#include "model_int8.h"
#define ARENA_SIZE 16384
#else
#include "model_fp32.h"
#define ARENA_SIZE 32768
#endif
#define NUM_INPUTS 4
#define INFER_PER_CYCLE 100
#define INSERT_RESOLVER(a) \
  { \
    TfLiteStatus resolve_status = a; \
    if (resolve_status != kTfLiteOk) { \
      printf("Op resolution failed"); \
      return; \
    } \
  }
#define NNET_DELAY 500

class NnetAgent: public Agent {
public:
  NnetAgent(tflite::MicroInterpreter* interpreter);
  virtual ~NnetAgent();

protected:
  virtual void run();
  virtual configSTACK_DEPTH_TYPE getMaxStackSize();

  tflite::MicroInterpreter* m_pInterpreter = nullptr;
  TfLiteTensor* m_pInput = nullptr;
  TfLiteTensor* m_pOutput = nullptr;
  unsigned int m_kCycleCount = 0;
  unsigned int m_kInferenceCount = 0;
  const int m_kInferencesPerCycle = INFER_PER_CYCLE;
  const int m_kTensorArenaSize = ARENA_SIZE;
  int m_kNumInputs = NUM_INPUTS;
};

#endif /* NNETAGENT_H_ */
