# AUTONOMOUS CAR WITH CPP

> This application developed on top of raspberry pi os 64 bit environment on raspberry pi 5 with camera module v3

## BUILD

Dependencies: [OpenCV](https://github.com/opencv/opencv/tree/4.9.0)
[ONNXRuntime](https://github.com/microsoft/onnxruntime/releases) [LCCV](https://github.com/kbarni/LCCV)

## DEBIAN

NOTE-1: You can specify your generator.
NOTE-2: YOU MUST CLONE THIS REPO RECURSIVELY.

```console
    sudo apt install build-essential cmake git libcamera-dev libopencv-dev
    
    git clone https://github.com/seyyitgenc/cpp-autonomous-vehicle.git --recursive
    bash build.sh
    bash run.sh
```

### For traffic sign recognition

- <https://github.com/MarioSamir123/Traffic-Sign-Recognition>

### For lane following code

- <https://github.com/jhoon6/LaneFollower_simulator>

### For onnxruntime

- <https://onnxruntime.ai/docs/get-started/with-cpp.html>

### For LCCV

- <https://github.com/kbarni/LCCV>
