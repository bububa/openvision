#ifndef _POSE_ESTIMATOR_C_H_
#define _POSE_ESTIMATOR_C_H_

#include "../common/common.h"

#ifdef __cplusplus
#include "estimator/estimator.hpp"
extern "C" {
#endif
    typedef void* IPoseEstimator;
    IPoseEstimator new_ultralight_estimator();
    int extract_pose_keypoints(IPoseEstimator d, const unsigned char* rgbdata,
        int img_width, int img_height,
        const Rect* rect, KeypointVector* keypoints); 
#ifdef __cplusplus
}
#endif
#endif // !_POSE_ESTIMATOR_C_H_
