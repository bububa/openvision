#ifndef _FACE_DETECTER_C_H_
#define _FACE_DETECTER_C_H_

#include "common.h"

#ifdef __cplusplus
#include "detecter/detecter.hpp"
extern "C" {
#endif
    typedef void* IDetecter;
    IDetecter new_retinaface();
    IDetecter new_centerface();
    IDetecter new_mtcnn();
    IDetecter new_anticonv();
    void destroy_detecter(IDetecter d);
    int detecter_load_model(IDetecter d, const char* root_path);
    int detect_face(IDetecter d, const unsigned char* rgbdata, int img_width, int img_height, FaceInfoVector* faces); 
#ifdef __cplusplus
}
#endif
#endif // !_FACE_DETECTER_C_H_
