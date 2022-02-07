#ifndef _FACE_HOPENET_C_H_
#define _FACE_HOPENET_C_H_

#include "common.h"

#ifdef __cplusplus
#include "hopenet/hopenet.hpp"
extern "C" {
#endif
typedef void *IHopenet;
IHopenet new_hopenet();
int hopenet_detect(IHopenet d, const unsigned char *rgbdata, int img_width,
                   int img_height, const Rect *roi, HeadPose *euler_angles);
#ifdef __cplusplus
}
#endif
#endif // !_FACE_HOPENET_C_H_
