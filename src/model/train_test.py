import numpy as np
import matplotlib.pyplot as plt
from tensorflow import keras
# from tensorflow.keras.models import keras.Sequential #type: ignore
# from tensorflow.keras.layers import Dense #type: ignore
# from tensorflow.keras.optimizers import Adam #type: ignore
# from tensorflow.keras.utils import to_categorical #type: ignore
# from tensorflow.keras.layers import Conv2D, MaxPooling2D #type: ignore
# from tensorflow.keras.layers import Dropout, Flatten #type: ignore
import cv2
from sklearn.model_selection import train_test_split
from sklearn.utils import shuffle
import pickle
import os
import pandas as pd
# from keras.preprocessing.image import keras.preprocessing.image.ImageDataGenerator
from tensorflow.keras.preprocessing.image import ImageDataGenerator # type: ignore

path = "/home/seyyit/dataset/" 
labelFile = '/home/seyyit/projects/cpp-autonomous-vehicle/src/model/labels.csv'
batch_size_val=32 
epochs_val=10
imageDimesions = (32,32,3)
testRatio = 0.2    
validationRatio = 0.2 

count = 0
images = []
classNo = []
myList = os.listdir(path)
print("Total Classes Detected:",len(myList))
noOfClasses=len(myList)
print("Importing Classes.....")

for x in range (0,len(myList)):
    myPicList = os.listdir(path+"/"+str(count))
    for y in myPicList:
        curImg = cv2.imread(path+"/"+str(count)+"/"+y)
        img = cv2.resize(curImg, (32, 32))
        images.append(img)
        classNo.append(count)
    print(count, end =" ")
    count +=1
# images, classNo = shuffle(images, classNo)
images = np.array(images)
classNo = np.array(classNo)
from tensorflow.keras.callbacks import EarlyStopping

early_stopping = EarlyStopping(monitor='val_loss', patience=3)

X_train, X_test, y_train, y_test = train_test_split(images, classNo, test_size=testRatio)
X_train, X_validation, y_train, y_validation = train_test_split(X_train, y_train, test_size=validationRatio)

print("Data Shapes")
print("Train",end = "");print(X_train.shape,y_train.shape)
print("Validation",end = "");print(X_validation.shape,y_validation.shape)
print("Test",end = "");print(X_test.shape,y_test.shape)


data=pd.read_csv(labelFile)
print("data shape ",data.shape,type(data))
 
num_of_samples = []
cols = 5
num_classes = noOfClasses

def grayscale(img):
    img = cv2.cvtColor(img,cv2.COLOR_BGR2GRAY)
    return img
def equalize(img):
    img =cv2.equalizeHist(img)
    return img
def preprocessing(img):
    img = grayscale(img)
    img = equalize(img)
    img = img/255
    return img


X_train=np.array(list(map(preprocessing,X_train)))  
X_validation=np.array(list(map(preprocessing,X_validation)))
X_test=np.array(list(map(preprocessing,X_test)))


X_train=X_train.reshape(X_train.shape[0],X_train.shape[1],X_train.shape[2],1)
X_validation=X_validation.reshape(X_validation.shape[0],X_validation.shape[1],X_validation.shape[2],1)
X_test=X_test.reshape(X_test.shape[0],X_test.shape[1],X_test.shape[2],1)
 
 
dataGen= ImageDataGenerator(width_shift_range=0.1,   
                            height_shift_range=0.1,
                            zoom_range=0.2,  
                            shear_range=0.1,  
                            rotation_range=10)  
dataGen.fit(X_train)
batches= dataGen.flow(X_train,y_train,batch_size=20)
X_batch,y_batch = next(batches)
 

y_train = keras.utils.to_categorical(y_train,noOfClasses)
y_validation = keras.utils.to_categorical(y_validation,noOfClasses)
y_test = keras.utils.to_categorical(y_test,noOfClasses)


def myModel():
    # model = keras.models.Sequential()
    # model.add(keras.layers.Conv2D(32, (3, 3), activation='relu', input_shape=(imageDimesions[0], imageDimesions[1], 1)))
    # model.add(keras.layers.MaxPooling2D((2, 2)))
    # model.add(keras.layers.Flatten())
    # model.add(keras.layers.Dense(64, activation='relu'))
    # model.add(keras.layers.Dense(noOfClasses, activation='softmax'))

    # model.compile(optimizer='adam',
    #             loss='categorical_crossentropy',
    #             metrics=['accuracy'])
    
    model= keras.models.Sequential()
    model.add((keras.layers.Conv2D(60,(5,5),input_shape=(imageDimesions[0],imageDimesions[1],1),activation='relu')))  # ADDING MORE CONVOLUTION LAYERS = LESS FEATURES BUT CAN CAUSE ACCURACY TO INCREASE
    model.add(keras.layers.MaxPooling2D(pool_size=(2,2)))
 
    model.add((keras.layers.Conv2D(30, (3,3),activation='relu')))
    model.add(keras.layers.MaxPooling2D(pool_size=(2,2)))
    model.add(keras.layers.Dropout(0.5))
 
    model.add(keras.layers.Flatten())
    model.add(keras.layers.Dense(500,activation='relu'))
    model.add(keras.layers.Dropout(0.5))
    model.add(keras.layers.Dense(noOfClasses,activation='softmax')) 
    model.compile(keras.optimizers.Adam(learning_rate=0.001),loss='categorical_crossentropy',metrics=['accuracy'])
    return model
 
model = myModel()
print(model.summary())
history=model.fit(dataGen.flow(X_train,y_train,batch_size=batch_size_val),steps_per_epoch=len(X_train)//32,epochs=epochs_val,validation_data=(X_validation,y_validation),shuffle=1,callbacks=[early_stopping])
 
plt.figure(1)
plt.plot(history.history['loss'])
plt.plot(history.history['val_loss'])
plt.legend(['training','validation'])
plt.title('loss')
plt.xlabel('epoch')
plt.figure(2)
plt.plot(history.history['accuracy'])
plt.plot(history.history['val_accuracy'])
plt.legend(['training','validation'])
plt.title('Acurracy')
plt.xlabel('epoch')
plt.show()
score=model.evaluate(X_test,y_test,verbose=0)
print('Test Score:',score[0])
print('Test Accuracy:',score[1])
 
model.save("model.h5")