from tensorflow.keras.models import load_model
import numpy as np

# Load the model
model = load_model('/home/seyyit/projects/cpp-autonomous-vehicle/model.h5')

# Assume we have some data in a numpy array
# Reshape it to match the input shape of the model
data = np.random.rand(1, 32, 32, 1)

# Use the model to make a prediction
prediction = model.predict(data)

print(prediction)