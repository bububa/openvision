#ifndef _REALESRGAN_H
#define _REALESRGAN_H

#include "../common/common.hpp"
// ncnn
#include "net.h"
#include "layer.h"
#ifdef OV_VULKAN
#include "gpu.h"
#endif

namespace ov {

class RealESRGAN: public Estimator {
public:
    RealESRGAN(int gpuid, bool tta_mode = false);
    ~RealESRGAN();
	int LoadModel(const char* root_path);
    int process(const ncnn::Mat& inimage, ncnn::Mat& outimage) const;

public:
    // realesrgan parameters
    int scale;
    int tilesize;
    int prepadding;

private:
    ncnn::Net* net = nullptr;
#ifdef OV_VULKAN
    ncnn::Pipeline* realesrgan_preproc;
    ncnn::Pipeline* realesrgan_postproc;
#endif
    ncnn::Layer* bicubic_4x;
    bool tta_mode;
    bool initialized_;
};

}

#endif // _REALESRGAN_H
