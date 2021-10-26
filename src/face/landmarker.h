#ifndef _FACE_LANDMARKER_C_H_
#define _FACE_LANDMARKER_C_H_

#include "common.h"

#ifdef __cplusplus
#include "landmarker/landmarker.hpp"
extern "C" {
#endif
    typedef void* ILandmarker;
    ILandmarker new_insightface();
    ILandmarker new_zq();
    void destroy_landmarker(ILandmarker m);
    int landmarker_load_model(ILandmarker m, const char* root_path);
    int extract_keypoints(ILandmarker m, const unsigned char* rgbdata, int img_width, int img_height, const Rect* face, Point2fVector* keypoints); 
#ifdef __cplusplus
}
#endif
#endif // !_FACE_LANDMARKER_C_H_
