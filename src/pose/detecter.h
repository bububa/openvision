#ifndef _POSE_DETECTER_C_H_
#define _POSE_DETECTER_C_H_

#include "common.h"

#ifdef __cplusplus
#include "detecter/detecter.hpp"
extern "C" {
#endif
    typedef void* IPoseDetecter;
    IPoseDetecter new_ultralight();
    int extract_pose_rois(IPoseDetecter d, const unsigned char* rgbdata,
        int img_width, int img_height,
        PoseROIVector* rois);
    int extract_pose_keypoints(IPoseDetecter d, const PoseROI* roi, PoseKeypointVector* keypoints); 
#ifdef __cplusplus
}
#endif
#endif // !_POSE_DETECTER_C_H_
