#ifndef _POSE_SEGMENTOR_ERDNET_H_
#define _POSE_SEGMENTOR_ERDNET_H_

#include "../segmentor.hpp"
#include "net.h"

namespace ovpose {

class ERDNetSegmentor : public Segmentor  {
public:
    virtual int Matting(const unsigned char* rgbdata,
        int img_width, int img_height,
        Image* out);
    virtual int Merge(const unsigned char* rgbdata,
        int img_width, int img_height,
        const unsigned char* bgdata,
        int bg_width, int bg_height,
        Image* out);
private:
    const int target_size = 256;
    const float mean_vals[3] = {104.f, 112.f, 121.f};
    const float norm_vals[3] = {1.f/255.f, 1.f/255.f, 1.f/255.f};
};

}
#endif // !_POSE_SEGMENTOR_ERDNET_H_
