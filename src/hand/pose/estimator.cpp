#include "../pose.h"
#include "handpose/handpose.hpp"

IHandPoseEstimator new_handpose() {
    return new ovhand::HandPose();
}


int hand_pose(IHandPoseEstimator d, const unsigned char* rgbdata,
    int img_width, int img_height,
    const Rect* rect,
    Point2fVector* keypoints) {
    std::vector<ov::Point2f>points;

    int ret = static_cast<ovhand::HandPose*>(d)->Detect(rgbdata, img_width, img_height, *rect, &points);
    if (ret != 0) {
        return ret;
    }
    keypoints->length = points.size();
    keypoints->points= (ov::Point2f*)malloc(keypoints->length * sizeof(ov::Point2f));
    for (size_t i = 0; i < keypoints->length; ++i) {
        keypoints->points[i] = points.at(i);
    }
    return 0;
}
