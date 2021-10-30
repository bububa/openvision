#ifndef _SCRFD_DETECT_H_
#define _SCRFD_DETECT_H_

#include "../detecter.hpp"
#include "net.h"

namespace ovface {
class Scrfd : public Detecter {
public:
	int DetectFace(const unsigned char* rgbdata,
        int img_width, int img_height,
        std::vector<FaceInfo>* faces);

private:
    const int target_size = 640;
    const float meanVals[3] = { 123.675f, 116.28f,  103.53f };
    const float normVals[3] = { 0.01712475f, 0.0175f, 0.01742919f };
    const float prob_threshold = 0.5f;
    const float nms_threshold = 0.45f;
    const bool has_kps = false;
};

}

#endif // !_RETINAFACE_H_

