#include "ultralight.hpp"
#include <string>

#ifdef OV_VULKAN
#include "gpu.h"
#endif // OV_VULKAN

namespace ovpose {

int UltralightEstimator::ExtractKeypoints(const unsigned char* rgbdata, 
    int img_width, int img_height,
    const ov::Rect& rect, std::vector<ov::Keypoint>* keypoints) {
	if (!initialized_) {
		return 10000;
	}
	if (rgbdata == 0){
		return 10001;
	}
    keypoints->clear();
    int w = rect.width;
    int h = rect.height;
    
    size_t total_size = w * h * 3 * sizeof(unsigned char);
    unsigned char* data = (unsigned char*)malloc(total_size);
    const unsigned char *start_ptr = rgbdata;
#if defined(_OPENMP)
#pragma omp parallel for num_threads(num_threads)
#endif
    for(size_t i = 0; i < h; ++i) {
        const unsigned char* srcCursor = start_ptr + ((i + rect.y) * img_width + rect.x) * 3; 
        unsigned char* dstCursor = data + i * w * 3;
        memcpy(dstCursor, srcCursor, sizeof(unsigned char) * 3 * w);
    }
    ncnn::Mat in = ncnn::Mat::from_pixels_resize(data, ncnn::Mat::PIXEL_RGB, w, h, 192, 256);
    //数据预处理
    in.substract_mean_normalize(meanVals, normVals);

    ncnn::Extractor ex = net_->create_extractor();
    ex.set_light_mode(light_mode_);
    ex.set_num_threads(num_threads);
    ex.input("data", in);
    ncnn::Mat out;
    ex.extract("hybridsequential0_conv7_fwd", out);
    for (int p = 0; p < out.c; p++)
    {
        const ncnn::Mat m = out.channel(p);

        float max_prob = 0.f;
        int max_x = 0;
        int max_y = 0;
        for (int y = 0; y < out.h; y++)
        {
            const float* ptr = m.row(y);
            for (int x = 0; x < out.w; x++)
            {
                float prob = ptr[x];
                if (prob > max_prob)
                {
                    max_prob = prob;
                    max_x = x;
                    max_y = y;
                }
            }
        }

        ov::Keypoint keypoint;
        keypoint.p = ov::Point2f(max_x * w / (float)out.w+rect.x, max_y * h / (float)out.h+rect.y);
        keypoint.score = max_prob;
        keypoints->push_back(keypoint);
    }

    free(data);
    return 0;
}

}
