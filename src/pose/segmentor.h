#ifndef _POSE_SEGMENTOR_C_H_
#define _POSE_SEGMENTOR_C_H_

#include "../common/common.h"

#ifdef __cplusplus
#include "segmentor/segmentor.hpp"
extern "C" {
#endif
    typedef void* IPoseSegmentor;
    IPoseSegmentor new_deeplabv3plus_pose_segmentor();
    IPoseSegmentor new_erdnet_pose_segmentor();
    IPoseSegmentor new_rvm_pose_segmentor(int w, int h);
    int pose_segment_matting(IPoseSegmentor s, const unsigned char* rgbdata,
        int img_width, int img_height,
        Image* out);
    int pose_segment_merge(IPoseSegmentor s, const unsigned char* rgbdata,
        int img_width, int img_height,
        const unsigned char* bgdata,
        int bg_width, int bg_height,
        Image* out);
#ifdef __cplusplus
}
#endif
#endif // !_POSE_SEGMENTOR_C_H_
