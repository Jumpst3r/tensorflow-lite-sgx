#include "Enclave_t.h"

#include "sgx_trts.h" /* for sgx_ocalloc, sgx_is_outside_enclave */
#include "sgx_lfence.h" /* for sgx_lfence */

#include <errno.h>
#include <mbusafecrt.h> /* for memcpy_s etc */
#include <stdlib.h> /* for malloc/free etc */

#define CHECK_REF_POINTER(ptr, siz) do {	\
	if (!(ptr) || ! sgx_is_outside_enclave((ptr), (siz)))	\
		return SGX_ERROR_INVALID_PARAMETER;\
} while (0)

#define CHECK_UNIQUE_POINTER(ptr, siz) do {	\
	if ((ptr) && ! sgx_is_outside_enclave((ptr), (siz)))	\
		return SGX_ERROR_INVALID_PARAMETER;\
} while (0)

#define CHECK_ENCLAVE_POINTER(ptr, siz) do {	\
	if ((ptr) && ! sgx_is_within_enclave((ptr), (siz)))	\
		return SGX_ERROR_INVALID_PARAMETER;\
} while (0)

#define ADD_ASSIGN_OVERFLOW(a, b) (	\
	((a) += (b)) < (b)	\
)


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

static sgx_status_t SGX_CDECL sgx_perform_inference(void* pms)
{
	CHECK_REF_POINTER(pms, sizeof(ms_perform_inference_t));
	//
	// fence after pointer checks
	//
	sgx_lfence();
	ms_perform_inference_t* ms = SGX_CAST(ms_perform_inference_t*, pms);
	sgx_status_t status = SGX_SUCCESS;
	char* _tmp_str = ms->ms_str;
	size_t _len_str = ms->ms_str_len ;
	char* _in_str = NULL;
	char* _tmp_filename = ms->ms_filename;
	size_t _len_filename = ms->ms_filename_len ;
	char* _in_filename = NULL;

	CHECK_UNIQUE_POINTER(_tmp_str, _len_str);
	CHECK_UNIQUE_POINTER(_tmp_filename, _len_filename);

	//
	// fence after pointer checks
	//
	sgx_lfence();

	if (_tmp_str != NULL && _len_str != 0) {
		_in_str = (char*)malloc(_len_str);
		if (_in_str == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		if (memcpy_s(_in_str, _len_str, _tmp_str, _len_str)) {
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}

		_in_str[_len_str - 1] = '\0';
		if (_len_str != strlen(_in_str) + 1)
		{
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}
	}
	if (_tmp_filename != NULL && _len_filename != 0) {
		_in_filename = (char*)malloc(_len_filename);
		if (_in_filename == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		if (memcpy_s(_in_filename, _len_filename, _tmp_filename, _len_filename)) {
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}

		_in_filename[_len_filename - 1] = '\0';
		if (_len_filename != strlen(_in_filename) + 1)
		{
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}
	}

	perform_inference(_in_str, _in_filename);

err:
	if (_in_str) free(_in_str);
	if (_in_filename) free(_in_filename);
	return status;
}

SGX_EXTERNC const struct {
	size_t nr_ecall;
	struct {void* ecall_addr; uint8_t is_priv; uint8_t is_switchless;} ecall_table[1];
} g_ecall_table = {
	1,
	{
		{(void*)(uintptr_t)sgx_perform_inference, 0, 0},
	}
};

SGX_EXTERNC const struct {
	size_t nr_ocall;
	uint8_t entry_table[4][1];
} g_dyn_entry_table = {
	4,
	{
		{0, },
		{0, },
		{0, },
		{0, },
	}
};


sgx_status_t SGX_CDECL ocall_print_string(const char* str)
{
	sgx_status_t status = SGX_SUCCESS;
	size_t _len_str = str ? strlen(str) + 1 : 0;

	ms_ocall_print_string_t* ms = NULL;
	size_t ocalloc_size = sizeof(ms_ocall_print_string_t);
	void *__tmp = NULL;


	CHECK_ENCLAVE_POINTER(str, _len_str);

	if (ADD_ASSIGN_OVERFLOW(ocalloc_size, (str != NULL) ? _len_str : 0))
		return SGX_ERROR_INVALID_PARAMETER;

	__tmp = sgx_ocalloc(ocalloc_size);
	if (__tmp == NULL) {
		sgx_ocfree();
		return SGX_ERROR_UNEXPECTED;
	}
	ms = (ms_ocall_print_string_t*)__tmp;
	__tmp = (void *)((size_t)__tmp + sizeof(ms_ocall_print_string_t));
	ocalloc_size -= sizeof(ms_ocall_print_string_t);

	if (str != NULL) {
		ms->ms_str = (const char*)__tmp;
		if (_len_str % sizeof(*str) != 0) {
			sgx_ocfree();
			return SGX_ERROR_INVALID_PARAMETER;
		}
		if (memcpy_s(__tmp, ocalloc_size, str, _len_str)) {
			sgx_ocfree();
			return SGX_ERROR_UNEXPECTED;
		}
		__tmp = (void *)((size_t)__tmp + _len_str);
		ocalloc_size -= _len_str;
	} else {
		ms->ms_str = NULL;
	}
	
	status = sgx_ocall(0, ms);

	if (status == SGX_SUCCESS) {
	}
	sgx_ocfree();
	return status;
}

sgx_status_t SGX_CDECL ocall_get_filesize(int* retval, const char* fname, long int* fsize)
{
	sgx_status_t status = SGX_SUCCESS;
	size_t _len_fname = fname ? strlen(fname) + 1 : 0;
	size_t _len_fsize = sizeof(long int);

	ms_ocall_get_filesize_t* ms = NULL;
	size_t ocalloc_size = sizeof(ms_ocall_get_filesize_t);
	void *__tmp = NULL;

	void *__tmp_fsize = NULL;

	CHECK_ENCLAVE_POINTER(fname, _len_fname);
	CHECK_ENCLAVE_POINTER(fsize, _len_fsize);

	if (ADD_ASSIGN_OVERFLOW(ocalloc_size, (fname != NULL) ? _len_fname : 0))
		return SGX_ERROR_INVALID_PARAMETER;
	if (ADD_ASSIGN_OVERFLOW(ocalloc_size, (fsize != NULL) ? _len_fsize : 0))
		return SGX_ERROR_INVALID_PARAMETER;

	__tmp = sgx_ocalloc(ocalloc_size);
	if (__tmp == NULL) {
		sgx_ocfree();
		return SGX_ERROR_UNEXPECTED;
	}
	ms = (ms_ocall_get_filesize_t*)__tmp;
	__tmp = (void *)((size_t)__tmp + sizeof(ms_ocall_get_filesize_t));
	ocalloc_size -= sizeof(ms_ocall_get_filesize_t);

	if (fname != NULL) {
		ms->ms_fname = (const char*)__tmp;
		if (_len_fname % sizeof(*fname) != 0) {
			sgx_ocfree();
			return SGX_ERROR_INVALID_PARAMETER;
		}
		if (memcpy_s(__tmp, ocalloc_size, fname, _len_fname)) {
			sgx_ocfree();
			return SGX_ERROR_UNEXPECTED;
		}
		__tmp = (void *)((size_t)__tmp + _len_fname);
		ocalloc_size -= _len_fname;
	} else {
		ms->ms_fname = NULL;
	}
	
	if (fsize != NULL) {
		ms->ms_fsize = (long int*)__tmp;
		__tmp_fsize = __tmp;
		if (_len_fsize % sizeof(*fsize) != 0) {
			sgx_ocfree();
			return SGX_ERROR_INVALID_PARAMETER;
		}
		memset(__tmp_fsize, 0, _len_fsize);
		__tmp = (void *)((size_t)__tmp + _len_fsize);
		ocalloc_size -= _len_fsize;
	} else {
		ms->ms_fsize = NULL;
	}
	
	status = sgx_ocall(1, ms);

	if (status == SGX_SUCCESS) {
		if (retval) *retval = ms->ms_retval;
		if (fsize) {
			if (memcpy_s((void*)fsize, _len_fsize, __tmp_fsize, _len_fsize)) {
				sgx_ocfree();
				return SGX_ERROR_UNEXPECTED;
			}
		}
	}
	sgx_ocfree();
	return status;
}

sgx_status_t SGX_CDECL ocall_load_model(int* retval, const char* fname, long int fsize, unsigned char* data)
{
	sgx_status_t status = SGX_SUCCESS;
	size_t _len_fname = fname ? strlen(fname) + 1 : 0;
	size_t _len_data = fsize;

	ms_ocall_load_model_t* ms = NULL;
	size_t ocalloc_size = sizeof(ms_ocall_load_model_t);
	void *__tmp = NULL;

	void *__tmp_data = NULL;

	CHECK_ENCLAVE_POINTER(fname, _len_fname);
	CHECK_ENCLAVE_POINTER(data, _len_data);

	if (ADD_ASSIGN_OVERFLOW(ocalloc_size, (fname != NULL) ? _len_fname : 0))
		return SGX_ERROR_INVALID_PARAMETER;
	if (ADD_ASSIGN_OVERFLOW(ocalloc_size, (data != NULL) ? _len_data : 0))
		return SGX_ERROR_INVALID_PARAMETER;

	__tmp = sgx_ocalloc(ocalloc_size);
	if (__tmp == NULL) {
		sgx_ocfree();
		return SGX_ERROR_UNEXPECTED;
	}
	ms = (ms_ocall_load_model_t*)__tmp;
	__tmp = (void *)((size_t)__tmp + sizeof(ms_ocall_load_model_t));
	ocalloc_size -= sizeof(ms_ocall_load_model_t);

	if (fname != NULL) {
		ms->ms_fname = (const char*)__tmp;
		if (_len_fname % sizeof(*fname) != 0) {
			sgx_ocfree();
			return SGX_ERROR_INVALID_PARAMETER;
		}
		if (memcpy_s(__tmp, ocalloc_size, fname, _len_fname)) {
			sgx_ocfree();
			return SGX_ERROR_UNEXPECTED;
		}
		__tmp = (void *)((size_t)__tmp + _len_fname);
		ocalloc_size -= _len_fname;
	} else {
		ms->ms_fname = NULL;
	}
	
	ms->ms_fsize = fsize;
	if (data != NULL) {
		ms->ms_data = (unsigned char*)__tmp;
		__tmp_data = __tmp;
		if (_len_data % sizeof(*data) != 0) {
			sgx_ocfree();
			return SGX_ERROR_INVALID_PARAMETER;
		}
		memset(__tmp_data, 0, _len_data);
		__tmp = (void *)((size_t)__tmp + _len_data);
		ocalloc_size -= _len_data;
	} else {
		ms->ms_data = NULL;
	}
	
	status = sgx_ocall(2, ms);

	if (status == SGX_SUCCESS) {
		if (retval) *retval = ms->ms_retval;
		if (data) {
			if (memcpy_s((void*)data, _len_data, __tmp_data, _len_data)) {
				sgx_ocfree();
				return SGX_ERROR_UNEXPECTED;
			}
		}
	}
	sgx_ocfree();
	return status;
}

sgx_status_t SGX_CDECL ocall_load_input(int* retval, const char* fname, long int fsize, float* data)
{
	sgx_status_t status = SGX_SUCCESS;
	size_t _len_fname = fname ? strlen(fname) + 1 : 0;
	size_t _len_data = fsize;

	ms_ocall_load_input_t* ms = NULL;
	size_t ocalloc_size = sizeof(ms_ocall_load_input_t);
	void *__tmp = NULL;

	void *__tmp_data = NULL;

	CHECK_ENCLAVE_POINTER(fname, _len_fname);
	CHECK_ENCLAVE_POINTER(data, _len_data);

	if (ADD_ASSIGN_OVERFLOW(ocalloc_size, (fname != NULL) ? _len_fname : 0))
		return SGX_ERROR_INVALID_PARAMETER;
	if (ADD_ASSIGN_OVERFLOW(ocalloc_size, (data != NULL) ? _len_data : 0))
		return SGX_ERROR_INVALID_PARAMETER;

	__tmp = sgx_ocalloc(ocalloc_size);
	if (__tmp == NULL) {
		sgx_ocfree();
		return SGX_ERROR_UNEXPECTED;
	}
	ms = (ms_ocall_load_input_t*)__tmp;
	__tmp = (void *)((size_t)__tmp + sizeof(ms_ocall_load_input_t));
	ocalloc_size -= sizeof(ms_ocall_load_input_t);

	if (fname != NULL) {
		ms->ms_fname = (const char*)__tmp;
		if (_len_fname % sizeof(*fname) != 0) {
			sgx_ocfree();
			return SGX_ERROR_INVALID_PARAMETER;
		}
		if (memcpy_s(__tmp, ocalloc_size, fname, _len_fname)) {
			sgx_ocfree();
			return SGX_ERROR_UNEXPECTED;
		}
		__tmp = (void *)((size_t)__tmp + _len_fname);
		ocalloc_size -= _len_fname;
	} else {
		ms->ms_fname = NULL;
	}
	
	ms->ms_fsize = fsize;
	if (data != NULL) {
		ms->ms_data = (float*)__tmp;
		__tmp_data = __tmp;
		if (_len_data % sizeof(*data) != 0) {
			sgx_ocfree();
			return SGX_ERROR_INVALID_PARAMETER;
		}
		memset(__tmp_data, 0, _len_data);
		__tmp = (void *)((size_t)__tmp + _len_data);
		ocalloc_size -= _len_data;
	} else {
		ms->ms_data = NULL;
	}
	
	status = sgx_ocall(3, ms);

	if (status == SGX_SUCCESS) {
		if (retval) *retval = ms->ms_retval;
		if (data) {
			if (memcpy_s((void*)data, _len_data, __tmp_data, _len_data)) {
				sgx_ocfree();
				return SGX_ERROR_UNEXPECTED;
			}
		}
	}
	sgx_ocfree();
	return status;
}

