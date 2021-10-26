#ifndef _POSE_DETECTER_C_H_
#define _POSE_DETECTER_C_H_

#include "common.h"

#ifdef __cplusplus
#include "detecter/detecter.hpp"
extern "C" {
#endif
    typedef void* IDetecter;
    IDetecter new_ultralight();
    void destroy_detecter(IDetecter d);
    int detecter_load_model(IDetecter d, const char* root_path);
    int extract_rois(IDetecter d, const unsigned char* rgbdata,
        int img_width, int img_height,
        ROIVector* rois);
    int extract_keypoints(IDetecter d, const ROI* roi,KeypointVector* keypoints); 
#ifdef __cplusplus
}
#endif
#endif // !_POSE_DETECTER_C_H_
