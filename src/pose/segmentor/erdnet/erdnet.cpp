#include "erdnet.hpp"

#ifdef OV_VULKAN
#include "gpu.h"
#endif // OV_VULKAN

namespace ovpose {

int ERDNetSegmentor::Matting(const unsigned char* rgbdata,
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

    ex.input("input_blob1", in);
    ncnn::Mat output;
    ex.extract("sigmoid_blob1", output);

    for (int c = 0; c < 3; ++c) {
        float* pImage = matting.channel(c);
        for (int i = 0; i < target_size*target_size; i++) {
            const float alpha = output[i];
            float value = 255 * alpha;
            value = std::max(std::min(value, 255.f), 0.f);
            pImage[i] = value;
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

int ERDNetSegmentor::Merge(const unsigned char* rgbdata,
    int img_width, int img_height,
    const unsigned char* bgdata,
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

    ex.input("input_blob1", in);
    ncnn::Mat output;
    ex.extract("sigmoid_blob1", output);

    for (int c = 0; c < 3; ++c) {
        float* pImage = matting.channel(c);
        for (int i = 0; i < target_size*target_size; i++) {
            const float alpha = output[i];
            float value = pImage[i] * alpha + bg.channel(c)[i] * (1 - alpha);
            value = std::max(std::min(value, 255.f), 0.f);
            pImage[i] = value;
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
