# To-Learn-Cars-Pose-with-Deep-Learning
To train a CNN network to learn the cars' poses on the image

# Problem
I want to know a car's pose on the road in real scene. Detailedly, I want to get the azimuth of the car in the following picture. After taking a photo of a car, I can use ssd, yolo or other methods to bound the car in the image, then the next step is to recognize its pose based on the crop image. Actually, I am not able to get a large amount of these images, unless I am a robot so that I can take photos day and night. A way to acquire enough images for training is to generate them with 3D model and virtual cameras. Then the next step is to build a deep network to train.
![car_pose](https://github.com/scnuhealthy/To_Learn_Cars_Pose_with_Deep_Learning/blob/master/images/car_pose.png)

# Generating Images
## How to generate
This part is complex and it is not the key point of this blog. So I don't want to explain too much about the code. I have uploaded the code and you can look over it. Though this part is not easy, but the process is simple. I use the virtual carema to take photos of the 3D car model, add some interference and immerse them into random backgrounds. The random background and the random position of the car in the image make my data more robust.

## Images
You can see the images in the 'images' folder

![image1](https://github.com/scnuhealthy/To_Learn_Cars_Pose_with_Deep_Learning/blob/master/images/2c8e9ff5fd58ff3fcd046ccc4d5c3da2_0.jpg)
![image2](https://github.com/scnuhealthy/To_Learn_Cars_Pose_with_Deep_Learning/blob/master/images/7e6da78c8dde0479f30da7304391ba9f_221.jpg)

# Solution
The problem can be defined as the classification problem. The poses can be classified into 36 classes, 1-10 degree, 11-20 degree... Now I simplify the problem and I just class the cars into 4 classes,1-90 degree, 91-180 degree.. The best way to solve a classification problem is using deep learning.

# Deep learning
# Network
I can easily build the network with keras and tensorflow backend. The 'model.png' in the 'images' folder shows the complete network. I have packaged the input images as a .h5 file, you can download my .h5 file and train in your environment.

```python
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
```

# Parameters for training
```python
adam=optimizers.Adam(lr=0.00005, beta_1=0.9, beta_2=0.999, epsilon=1e-08)
model.compile(loss='categorical_crossentropy',
              optimizer=adam,
              metrics = ['accuracy'] )
```

# Result
The accuracy on the test data is over 90%! The following pictures show the training log from epoch 50 to 350.
![accuracy](https://github.com/scnuhealthy/To_Learn_Cars_Pose_with_Deep_Learning/blob/master/images/az_350_4_classes_accuracy.png)
![loss](https://github.com/scnuhealthy/To_Learn_Cars_Pose_with_Deep_Learning/blob/master/images/az_350_4_classes_loss.png)
