## Tensorflow Lite For Intel SGX

## Description

This repository contains a port of [TensorFlow Lite for Microcontrollers](https://www.tensorflow.org/lite/microcontrollers) library.

The port supports non-quantized `float32` models and compatible `float32` inputs. The port is model and input agnostic and should work on any [TF light flatbuffer](https://www.tensorflow.org/lite/convert) model and input. 

The sole restriction is that the model **must** be compromised of operators supported by the TensorFlow Lite for Microcontrollers library, a full list of supported operators can be found [here](https://github.com/tensorflow/tensorflow/blob/master/tensorflow/lite/micro/all_ops_resolver.cc).

The library can only be used to perform inference tasks. 

## Disclaimer

This port is designed to facilitate research on privacy preserving ML in the context of Intel SGX Enclaves. As such, it is not intended to be used in production and should not be used to protect model or data confidentiality.

## Usage

After compiling the project (`make clean && make -j4`), the application can be launched with 

```bash
./app <model path> <input path>
```

Any  [TF light flatbuffer](https://www.tensorflow.org/lite/convert)  model (non quantized, in `float32` format) is compatible, up to the list of supported operators. Model inputs should also be saved in a `float32` format, which can be done for example in the following manner:

```python
nparr.astype('float32').tofile('input.bin')
```

