/*
 * NnetAgent.cpp
 */

#include "NnetAgent.h"

NnetAgent::NnetAgent(tflite::MicroInterpreter* interpreter) {

  printf("start\n");

  m_pInterpreter = interpreter;
  
  // Obtain pointers to the model's input and output tensors.
  m_pInput = interpreter->input(0);
  m_pOutput = interpreter->output(0);

  // Keep track of how many inferences we have performed.
  m_kCycleCount = 0;
  m_kInferenceCount = 0;

  printf("NnetAgent construction complete\n");
}

NnetAgent::~NnetAgent() {
  stop();
}


void NnetAgent::run(){

  printf("Nnet Started\n");

  while (true) { // Loop forever
#ifdef INT8_MODEL
    for(int i = 0; i < m_kNumInputs; i++) {
      m_pInput->data.int8[i] = (char)i;
    }
#else
    for(int i = 0; i < m_kNumInputs; i++) {
      m_pInput->data.f[i] = (float)i;
    }
#endif
    
    printf("NnetAgent Invoke\n");
    
    // Run inference, and report any error
    TfLiteStatus invoke_status = m_pInterpreter->Invoke();
    if (invoke_status != kTfLiteOk) {
      printf("Invoke %u failed\n", m_kInferenceCount);
      return;
    }

    printf("NnetAgent read output\n");
    
    // Read output SOC estimate and print to console
    float soc = 0.0;
#ifdef INT8_MODEL
    soc = (float) m_pOutput->data.int8[0];
#else
    soc = m_pOutput->data.f[0];
#endif

    printf("NnetAgent increment\n");
    
    // Increment the inference_counter, dump results once per cycle
    if ((m_kInferenceCount % m_kInferencesPerCycle) == 0) {
      printf("soc=%f, count=%d\n", soc, m_kInferenceCount);
      m_kCycleCount += 1;
    }
    m_kInferenceCount += 1;
  }
}

/***
 * Get the static depth required in words
 * @return - words
 */
configSTACK_DEPTH_TYPE NnetAgent::getMaxStackSize(){
	return 150;
}
