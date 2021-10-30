#ifndef _HAND_DETECTER_YOLOX_H_
#define _HAND_DETECTER_YOLOX_H_

#include "../detecter.hpp"
#include <vector>
#include "net.h"

namespace ovhand {

class Yolox : public Detecter {
public:
    int LoadModel(const char * root_path);
    int Detect(const unsigned char* rgbadata,
        int img_width, int img_height,
        std::vector<ov::ObjectInfo>& rois);

private:
    const int target_size = 416;
    const float mean_vals[3] = {255.f * 0.485f, 255.f * 0.456, 255.f * 0.406f};
    const float norm_vals[3] = {1 / (255.f * 0.229f), 1 / (255.f * 0.224f), 1 / (255.f * 0.225f)};
    const float prob_threshold = 0.45f; 
    const float nms_threshold = 0.65f;
};

}
#endif // !_HAND_DETECTER_YOLOX_H_
