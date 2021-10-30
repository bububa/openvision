#ifndef _POSE_ULTRALIGHT_H_
#define _POSE_ULTRALIGHT_H_ 

#include "../detecter.hpp"
#include <vector>
#include "net.h"

namespace ovpose {

class Ultralight : public Detecter {
public:
    int Detect(const unsigned char* rgbadata,
        int img_width, int img_height,
        std::vector<ov::ObjectInfo>* rois);

private:
    const float mean_vals[3] = {0.f, 0.f, 0.f};
    const float norm_vals[3] = {1/255.f, 1/255.f, 1/255.f};
};

}

#endif // !_POSE_ULTRALIGHT_H_

