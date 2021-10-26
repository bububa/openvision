#ifndef _POSE_COMMON_C_H_
#define _POSE_COMMON_C_H_

#include "../common/common.h"
#ifdef __cplusplus
#include "common/common.hpp"
extern "C" {
#endif
#ifdef __cplusplus
typedef ov::Keypoint Keypoint;
typedef ov::ROI ROI;
#else
typedef struct Keypoint {
    Point2f p;
    float prob;
} Keypoint;

typedef struct ROI {
    Rect rect;
    unsigned char *data;
    float score;
} ROI;

#endif
typedef struct ROIVector {
    ROI* items;
    int length;
} ROIVector;

typedef struct KeypointVector {
    Keypoint* points;
    int length;
} KeypointVector;

void FreeKeypointVector(KeypointVector *p);
void FreeROIVector(ROIVector *p);
#ifdef __cplusplus
}
#endif
#endif // !_POSE_COMMON_C_H_
