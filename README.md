# libopenvision deep learning visualization C library

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
  - hair (for hair segmentation) [Google Drive](https://drive.google.com/drive/folders/14DOBaFrxTL1k4T1ved5qfRUUziurItT8?usp=sharing)
  - eye
    - lenet (eye status detector) [Google Drive](https://drive.google.com/drive/folders/1jaonx6PeXFLA8gBKo4eQGuxsncVnqS7o?usp=sharing)
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
- golang binding (github.com/bububa/openvision/go)

## Reference

- [MirrorYuChen/ncnn_example](https://github.com/MirrorYuChen/ncnn_example)
- [nihui/ncnn-android-nanodet](https://github.com/nihui/ncnn-android-nanodet)
- [FeiGeChuanShu/ncnn_Android_face](https://github.com/FeiGeChuanShu/ncnn_Android_face)
- [FeiGeChuanShu/ncnn_nanodet_hand](https://github.com/FeiGeChuanShu/ncnn_nanodet_hand)
- [docongminh/deep-head-pose-ncnn](https://github.com/docongminh/deep-head-pose-ncnn)
- [nilseuropa/hopenet_ncnn](https://github.com/nilseuropa/hopenet_ncnn)
- [dog-qiuqiu/Ultralight-SimplePose](https://github.com/dog-qiuqiu/Ultralight-SimplePose)
- [GHLab/deep-head-pose-lite-ncnn](https://github.com/GHLab/deep-head-pose-lite-ncnn)
