#include "handpose.hpp"
#include <string>

#ifdef OV_VULKAN
#include "gpu.h"
#endif // OV_VULKAN

namespace ovhand {

int HandPose::Detect(const unsigned char* rgbdata,
    int img_width, int img_height,
    const ov::Rect& rect,
    std::vector<ov::Point2f>& keypoints) {
    keypoints.clear(); 
	if (!initialized_) {
		return 10000;
	}
	if (rgbdata == 0){
		return 10001;
    }

    size_t total_size = rect.width * rect.height * 3 * sizeof(unsigned char);
    unsigned char* crop_img = (unsigned char*)malloc(total_size);
    const unsigned char *start_ptr = rgbdata;
    for(size_t i = 0; i < rect.height; ++i) {
        const unsigned char* srcCursor = start_ptr + ((i + rect.y) * img_width + rect.x) * 3; 
        unsigned char* dstCursor = crop_img + i * rect.width * 3;
        memcpy(dstCursor, srcCursor, sizeof(unsigned char) * 3 * rect.width);
    }

    ncnn::Mat ncnn_in = ncnn::Mat::from_pixels_resize(crop_img, ncnn::Mat::PIXEL_RGB, rect.width, rect.height, 224, 224);
    const float meanVals[3] = { 128.0f, 128.0f,  128.0f };
    const float normVals[3] = { 0.00390625f, 0.00390625f, 0.00390625f };
    ncnn_in.substract_mean_normalize(meanVals, normVals);
    ncnn::Extractor ex1 = net_->create_extractor();
    ex1.input("input", ncnn_in);
    ncnn::Mat ncnn_out;
    ex1.extract("output", ncnn_out);
    keypoints.resize(21);

    for (int c = 0; c < ncnn_out.c; c++)
    {
        ncnn::Mat data = ncnn_out.channel(c);
        const float *ptr = data.row(0);
        for (size_t j = 0; j < 21; j++)
        {
            float pt_x = ptr[j * 2] * rect.width;
            float pt_y = ptr[j * 2 + 1] * rect.height;
            keypoints[j] = ov::Point2f(pt_x + rect.x, pt_y + rect.y);
        }
    }
    free(crop_img);
    return 0;
}
}