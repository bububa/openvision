# libopenvision deep learning visualization C liberay

## Prerequest

- ncnn [Install](http://github.com/Tencent/ncnn/wiki/how-to-build)
- openmp
- vulkan(optional)

## Build

```bash
git submodule update --init --recursuve
cd build
cmake .. # optional -DNCNN_VULKAN=OFF -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang
```

## Features

- face
  - detecter (for face location and keypoints detection)
    - mtcnn
    - centerface
    - retainface
    - anticonv (for mask detection)
  - recognizer (face feature extration for classification)
    - mobilenet
  - landmarker (for face landmarkers extraction)
    - insightface
    - zq
  - tracker (for face IOU calculation bettween frames)
  - hopenet (for head pose detection)
- pose
- detector (for pose estimation)
  - ultralight
- golang binding (github.com/bububa/openvision/go)
