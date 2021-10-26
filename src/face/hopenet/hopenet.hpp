#ifndef _ROS_NCNN_HOPENET_H_
#define _ROS_NCNN_HOPENET_H_

#include "../../common/common.h"
#include "net.h"

namespace ov {

class HopeNet {
public:
    HopeNet();
    ~HopeNet();
	int LoadModel(const char* root_path);
    int Detect(const unsigned char* rgbdata, 
        int img_width, int img_height,
        Rect roi, HeadPose* euler_angles);

private:
    ncnn::Net* net_;
    bool initialized_;
    float idx_tensor[66];
    void softmax(float* z, size_t el);
    double getAngle(float* prediction, size_t len);

};

}

#endif
