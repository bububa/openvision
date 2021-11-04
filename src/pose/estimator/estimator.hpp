#ifndef _POSE_ESTIMATOR_H_
#define _POSE_ESTIMATOR_H_

#include "../common/common.hpp"
#include <vector>
namespace ovpose {

class Estimator: public ov::Estimator {
public:
    virtual int ExtractKeypoints(const unsigned char* rgbdata,
        int img_width, int img_height,
        const ov::Rect& rect, std::vector<ov::Keypoint>* keypoints) = 0;
};

class EstimatorFactory {
public:
    virtual Estimator* CreateEstimator() = 0;
    virtual ~EstimatorFactory() {};
};

class UltralightEstimatorFactory: public EstimatorFactory {
public:
    UltralightEstimatorFactory() {}
    ~UltralightEstimatorFactory() {}
    Estimator* CreateEstimator();
};

class MoveNetFactory: public EstimatorFactory {
public:
    MoveNetFactory(const int model_type) {}
    ~MoveNetFactory() {}
    Estimator* CreateEstimator(const int model_type);
};
}
#endif // !_POSE_ESTIMATOR_H
