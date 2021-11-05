#ifndef _POSE_SEGMENTOR_DEEPLABV3PLUS_H_
#define _POSE_SEGMENTOR_DEEPLABV3PLUS_H_

#include "../segmentor.hpp"
#include "net.h"

namespace ovpose {

class Deeplabv3plusSegmentor : public Segmentor  {
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
    const int target_size = 640;
    const float mean_vals[3] = {0.45734706f * 255.f, 0.43338275f * 255.f, 0.40058118f*255.f};
    const float norm_vals[3] = {1/0.23965294/255.f, 1/0.23532275/255.f, 1/0.2398498/255.f};
};

}
#endif // !_POSE_SEGMENTOR_DEEPLABV3PLUS_H_
