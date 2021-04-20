## Tensorflow Lite For Intel SGX

![](header.png)

Table of Contents
=================

  * [Description](#Description)
  * [Disclaimer](#Disclaimer)
  * [Requirements](#Requirements)
  * [Usage](#Usage)

## Description

This repository contains a port of [TensorFlow Lite for Microcontrollers](https://www.tensorflow.org/lite/microcontrollers) library for Intel SGX. It is intended to be easy to understand and follows the same source code structure as Intel's sample enclave code. Furthermore, it does not require an a priori installation of Tensorflow or complex build systems and toolchains (except the Intel SGX SDK & tools).

This port supports non-quantized `float32` models and compatible `float32` inputs. The port is model and input agnostic and should work on any [TF light flatbuffer](https://www.tensorflow.org/lite/convert) model and input. 

The sole restriction is that the model **must** be compromised of operators supported by the TensorFlow Lite for Microcontrollers library, a full list of supported operators can be found [here](https://github.com/tensorflow/tensorflow/blob/master/tensorflow/lite/micro/all_ops_resolver.cc).

The library can only be used to perform inference tasks, this limitation is due to the Tensorflow lite library and not the SGX environment.

## Disclaimer

This port is designed to facilitate research on privacy preserving ML in the context of Intel SGX Enclaves. As such, it is not intended to be used in production and should not be used to protect model or data confidentiality.

## Requirements

Requires a platform with Intel SGX support, an installed SGX [driver](https://github.com/intel/linux-sgx-driver) and the Intel SGX [SDK](https://github.com/intel/linux-sgx). If you are able to compile and run the [sample Enclave](https://github.com/intel/linux-sgx/tree/master/SampleCode/SampleEnclave) after installing the SGX driver & SDK, you should have no problems building and running this project.

## Usage

After compiling the project (`make clean && make -j4`), the application can be launched with 

```bash
./tf-sgx-infer <model path> <input path>
```

Any  [TF light flatbuffer](https://www.tensorflow.org/lite/convert)  model (non quantized, in `float32` format) is compatible, up to the list of supported operators. Model inputs should also be saved in a `float32` format, which can be done for example in the following manner:

```python
nparr.astype('float32').tofile('input.bin')
```

Loading large models from disk might require increasing the default stack size on linux. If you run into segmentation faults, try increasing the stack size as follows:

```bash
ulimit -S -s 131072   
```

This will temporarly increase the stack size from 8MB to 128MB.
