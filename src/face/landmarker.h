#ifndef _FACE_LANDMARKER_C_H_
#define _FACE_LANDMARKER_C_H_

#include "common.h"

#ifdef __cplusplus
#include "landmarker/landmarker.hpp"
extern "C" {
#endif
    typedef void* IFaceLandmarker;
    IFaceLandmarker new_insightface();
    IFaceLandmarker new_zq();
    IFaceLandmarker new_scrfd_landmarker();
    int extract_face_keypoints(IFaceLandmarker m, const unsigned char* rgbdata, int img_width, int img_height, const Rect* face, Point2fVector* keypoints); 
#ifdef __cplusplus
}
#endif
#endif // !_FACE_LANDMARKER_C_H_
