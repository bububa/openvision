#ifndef _FACE_COMMON_H_
#define _FACE_COMMON_H_

#include "../../common/common.h"
#include <vector>

namespace ovface {
#define kFaceFeatureDim 128
#define kFaceNameDim 256
struct FaceInfo {
    ov::Rect location_;
	float score_;
	float keypoints_[10];
    bool mask_;
};

struct TrackedFaceInfo {
	FaceInfo face_info_;
	float iou_score_;
};

struct HeadPose
{
    float roll;
    float pitch;
    float yaw;
};
}

float CalculateSimilarity(const std::vector<float>&feature1, const std::vector<float>& feature2);
#endif // !_FACE_COMMON_H_
