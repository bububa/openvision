#ifndef _HAND_POSE_C_H_
#define _HAND_POSE_C_H_

#include "common.h"

#ifdef __cplusplus
#include "pose/estimator.hpp"
extern "C" {
#endif
    typedef void* IHandPoseEstimator;
    IHandPoseEstimator new_handpose();
    int hand_pose(IHandPoseEstimator d, const unsigned char* rgbdata,
        int img_width, int img_height,
        const Rect* rect,
        Point2fVector* keypoints);
#ifdef __cplusplus
}
#endif
#endif // !_HAND_DETECTER_C_H_
