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
    - mtcnn [Google Drive](https://drive.google.com/drive/folders/14ToHyDXZr4Ihuk8WYp1mVS7QnVxnzEjn?usp=sharing)
    - centerface [Google Drive](https://drive.google.com/drive/folders/1xMhO6aCnkkjt90Fh8BxVD_JHB3QJ2q-q?usp=sharing)
    - retainface [Google Drive](https://drive.google.com/drive/folders/1nxR3WFqqEWLwGVsp5c4tI0_iVVEaVOe8?usp=sharing)
    - anticonv (for mask detection) [Google Drive](https://drive.google.com/drive/folders/1Fje0fmVPy5g0_oaxUbH_cAedkgjBf7QW?usp=sharing)
  - recognizer (face feature extration for classification)
    - mobilenet [Google Drive](https://drive.google.com/drive/folders/1fRLs10atm_vwDWQXZ-GJbKQpypNcXLAx?usp=sharing)
  - landmarker (for face landmarkers extraction)
    - insightface [Google Drive](https://drive.google.com/drive/folders/1e_nRwneMEDf_sXEMZCmOk0S4VT0_XpOS?usp=sharing)
    - zq [Google Drive](https://drive.google.com/drive/folders/1ax0J1TVhf2S-B3V6lnqwJaaHUK433sPm?usp=sharing)
  - tracker (for face IOU calculation bettween frames)
  - hopenet (for head pose detection) [Google Drive](https://drive.google.com/drive/folders/1zLam-8s9ZMPDUxUEtNU2F9yFTDRM5fk-?usp=sharing)
- pose
  - detector (for pose estimation)
    - ultralight [Google Drive](https://drive.google.com/drive/folders/15b-I5HDyGe2WLb-TO85SJYmnYONvGOKh?usp=sharing)
- golang binding (github.com/bububa/openvision/go)
