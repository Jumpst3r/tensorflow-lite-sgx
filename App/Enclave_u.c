#include "Enclave_u.h"
#include <errno.h>

typedef struct ms_perform_inference_t {
	char* ms_str;
	size_t ms_str_len;
	char* ms_filename;
	size_t ms_filename_len;
} ms_perform_inference_t;

typedef struct ms_ocall_print_string_t {
	const char* ms_str;
} ms_ocall_print_string_t;

typedef struct ms_ocall_get_filesize_t {
	int ms_retval;
	const char* ms_fname;
	long int* ms_fsize;
} ms_ocall_get_filesize_t;

typedef struct ms_ocall_load_model_t {
	int ms_retval;
	const char* ms_fname;
	long int ms_fsize;
	unsigned char* ms_data;
} ms_ocall_load_model_t;

typedef struct ms_ocall_load_input_t {
	int ms_retval;
	const char* ms_fname;
	long int ms_fsize;
	float* ms_data;
} ms_ocall_load_input_t;

static sgx_status_t SGX_CDECL Enclave_ocall_print_string(void* pms)
{
	ms_ocall_print_string_t* ms = SGX_CAST(ms_ocall_print_string_t*, pms);
	ocall_print_string(ms->ms_str);

	return SGX_SUCCESS;
}

static sgx_status_t SGX_CDECL Enclave_ocall_get_filesize(void* pms)
{
	ms_ocall_get_filesize_t* ms = SGX_CAST(ms_ocall_get_filesize_t*, pms);
	ms->ms_retval = ocall_get_filesize(ms->ms_fname, ms->ms_fsize);

	return SGX_SUCCESS;
}

static sgx_status_t SGX_CDECL Enclave_ocall_load_model(void* pms)
{
	ms_ocall_load_model_t* ms = SGX_CAST(ms_ocall_load_model_t*, pms);
	ms->ms_retval = ocall_load_model(ms->ms_fname, ms->ms_fsize, ms->ms_data);

	return SGX_SUCCESS;
}

static sgx_status_t SGX_CDECL Enclave_ocall_load_input(void* pms)
{
	ms_ocall_load_input_t* ms = SGX_CAST(ms_ocall_load_input_t*, pms);
	ms->ms_retval = ocall_load_input(ms->ms_fname, ms->ms_fsize, ms->ms_data);

	return SGX_SUCCESS;
}

static const struct {
	size_t nr_ocall;
	void * table[4];
} ocall_table_Enclave = {
	4,
	{
		(void*)Enclave_ocall_print_string,
		(void*)Enclave_ocall_get_filesize,
		(void*)Enclave_ocall_load_model,
		(void*)Enclave_ocall_load_input,
	}
};
sgx_status_t perform_inference(sgx_enclave_id_t eid, char* str, char* filename)
{
	sgx_status_t status;
	ms_perform_inference_t ms;
	ms.ms_str = str;
	ms.ms_str_len = str ? strlen(str) + 1 : 0;
	ms.ms_filename = filename;
	ms.ms_filename_len = filename ? strlen(filename) + 1 : 0;
	status = sgx_ecall(eid, 0, &ocall_table_Enclave, &ms);
	return status;
}

