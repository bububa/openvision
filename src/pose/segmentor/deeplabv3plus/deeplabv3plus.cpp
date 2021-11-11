#include "deeplabv3plus.hpp"

#ifdef OV_VULKAN
#include "gpu.h"
#endif // OV_VULKAN

namespace ovpose {

int Deeplabv3plusSegmentor::Matting(const unsigned char* rgbdata,
    int img_width, int img_height,
    Image* out) {
	if (!initialized_) {
		return 10000;
	}
	if (rgbdata == 0){
		return 10001;
	}

    ncnn::Mat in = ncnn::Mat::from_pixels_resize(rgbdata, ncnn::Mat::PIXEL_RGB, img_width, img_height, target_size, target_size);
    ncnn::Mat matting = ncnn::Mat(target_size, target_size, 3);

    in.substract_mean_normalize(mean_vals, norm_vals);
    ncnn::Extractor ex = net_->create_extractor();
    ex.set_light_mode(light_mode_);
    ex.set_num_threads(num_threads);

    ex.input("input", in);
    ncnn::Mat output;
    ex.extract("output", output);

    const float* pCls0 = output.channel(0);
    const float* pCls1 = output.channel(1);

    for (int c = 0; c < 3; c++)
    {
        float* pImage = matting.channel(c);
        for (int i = 0; i < target_size*target_size; i++){
            pImage[i] = pCls0[i] < pCls1[i] ? 255 : pImage[i] * 0;
        }
    }
    ncnn::Mat outimg;
    ncnn::resize_bicubic(matting, outimg, img_width, img_height);
    out->width = outimg.w;
    out->height = outimg.h;
    out->channels = outimg.c;
    out->data = (unsigned char*)malloc(outimg.total());
    outimg.to_pixels(out->data, ncnn::Mat::PIXEL_RGB); 

    return 0;
}

int Deeplabv3plusSegmentor::Merge(const unsigned char* rgbdata,
    int img_width, int img_height,
    const unsigned char*bgdata,
    int bg_width, int bg_height,
    Image* out) {
	if (!initialized_) {
		return 10000;
	}
	if (rgbdata == 0){
		return 10001;
	}

    ncnn::Mat in = ncnn::Mat::from_pixels_resize(rgbdata, ncnn::Mat::PIXEL_RGB, img_width, img_height, target_size, target_size);
    ncnn::Mat bg = ncnn::Mat::from_pixels_resize(bgdata, ncnn::Mat::PIXEL_RGB, bg_width, bg_height, target_size, target_size);
    ncnn::Mat matting = in.clone(); 

    in.substract_mean_normalize(mean_vals, norm_vals);
    ncnn::Extractor ex = net_->create_extractor();
    ex.set_light_mode(light_mode_);
    ex.set_num_threads(num_threads);

    ex.input("input", in);
    ncnn::Mat output;
    ex.extract("output", output);

    const float* pCls0 = output.channel(0);
    const float* pCls1 = output.channel(1);

    for (int c = 0; c < 3; c++)
    {
        float* pImage = matting.channel(c);
        for (int i = 0; i < target_size*target_size; i++){
            if (pCls0[i] >= pCls1[i]) {
                pImage[i] = bg.channel(c)[i];
            }
        }
    }
    ncnn::Mat outimg;
    ncnn::resize_bicubic(matting, outimg, img_width, img_height);
    out->width = outimg.w;
    out->height = outimg.h;
    out->channels = outimg.c;
    out->data = (unsigned char*)malloc(outimg.total());
    outimg.to_pixels(out->data, ncnn::Mat::PIXEL_RGB); 

    return 0;
}
}
