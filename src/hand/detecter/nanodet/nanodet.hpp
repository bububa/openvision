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
        std::vector<HandROI>* rois);

private:
	ncnn::Net* net_;
    bool initialized_;
};

}
#endif // !_HAND_DETECTER_NANODET_H_
