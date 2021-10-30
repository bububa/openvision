#include "../estimator.h"
#include "ultralight/ultralight.hpp"

IPoseEstimator new_ultralight_estimator() {
    return new ovpose::UltralightEstimator();
}

int extract_pose_keypoints(IPoseEstimator d, const unsigned char* rgbdata, int img_width, int img_height, const Rect* rect, KeypointVector* keypoints) {
    std::vector<ov::Keypoint> points;
    int ret = static_cast<ovpose::Estimator*>(d)->ExtractKeypoints(rgbdata, img_width, img_height, *rect, &points);
    if (ret != 0) {
        return ret;
    }
    keypoints->length = points.size();
    keypoints->points = (Keypoint*)malloc(keypoints->length * sizeof(Keypoint));
    for (size_t i = 0; i < points.size(); ++i) {
        keypoints->points[i] = points[i];
    }
    return 0;
}

namespace ovpose {
Estimator* UltralightEstimatorFactory::CreateEstimator() {
    return new UltralightEstimator();
}
}