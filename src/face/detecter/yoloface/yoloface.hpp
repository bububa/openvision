#ifndef _YOLOFACE_H_
#define _YOLOFACE_H_

#include "../detecter.hpp"
#include "net.h"

namespace ovface {
class YoloFace : public Detecter {
public:
	YoloFace();
	~YoloFace();
	int LoadModel(const char* root_path);
	int DetectFace(const unsigned char* rgbdata,
        int img_width, int img_height,
        std::vector<FaceInfo>* faces);

private:
	ncnn::Net* net_;
	bool initialized_;
    const int target_size = 640;
    const float mean_vals[3] = {127.f, 127.f, 127.f};
    const float norm_vals[3] = {1 / 255.f, 1 / 255.f, 1 / 255.f};
    const float prob_threshold = 0.25f;
    const float nms_threshold = 0.45f;
};

}

#endif // !_RETINAFACE_H_

