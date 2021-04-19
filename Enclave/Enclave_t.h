#ifndef ENCLAVE_T_H__
#define ENCLAVE_T_H__

#include <stdint.h>
#include <wchar.h>
#include <stddef.h>
#include "sgx_edger8r.h" /* for sgx_ocall etc. */

#include "user_types.h"

#include <stdlib.h> /* for size_t */

#define SGX_CAST(type, item) ((type)(item))

#ifdef __cplusplus
extern "C" {
#endif

void perform_inference(char* str, char* filename);

sgx_status_t SGX_CDECL ocall_print_string(const char* str);
sgx_status_t SGX_CDECL ocall_get_filesize(int* retval, const char* fname, long int* fsize);
sgx_status_t SGX_CDECL ocall_load_model(int* retval, const char* fname, long int fsize, unsigned char* data);
sgx_status_t SGX_CDECL ocall_load_input(int* retval, const char* fname, long int fsize, float* data);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
