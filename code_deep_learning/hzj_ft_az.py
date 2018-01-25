import numpy as np
import matplotlib.pyplot as plt
from keras.models import Sequential
from keras.layers import Dense, Dropout, Activation, Flatten
from keras.layers import Convolution2D, MaxPooling2D,Conv2D
from keras.utils import np_utils,plot_model
from keras import backend as K
import h5py
from keras.models import model_from_json
from keras import regularizers,optimizers

import os
os.environ["CUDA_VISIBLE_DEVICES"] = "0"

batch_size = 128
nb_epoch = 350

# Input shape
img_width = 224
img_height = 224
channel = 3
input_shape = (img_height,img_width,channel)

# number of convolutional filters to use
nb_filters1 = 32
nb_filters2 = 64
nb_filters3 = 64
nb_filters4 = 128
nb_filters5 = 128
nb_filters6 = 256
# size of pooling area for max pooling
pool_size = (2, 2)
# convolution kernel size
kernel_size = (3, 3)

model = Sequential()

# Convolution layers
model.add(Conv2D(nb_filters1, 7, 7,
                        border_mode='valid',
                        input_shape=input_shape))
model.add(Activation('relu'))
model.add(MaxPooling2D(pool_size=pool_size))
model.add(Dropout(0.4))

model.add(Conv2D(nb_filters2, kernel_size[0], kernel_size[1]))
model.add(Activation('relu'))
model.add(MaxPooling2D(pool_size=pool_size))
model.add(Dropout(0.4))

model.add(Conv2D(nb_filters3, kernel_size[0], kernel_size[1]))
model.add(Activation('relu'))
model.add(MaxPooling2D(pool_size=pool_size))
model.add(Dropout(0.4))

model.add(Conv2D(nb_filters4, kernel_size[0], kernel_size[1]))
model.add(Activation('relu'))
model.add(MaxPooling2D(pool_size=pool_size))
model.add(Dropout(0.4))

model.add(Conv2D(nb_filters5, kernel_size[0], kernel_size[1]))
model.add(Activation('relu'))
model.add(MaxPooling2D(pool_size=pool_size))
model.add(Dropout(0.4))

model.add(Conv2D(nb_filters6, kernel_size[0], kernel_size[1]))
model.add(Activation('relu'))
model.add(MaxPooling2D(pool_size=pool_size))
model.add(Dropout(0.4))

# Fully connected layer
model.add(Flatten())
model.add(Dense(4096))
model.add(Activation('relu'))
model.add(Dropout(0.25))
model.add(Dense(1024))
model.add(Activation('relu'))
model.add(Dropout(0.25))
model.add(Dense(256))
model.add(Activation('relu'))
model.add(Dropout(0.25))
model.add(Dense(4))
model.add(Activation('softmax'))

# parameters for training
adam=optimizers.Adam(lr=0.00005, beta_1=0.9, beta_2=0.999, epsilon=1e-08)
model.compile(loss='categorical_crossentropy',
              optimizer=adam,
              metrics = ['accuracy']
              )
# plot the model 
plot_model(model,to_file = 'model.png',show_shapes = True, show_layer_names = True)

# input file path 
dfnm = '/home/zsx/cvpr2018zsx/programe/nets_v1/zsx_cvpr2018_Resnet/'
h5_filedir = dfnm + 'data_hzj_180122.h5'
f = h5py.File(h5_filedir)

# load the data
all_img = f['img'][:]
az_label = f['az'][:]

# class the labels
az_label = az_label/10
az_label = az_label.astype('int')
for i in range(len(az_label)):
	if az_label[i] < 9:
		az_label[i] = 0
	elif az_label[i] < 18:
		az_label[i] = 1
	elif az_label[i] < 27:
		az_label[i] = 2
	else:
		az_label[i] = 3
az_label = np_utils.to_categorical(az_label)

# divide it into training data and test data
nb_img = len(all_img)
nb_train = int(0.8*nb_img)

idx = np.arange(nb_img)
np.random.shuffle(idx)
all_img = all_img.reshape((-1,img_width,img_height,channel))
train_img = all_img[idx][:nb_train]
train_az_label = az_label[idx][:nb_train]
test_img =  all_img[idx][nb_train:]
test_az_label = az_label[idx][nb_train:]

# load the trained model 
model.load_weights('./hzj_data_log/model_az_92-300-epoch.h5')

history = model.fit(train_img, train_az_label, batch_size=batch_size, nb_epoch=nb_epoch,
            verbose=1, validation_data=(test_img,test_az_label))

#score = model.evaluate(test_img, test_az_label, verbose=1)
#print(score)

# predict
predict = model.predict(test_img,batch_size=batch_size,verbose=0)
for i in range(10):
	print(test_az_label[i])
	print(predict[i])
	print('*'*50)
model.save(('./model_az-%d-epoch.h5' % nb_epoch))

# summarize history for accuracy
plt.plot(history.history['acc'])
plt.plot(history.history['val_acc'])
plt.title('model accuracy')
plt.ylabel('accuracy')
plt.xlabel('epoch')
plt.legend(['train', 'test'], loc='upper left')
plt.show()

# summarize history for loss
plt.plot(history.history['loss'])
plt.plot(history.history['val_loss'])
plt.title('model loss')
plt.ylabel('loss')
plt.xlabel('epoch')
plt.legend(['train', 'test'], loc='upper left')
plt.show()