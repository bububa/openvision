#include "mobilefacenet.hpp"
#include <string>
#ifdef OV_VULKAN
#include "gpu.h"
#endif // OV_VULKAN

namespace ovface {

int Mobilefacenet::ExtractFeature(const unsigned char* rgbdata, 
    int img_width, int img_height,
	const ov::Rect & face, std::vector<float>* feature) {
	feature->clear();
	if (!initialized_) {
		return 10000;
	}
	if (rgbdata == 0){
		return 10001;
	}

    size_t total_size = face.width * face.height * 3 * sizeof(unsigned char);
    unsigned char* img_face = (unsigned char*)malloc(total_size);
    const unsigned char *start_ptr = rgbdata;
#if defined(_OPENMP)
#pragma omp parallel for num_threads(num_threads)
#endif
    for(size_t i = 0; i < face.height; ++i) {
        const unsigned char* srcCursor = start_ptr + ((i + face.y) * img_width + face.x) * 3; 
        unsigned char* dstCursor = img_face + i * face.width * 3;
        memcpy(dstCursor, srcCursor, sizeof(unsigned char) * 3 * face.width);
    }

	ncnn::Mat in = ncnn::Mat::from_pixels_resize(img_face,
		ncnn::Mat::PIXEL_RGB, face.width, face.height, 112, 112);
	ncnn::Extractor ex = net_->create_extractor();
    ex.set_light_mode(light_mode_);
    ex.set_num_threads(num_threads);
	ex.input("data", in);
	ncnn::Mat out;
	ex.extract("fc1", out);

	feature->resize(kFaceFeatureDim);
	for (int i = 0; i < kFaceFeatureDim; ++i) {
		feature->at(i) = out[i];
	}

    free(img_face);
	return 0;
}

}


