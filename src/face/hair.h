#ifndef _FACE_HAIR_C_H_
#define _FACE_HAIR_C_H_

#include "common.h"

#ifdef __cplusplus
#include "hair/hair.hpp"
extern "C" {
#endif
typedef void *IHair;
IHair new_hair();
int hair_matting(IHair d, const unsigned char *rgbdata, int img_width,
                 int img_height, Image *out);
#ifdef __cplusplus
}
#endif
#endif // !_FACE_HAIR_C_H_
