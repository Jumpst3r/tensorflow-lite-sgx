#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "Enclave_t.h"
#include "sgx_error.h"

int stdin = 0, stdout = 1, stderr = 2;

int load_model(char *modelname, long f_size, unsigned char *data){
    int ret;
    if (ocall_load_model(&ret, modelname, f_size, data) != SGX_SUCCESS)
        return -1;
    return ret;
}

int load_input(char *modelname, long f_size, float *data){
    int ret;
    if (ocall_load_input(&ret, modelname, f_size, data) != SGX_SUCCESS)
        return -1;
    return ret;
}


int get_filesize(char *modelname, long *f_size){
    int ret;
    if (ocall_get_filesize(&ret, modelname, f_size) != SGX_SUCCESS)
        return -1;
    return ret;
}

int printf(const char *fmt, ...) {
    char buf[8192] = {'\0'};
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, 8192, fmt, ap);
    va_end(ap);
    ocall_print_string(buf);
    return (int)strnlen(buf, 8192 - 1) + 1;
}
