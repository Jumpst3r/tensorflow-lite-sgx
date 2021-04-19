#ifndef ENCLAVE_U_H__
#define ENCLAVE_U_H__

#include <stdint.h>
#include <wchar.h>
#include <stddef.h>
#include <string.h>
#include "sgx_edger8r.h" /* for sgx_status_t etc. */

#include "user_types.h"

#include <stdlib.h> /* for size_t */

#define SGX_CAST(type, item) ((type)(item))

#ifdef __cplusplus
extern "C" {
#endif

#ifndef OCALL_PRINT_STRING_DEFINED__
#define OCALL_PRINT_STRING_DEFINED__
void SGX_UBRIDGE(SGX_NOCONVENTION, ocall_print_string, (const char* str));
#endif
#ifndef OCALL_GET_FILESIZE_DEFINED__
#define OCALL_GET_FILESIZE_DEFINED__
int SGX_UBRIDGE(SGX_NOCONVENTION, ocall_get_filesize, (const char* fname, long int* fsize));
#endif
#ifndef OCALL_LOAD_MODEL_DEFINED__
#define OCALL_LOAD_MODEL_DEFINED__
int SGX_UBRIDGE(SGX_NOCONVENTION, ocall_load_model, (const char* fname, long int fsize, unsigned char* data));
#endif
#ifndef OCALL_LOAD_INPUT_DEFINED__
#define OCALL_LOAD_INPUT_DEFINED__
int SGX_UBRIDGE(SGX_NOCONVENTION, ocall_load_input, (const char* fname, long int fsize, float* data));
#endif

sgx_status_t perform_inference(sgx_enclave_id_t eid, char* str, char* filename);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
