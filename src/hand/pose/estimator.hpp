#ifndef _HAND_POSE_H_
#define _HAND_POSE_H_

#include <vector>
#include "../common/common.hpp"

namespace ovhand {
class PoseEstimator: public ov::Estimator {
public:
    virtual ~PoseEstimator() {};
    virtual int Detect(const unsigned char*rgbdata,
        int img_width, int img_height,
        const ov::Rect& rect,
        std::vector<ov::Point2f>* keypoints) = 0;
};

class PoseEstimatorFactory {
public:
    virtual PoseEstimator* CreateEstimator() = 0;
    virtual ~PoseEstimatorFactory() {};
};

}
#endif // !_HAND_POSE_H_
