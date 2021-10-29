#ifndef _POSE_ULTRALIGHT_H_
#define _POSE_ULTRALIGHT_H_ 

#include "../detecter.hpp"
#include <vector>
#include "net.h"

namespace ovpose {

class Ultralight : public Detecter {
public:
	Ultralight();
	~Ultralight();

	int LoadModel(const char* root_path);
    int ExtractROIs(const unsigned char* rgbadata,
        int img_width, int img_height,
        std::vector<ov::ObjectInfo>* rois);
	int ExtractKeypoints(const unsigned char* rgbdata,
        int img_width, int img_height,
        const ov::Rect& rect, 
        std::vector<ov::Keypoint>* keypoints);

private:
	ncnn::Net* roi_net_;
	ncnn::Net* pose_net_;
	bool initialized_;
    const float mean_vals[3] = {0.f, 0.f, 0.f};
    const float norm_vals[3] = {1/255.f, 1/255.f, 1/255.f};
    const float meanVals[3] = {0.485f * 255.f, 0.456f * 255.f, 0.406f * 255.f};
    const float normVals[3] = {1 / 0.229f / 255.f, 1 / 0.224f / 255.f, 1 / 0.225f / 255.f};
};

}

#endif // !_POSE_ULTRALIGHT_H_

