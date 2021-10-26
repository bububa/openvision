#ifndef _ROS_NCNN_HOPENET_C_H_
#define _ROS_NCNN_HOPENET_C_H_

#include "common.h"

#ifdef __cplusplus
#include "hopenet/hopenet.hpp"
extern "C" {
#endif
    typedef void* IHopeNet;
    IHopeNet new_hopenet();
    void destroy_hopenet(IHopeNet h);
    int hopenet_load_model(IHopeNet h, const char* root_path);
    int hopenet_detect(IHopeNet h, const unsigned char* rgbdata, int img_width, int img_height, const Rect* roi, HeadPose* euler_angles);
#ifdef __cplusplus
}
#endif
#endif
