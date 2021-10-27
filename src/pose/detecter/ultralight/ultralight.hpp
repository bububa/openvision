#ifndef _POSE_ULTRALIGHT_H_
#define _POSE_ULTRALIGHT_H_ 

#include "../detecter.hpp"
#include <vector>
#include "net.h"

namespace ov {

class Ultralight : public Detecter {
public:
	Ultralight();
	~Ultralight();

	int LoadModel(const char* root_path);
    int ExtractROIs(const unsigned char* rgbadata,
        int img_width, int img_height,
        std::vector<PoseROI>* rois);
	int ExtractKeypoints(const PoseROI& roi, 
        std::vector<PoseKeypoint>* keypoints);

private:
	ncnn::Net* roi_net_;
	ncnn::Net* pose_net_;
	bool initialized_;
};

}

#endif // !_POSE_ULTRALIGHT_H_

