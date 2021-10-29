#ifndef _HAND_DETECTER_NANODET_H_
#define _HAND_DETECTER_NANODET_H_

#include "../detecter.hpp"
#include <vector>
#include "net.h"

namespace ovhand {

class Nanodet : public Detecter {
public:
    Nanodet();
    ~Nanodet();

    int LoadModel(const char* root_path);
    int Detect(const unsigned char* rgbadata,
        int img_width, int img_height,
        std::vector<ov::ObjectInfo>& rois);

private:
	ncnn::Net* net_;
    bool initialized_;
    const int target_size = 320;
    const float mean_vals[3] = {103.53f, 116.28f, 123.675f};
    const float norm_vals[3] = {1.f / 57.375f, 1.f / 57.12f, 1.f / 58.395f};
    const float prob_threshold = 0.4f; 
    const float nms_threshold = 0.5f;
};

}
#endif // !_HAND_DETECTER_NANODET_H_
