// Modified to use an OCALL printf for out-of-enclave printing
#include "tensorflow/lite/micro/debug_log.h"
#include "../../../../Enclave.h"
#ifndef TF_LITE_STRIP_ERROR_STRINGS
#include <cstdio>
#endif

extern "C" void DebugLog(const char* s) {
#ifndef TF_LITE_STRIP_ERROR_STRINGS
  // Reusing TF_LITE_STRIP_ERROR_STRINGS to disable DebugLog completely to get
  // maximum reduction in binary size. This is because we have DebugLog calls
  // via TF_LITE_CHECK that are not stubbed out by TF_LITE_REPORT_ERROR.
  printf("%s", s);
#endif
}
