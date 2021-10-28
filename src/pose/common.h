#ifndef _POSE_COMMON_C_H_
#define _POSE_COMMON_C_H_

#include "../common/common.h"
#ifdef __cplusplus
#include "common/common.hpp"
extern "C" {
#endif
#ifdef __cplusplus
typedef ovpose::PoseKeypoint PoseKeypoint;
typedef ovpose::PoseROI PoseROI;
#else
typedef struct PoseKeypoint {
    Point2f p;
    float prob;
} PoseKeypoint;

typedef struct PoseROI {
    Rect rect;
    unsigned char *data;
    float score;
} PoseROI;

#endif
typedef struct PoseROIVector {
    PoseROI* items;
    int length;
} PoseROIVector;

typedef struct PoseKeypointVector {
    PoseKeypoint* points;
    int length;
} PoseKeypointVector;

void FreePoseKeypointVector(PoseKeypointVector *p);
void FreePoseROI(PoseROI *p);
void FreePoseROIVector(PoseROIVector *p);
#ifdef __cplusplus
}
#endif
#endif // !_POSE_COMMON_C_H_
