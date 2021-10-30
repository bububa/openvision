#ifndef _POSE_ULTRALIGHT_ESTIMATOR_H_
#define _POSE_ULTRALIGHT_ESTIMATOR_H_ 

#include "../estimator.hpp"
#include <vector>
#include "net.h"

namespace ovpose {

class UltralightEstimator: public Estimator {
public:
	int ExtractKeypoints(const unsigned char* rgbdata,
        int img_width, int img_height,
        const ov::Rect& rect, 
        std::vector<ov::Keypoint>* keypoints);

private:
    const float meanVals[3] = {0.485f * 255.f, 0.456f * 255.f, 0.406f * 255.f};
    const float normVals[3] = {1 / 0.229f / 255.f, 1 / 0.224f / 255.f, 1 / 0.225f / 255.f};
};

}

#endif // !_POSE_ULTRALIGHT_ESTIMATOR_H_

