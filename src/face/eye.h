#ifndef _FACE_EYE_C_H_
#define _FACE_EYE_C_H_

#include "common.h"

#ifdef __cplusplus
#include "eye/eye.hpp"
extern "C" {
#endif
typedef void *IEye;
IEye new_lenet_eye();
int eye_status(IEye d, const unsigned char *rgbdata, int img_width,
               int img_height, const Rect *rect, FloatVector *cls_scores);
#ifdef __cplusplus
}
#endif
#endif // !_FACE_HAIR_C_H_
