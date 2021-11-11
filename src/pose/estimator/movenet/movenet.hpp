#ifndef _POSE_MOVENET_ESTIMATOR_H_
#define _POSE_MOVENET_ESTIMATOR_H_ 

#include "../estimator.hpp"
#include <vector>
#include "net.h"

namespace ovpose {

class MoveNet: public Estimator {
public:
    MoveNet(int model_type); // 0: ligntning, 1: thunder
	int ExtractKeypoints(const unsigned char* rgbdata,
        int img_width, int img_height,
        const ov::Rect& rect, 
        std::vector<ov::Keypoint>* keypoints);

private:
    int feature_size;
    float kpt_scale;
    int target_size;
    const int num_joints = 17;
    const float mean_vals[3] = {127.5f, 127.5f,  127.5f};
    const float norm_vals[3] = {1/ 127.5f, 1 / 127.5f, 1 / 127.5f};
    std::vector<std::vector<float>> dist_y, dist_x;
};

}

#endif // !_POSE_MOVENET_ESTIMATOR_H_

