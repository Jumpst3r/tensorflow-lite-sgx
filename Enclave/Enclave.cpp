#include "Enclave.h"


#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/examples/hello_world/model.h"
#include "tensorflow/lite/micro/examples/hello_world/output_handler.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/system_setup.h"
#include "tensorflow/lite/schema/schema_generated.h"

#include "stdioPatched.h"

namespace
{
  tflite::ErrorReporter *error_reporter = nullptr;
  const tflite::Model *model = nullptr;
  tflite::MicroInterpreter *interpreter = nullptr;
  TfLiteTensor *input = nullptr;
  TfLiteTensor *output = nullptr;
  int inference_count = 0;

  constexpr int kTensorArenaSize = 540 * 2000;
  uint8_t tensor_arena[kTensorArenaSize];
} // namespace

void setup(unsigned char *tfmodel)
{
  tflite::InitializeTarget();
  static tflite::MicroErrorReporter micro_error_reporter;
  error_reporter = &micro_error_reporter;
  model = tflite::GetModel(tfmodel);
  if (model->version() != TFLITE_SCHEMA_VERSION)
  {
    TF_LITE_REPORT_ERROR(error_reporter,
                         "Model provided is schema version %d not equal "
                         "to supported version %d.",
                         model->version(), TFLITE_SCHEMA_VERSION);
    return;
  }

  static tflite::AllOpsResolver resolver;
  static tflite::MicroInterpreter static_interpreter(
      model, resolver, tensor_arena, kTensorArenaSize, error_reporter);
  interpreter = &static_interpreter;

  // Allocate memory from the tensor_arena for the model's tensors.
  TfLiteStatus allocate_status = interpreter->AllocateTensors();
  if (allocate_status != kTfLiteOk)
  {
    TF_LITE_REPORT_ERROR(error_reporter, "AllocateTensors() failed");
    return;
  }

  // Obtain pointers to the model's input and output tensors.
  input = interpreter->input(0);

  output = interpreter->output(0);

  printf("[-ENCLAVE-] model loaded,  input type: %s", TfLiteTypeGetName(input->type));
  printf(" output type: %s \n", TfLiteTypeGetName(output->type));
  printf("[-ENCLAVE-] model loaded,  input dims[0]: %d\n", input->dims[0].data[1]);
  printf("[-ENCLAVE-] model loaded,  input dim[1]: %d\n", input->dims[1].data[0]);

  // Keep track of how many inferences we have performed.
  inference_count = 0;
}

void load_data(float *data, TfLiteTensor *input)
{
  for (int i = 0; i < input->bytes; ++i)
  {
    input->data.f[i] = data[i];
  }
}

void perform_inference(char *model_path, char *input_path)
{

  // Read the tflight model from disk:
  long fsize;
  int res = get_filesize(model_path, &fsize);
  if (res < 0)
  {
    printf("failed to read model file\n");
    return;
  }

  unsigned char tfmodel[fsize + 1];
  int res1 = load_model(model_path, fsize, tfmodel);
  if (res1 < 0)
  {
    printf("failed to read model file\n");
    return;
  }

  // load input data

  // Get input length

  long isize;
  int res3 = get_filesize(input_path, &isize);
  if (res3 < 0)
  {
    printf("failed to read input file\n");
    return;
  }

  float tfinput[isize + 1];
  load_input(input_path, isize, tfinput);

  setup(tfmodel);

  load_data(tfinput, input);
  printf("[-ENCLAVE-] transfered data to model\n");

  TfLiteStatus invoke_status = interpreter->Invoke();
  if (invoke_status != kTfLiteOk)
  {
    TF_LITE_REPORT_ERROR(error_reporter, "Invoke failed\n");
    return;
  }

  printf("[-ENCLAVE-] Inference results: (output dim: %d)\n", output->dims[1].data[0]);

  for (int i = 0; i < (output->dims[1].data[0]); i++){
    printf("output[%d] = %2f \n",i, output->data.f[i]);
  }
  
}
