#ifndef _HAND_DETECTER_YOLOX_H_
#define _HAND_DETECTER_YOLOX_H_

#include "../detecter.hpp"
#include <vector>
#include "net.h"

namespace ovhand {

class Yolox : public Detecter {
public:
    Yolox();
    ~Yolox();

    int LoadModel(const char* root_path);
    int Detect(const unsigned char* rgbadata,
        int img_width, int img_height,
        std::vector<HandROI>* rois);

private:
	ncnn::Net* net_;
    bool initialized_;
};

}
#endif // !_HAND_DETECTER_YOLOX_H_
