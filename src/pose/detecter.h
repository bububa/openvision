#ifndef _POSE_DETECTER_C_H_
#define _POSE_DETECTER_C_H_

#include "../common/common.h"

#ifdef __cplusplus
#include "detecter/detecter.hpp"
extern "C" {
#endif
    typedef void* IPoseDetecter;
    IPoseDetecter new_ultralight();
    int detect_pose(IPoseDetecter d, const unsigned char* rgbdata,
        int img_width, int img_height,
        ObjectInfoVector* rois);
#ifdef __cplusplus
}
#endif
#endif // !_POSE_DETECTER_C_H_
