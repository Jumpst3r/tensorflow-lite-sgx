// Modified to use an OCALL printf for out-of-enclave printing
#include "tensorflow/lite/micro/debug_log.h"
#include "../../../../Enclave.h"

extern "C" void DebugLog(const char* s) {
    printf("%s", s);
}
