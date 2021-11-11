#ifndef _POSE_SEGMENTOR_RVM_H_
#define _POSE_SEGMENTOR_RVM_H_

#include "../segmentor.hpp"
#include "net.h"

namespace ovpose {

class RVM : public Segmentor  {
public:
    RVM(int width = 640, int height = 480); // model width/height;
    virtual int Matting(const unsigned char* rgbdata,
        int img_width, int img_height,
        Image* out);
    virtual int Merge(const unsigned char* rgbdata,
        int img_width, int img_height,
        const unsigned char* bgdata,
        int bg_width, int bg_height,
        Image* out);
private:
    // will be update inner video matting process.
    ncnn::Mat r1i, r2i, r3i, r4i;
    int target_width = 640;
    int target_height = 480;
    const float mean_vals[3] = {0.f, 0.f, 0.f};
    const float norm_vals[3] = {1.f / 255.f, 1.f / 255.f, 1.f / 255.f};
    const float mean_vals1[3] = {123.675f, 116.28f,  103.53f};
    const float norm_vals1[3] = {0.01712475f, 0.0175f, 0.01742919f};
    bool context_is_update = false;

    void update_context(ncnn::Extractor& ex);
};

}
#endif // !_POSE_SEGMENTOR_RVM_H_
