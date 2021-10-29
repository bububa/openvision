#ifndef _HAND_POSE_HANDPOSE_H_
#define _HAND_POSE_HANDPOSE_H_

#include "../estimator.hpp"
#include <vector>
#include "net.h"

namespace ovhand {

class HandPose : public PoseEstimator {
public:
    HandPose();
    ~HandPose();

    int LoadModel(const char* root_path);
    int Detect(const unsigned char* rgbdata,
        int img_width, int img_height,
        const ov::Rect& rect,
        std::vector<ov::Point2f>& keypoints);

private:
	ncnn::Net* net_;
    bool initialized_;
    const float meanVals[3] = { 128.0f, 128.0f,  128.0f };
    const float normVals[3] = { 0.00390625f, 0.00390625f, 0.00390625f };
};

}
#endif // !_HAND_DETECTER_NANODET_H_
