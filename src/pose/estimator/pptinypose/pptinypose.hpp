#ifndef _POSE_PPTINYPOSE_ESTIMATOR_H_
#define _POSE_PPTINYPOSE_ESTIMATOR_H_

#include "../estimator.hpp"
#include "net.h"
#include <vector>

namespace ovpose {
class PPTinyPoseEstimator : public Estimator {
public:
  PPTinyPoseEstimator(int target_size);
  int ExtractKeypoints(const unsigned char *rgbdata, int img_width,
                       int img_height, const ov::Rect &rect,
                       std::vector<ov::Keypoint> *keypoints);

private:
  int target_width_ = 96;
  int target_height_ = 128;
  const float meanVals[3] = {123.675f, 116.28f, 103.53f};
  const float normVals[3] = {0.01712475f, 0.0175f, 0.01742919f};
};

} // namespace ovpose

#endif // !_POSE_PPTINYPOSE_ESTIMATOR_H_
