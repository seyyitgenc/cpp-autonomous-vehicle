import numpy as np
import onnxruntime as rt

# Load the ONNX model
sess = rt.InferenceSession("sign.onnx")

# Create a random 32x32 image for testing
# Assuming the image is grayscale, it would have just one channel
# If the image is RGB, it would have three channels and you'd need to use (1, 32, 32, 3)
input_name = sess.get_inputs()[0].name
x = np.random.random((1, 32, 32, 1)).astype(np.float32)

# Run the model
result = sess.run(None, {input_name: x})

# Print the result
print(result)