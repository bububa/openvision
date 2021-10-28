#ifndef _FACE_COMMON_C_H_
#define _FACE_COMMON_C_H_

#include "../common/common.h"
#ifdef __cplusplus
#include "common/common.hpp"
extern "C" {
#endif

#ifdef __cplusplus
typedef ovface::FaceInfo FaceInfo;
typedef ovface::TrackedFaceInfo TrackedFaceInfo;
typedef ovface::HeadPose HeadPose;
#else
typedef struct FaceInfo {
    Rect location_;
    float score_;
    float keypoints_[10];
    bool mask_;
} FaceInfo;

typedef struct TrackedFaceInfo {
    FaceInfo face_info_;
    float iou_score_;
} TrackedFaceInfo;

typedef struct HeadPose {
    float roll;
    float pitch;
    float yaw;
} HeadPose;
#endif

typedef struct FaceInfoVector {
    FaceInfo* faces;
    int length;
} FaceInfoVector;

typedef struct TrackedFaceInfoVector {
    TrackedFaceInfo* faces;
    int length;
} TrackedFaceInfoVector;

void FreeFaceInfoVector(FaceInfoVector *p);
void NewFaceInfoVector(FaceInfoVector *v, int num);
void FreeTrackedFaceInfoVector(TrackedFaceInfoVector *p);
#ifdef __cplusplus
}
#endif
#endif // !_FACE_COMMON_C_H_
