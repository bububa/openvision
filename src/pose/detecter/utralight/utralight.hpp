#ifndef _POSE_UTRALIGHT_H_
#define _POSE_UTRALIGHT_H_ 

#include "../detecter.hpp"
#include <vector>
#include "net.h"

namespace ov {

class Utralight : public Detecter {
public:
	Utralight();
	~Utralight();

	int LoadModel(const char* root_path);
    int ExtractROIs(const unsigned char* rgbadata,
        int img_width, int img_height,
        std::vector<ROI>* rois);
	int ExtractKeypoints(const ROI& roi, 
        std::vector<Keypoint>* keypoints);

private:
	ncnn::Net* roi_net_;
	ncnn::Net* pose_net_;
	bool initialized_;
};

}

#endif // !_POSE_MOBILEFACENET_H_

