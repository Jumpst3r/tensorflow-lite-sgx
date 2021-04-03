
#include "Enclave.h"
#include "Enclave_t.h" /* print_string */
#include <stdarg.h>
#include <stdio.h> /* vsnprintf */
#include <string.h>

#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "model.h"

int printf(const char* fmt, ...)
{
    char buf[BUFSIZ] = { '\0' };
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, BUFSIZ, fmt, ap);
    va_end(ap);
    ocall_print_string(buf);
    return (int)strnlen(buf, BUFSIZ - 1) + 1;
}

void perform_inference(){

    printf("IN ENCLAVE\n");
    // Setup logging
    tflite::MicroErrorReporter micro_error_reporter;
    tflite::ErrorReporter* error_reporter = &micro_error_reporter;

    // load model defined in model.h
    const tflite::Model* model = ::tflite::GetModel(g_model);
    if (model->version() != TFLITE_SCHEMA_VERSION) {
    TF_LITE_REPORT_ERROR(error_reporter,
        "Model provided is schema version %d not equal "
        "to supported version %d.\n",
        model->version(), TFLITE_SCHEMA_VERSION);
    }

    // init op resolver
    tflite::AllOpsResolver resolver;

    // Allocate memory
    const int tensor_arena_size = 2 * 1024;
    uint8_t tensor_arena[tensor_arena_size];

    // init interpreter & allocate tensors
    tflite::MicroInterpreter interpreter(model, resolver, tensor_arena, tensor_arena_size, error_reporter);
    interpreter.AllocateTensors();

    // Obtain a pointer to the model's input tensor
    TfLiteTensor* input = interpreter.input(0);

    // Set input data
    input->data.f[0] = 0.4;

    // Eval model

    TfLiteStatus invoke_status = interpreter.Invoke();
    if (invoke_status != kTfLiteOk) {
        TF_LITE_REPORT_ERROR(error_reporter, "Invoke failed\n");
    }

    // Check model output
    TfLiteTensor* output = interpreter.output(0);
   
    // Obtain the output value from the tensor
    float value = output->data.f[0];
    // Check that the output value is within 0.05 of the expected value

    printf("RESULT=%f", value);


}
