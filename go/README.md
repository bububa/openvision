# Openvision Golang binding 
vision detection/estimation/transformation based on deep learning 

[![Go Reference](https://pkg.go.dev/badge/github.com/bububa/openvision/go.svg)](https://pkg.go.dev/github.com/bububa/openvision/go)
[![GitHub go.mod Go version of a Go module](https://img.shields.io/github/go-mod/go-version/bububa/openvision/go.svg)](https://github.com/bububa/openvision/go)
[![GoReportCard](https://goreportcard.com/badge/github.com/bububa/openvision/go)](https://goreportcard.com/report/github.com/bububa/openvision/go)
[![GitHub license](https://img.shields.io/github/license/bububa/openvision/go.svg)](https://github.com/bububa/openvision/blob/master/go/LICENSE)
[![GitHub release](https://img.shields.io/github/release/bububa/openvision.svg)](https://GitHub.com/bububa/openvision/releases/)


## Prerequest

- ncnn [Install](http://github.com/Tencent/ncnn/wiki/how-to-build)
- openmp
- vulkan(optional)

## Install
```bash
git clone github.com/bububa/openvision.git
cd build
cmake -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang -DNCNN_VULKAN=ON # -DNCNN_VULKAN is optional
make -j 4
# modify go.mod replace github.com/bububa/openvision/go path
```

## Build tags
- vulkan build with vulkan

## Features & models

- face
  - aligner (for face keypoints alignment)
  - detecter (for face location and keypoints detection)
    - mtcnn [Google Drive](https://drive.google.com/drive/folders/14ToHyDXZr4Ihuk8WYp1mVS7QnVxnzEjn?usp=sharing)
    - centerface [Google Drive](https://drive.google.com/drive/folders/1xMhO6aCnkkjt90Fh8BxVD_JHB3QJ2q-q?usp=sharing)
    - retainface [Google Drive](https://drive.google.com/drive/folders/1nxR3WFqqEWLwGVsp5c4tI0_iVVEaVOe8?usp=sharing)
    - yoloface [Google Drive](https://drive.google.com/drive/folders/1EM9H6-aYXKsWTRxx_wbKDyYHVIYpU6f7?usp=sharing)
    - scrfd [Google Drive](https://drive.google.com/drive/folders/1XPjfsuXGj9rXqAmo1K70BsqWmHvoYQv_?usp=sharing)
    - anticonv (for mask detection) [Google Drive](https://drive.google.com/drive/folders/1Fje0fmVPy5g0_oaxUbH_cAedkgjBf7QW?usp=sharing)
  - recognizer (face feature extration for classification)
    - mobilenet [Google Drive](https://drive.google.com/drive/folders/1fRLs10atm_vwDWQXZ-GJbKQpypNcXLAx?usp=sharing)
  - landmarker (for face landmarkers extraction)
    - insightface [Google Drive](https://drive.google.com/drive/folders/1e_nRwneMEDf_sXEMZCmOk0S4VT0_XpOS?usp=sharing)
    - zq [Google Drive](https://drive.google.com/drive/folders/1ax0J1TVhf2S-B3V6lnqwJaaHUK433sPm?usp=sharing)
    - scrfd [Google Drive](https://drive.google.com/drive/folders/1XPjfsuXGj9rXqAmo1K70BsqWmHvoYQv_?usp=sharing)
  - tracker (for face IOU calculation bettween frames)
  - hopenet (for head pose detection) [Google Drive](https://drive.google.com/drive/folders/1zLam-8s9ZMPDUxUEtNU2F9yFTDRM5fk-?usp=sharing)
- pose
  - detector (for pose detection/estimation)
    - ultralight [Google Drive](https://drive.google.com/drive/folders/15b-I5HDyGe2WLb-TO85SJYmnYONvGOKh?usp=sharing)
    - openpose [Google Drive](https://drive.google.com/drive/folders/1Q2mq7dOE-eHsvu4BYpBaWVLkU5roKsm5?usp=sharing)
  - estimator (for pose estimation)
    - ultralight [Google Drive](https://drive.google.com/drive/folders/15b-I5HDyGe2WLb-TO85SJYmnYONvGOKh?usp=sharing)
    - movenet [Google Drive](https://drive.google.com/drive/folders/14zgKk0tro1kjRrSTs0EAlEKrV8Q4XA34?usp=sharing)
  - segmentor (for pose segmentation)
    - deeplabv3plus [Google Drive](https://drive.google.com/drive/folders/1BjiK0IiVAfyX30OoyQzoe1YBzvSudghG?usp=sharing)
    - erdnet [Google Drive](https://drive.google.com/drive/folders/1WkQqYT9D4oGL6Gubu0SOeqcZmrdZ5cNw?usp=sharing)
    - rvm (RobustVideoMatting) [Google Drive](https://drive.google.com/drive/folders/1roV7L8Z3OIC4C7CKQcm_KJ_CPHeTJug5?usp=sharing)
- hand
  - detector (for hand detect)
    - yolox [Google Drive](https://drive.google.com/drive/folders/1lNm5X6DJ1ZXVaqg54rXnRhvPfC5lAxlH?usp=sharing)
    - nanodet [Google Drive](https://drive.google.com/drive/folders/1ywH7r_clqqA_BAOFSzA92Q0lxJtWlN3z?usp=sharing)
  - pose (for hand pose estimation)
    - handnet [Google Drive](https://drive.google.com/drive/folders/1DsCGmiVaZobbMWRp5Oec8GbIpeg7CsNR?usp=sharing)
- styletransfer
  - animegan2 [Google Drive](https://drive.google.com/drive/folders/1K6ZScENPHVbxupHkwl5WcpG8PPECtD8e?usp=sharing)
- tracker
  - lighttrack [Google Drive](https://drive.google.com/drive/folders/16cxns_xzSOABHn6UcY1OXyf4MFcSSbEf?usp=sharing)
- counter
  - p2pnet [Google Drive](https://drive.google.com/drive/folders/1kmtBsPIS79C3hMAwm_Tv9tAPvJLV9k35?usp=sharing)
