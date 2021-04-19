import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from keras.datasets import mnist, cifar10
from keras.models import Sequential
from keras.layers import *
from keras.utils import np_utils
import tensorflow as tf
import os

MODELS_DIR = 'data/models/'
INPUTS_DIR = 'data/inputs/'

if not os.path.exists(MODELS_DIR):
    os.mkdir(MODELS_DIR)

MODEL_NO_QUANT_TFLITE = MODELS_DIR + 'modelCIFAR.tflite'

(X_train, y_train), (X_test, y_test) = cifar10.load_data()


X_train = X_train.astype('float32')
X_test = X_test.astype('float32')

number_of_classes = 10

Y_train = np_utils.to_categorical(y_train, number_of_classes)
Y_test = np_utils.to_categorical(y_test, number_of_classes)

model = Sequential()

# example of a 3-block vgg style architecture
model = Sequential()
model.add(Input(shape=(32,32,3), name="CIFAR input"))
model.add(Conv2D(32, (3, 3), activation='relu', padding='same', input_shape=(32, 32, 3)))
model.add(Conv2D(32, (3, 3), activation='relu', padding='same'))
model.add(MaxPooling2D((2, 2)))
model.add(Conv2D(64, (3, 3), activation='relu', padding='same'))
model.add(Conv2D(64, (3, 3), activation='relu', padding='same'))
model.add(MaxPooling2D((2, 2)))
model.add(Conv2D(128, (3, 3), activation='relu', padding='same'))
model.add(Conv2D(128, (3, 3), activation='relu', padding='same'))
model.add(MaxPooling2D((2, 2)))
model.add(Flatten())
model.add(Dense(128, activation='relu', kernel_initializer='he_uniform'))
model.add(Dense(10, activation='softmax'))

model.compile(loss='categorical_crossentropy', optimizer='adam', metrics=['accuracy'])


size = int(len(X_train) * 0.8)

train_x, val_x = X_train[:size], X_train[size:]
train_y, val_y = Y_train[:size], Y_train[size:]


model.fit(train_x, train_y, batch_size=128, epochs=3, validation_data=(val_x, val_y))

with open('model.yaml', 'w') as f:
  f.write(model.to_yaml())

print(train_x.shape)

# Convert the model to the TensorFlow Lite format without quantization
converter = tf.lite.TFLiteConverter.from_keras_model(model)
model_no_quant_tflite = converter.convert()

# Save the model to disk
open(MODEL_NO_QUANT_TFLITE, "wb").write(model_no_quant_tflite)


# Save an input sample
val_x[0].reshape(1, -1).astype('float32').tofile(INPUTS_DIR + 'cifar.bin')
