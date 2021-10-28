#ifndef _FACE_DETECTER_C_H_
#define _FACE_DETECTER_C_H_

#include "common.h"

#ifdef __cplusplus
#include "detecter/detecter.hpp"
extern "C" {
#endif
    typedef void* IFaceDetecter;
    IFaceDetecter new_retinaface();
    IFaceDetecter new_centerface();
    IFaceDetecter new_mtcnn();
    IFaceDetecter new_anticonv();
    int detect_face(IFaceDetecter d, const unsigned char* rgbdata, int img_width, int img_height, FaceInfoVector* faces); 
#ifdef __cplusplus
}
#endif
#endif // !_FACE_DETECTER_C_H_
