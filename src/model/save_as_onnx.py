import tensorflow as tf
import tf2onnx
import onnx

# Load Keras model
model = tf.keras.models.load_model('C:\projects_ai\Traffic-Sign-Recognition\model.h5')

model_name = "sign"
input_signature = [tf.TensorSpec(model.inputs[0].shape, model.inputs[0].dtype, name='sign')]

model.output_names=['output']

onnx_model, _ = tf2onnx.convert.from_keras(model, input_signature, opset=13)
onnx.save(onnx_model, model_name + ".onnx")