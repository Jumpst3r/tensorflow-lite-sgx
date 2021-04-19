
#ifndef _ENCLAVE_H_
#define _ENCLAVE_H_

#include <assert.h>
#include "stdioPatched.h"
#include <stdlib.h>

#if defined(__cplusplus)
extern "C" {
#endif

void perform_inference(char *model_path, char *input_path);

#if defined(__cplusplus)
}
#endif

#endif /* !_ENCLAVE_H_ */
