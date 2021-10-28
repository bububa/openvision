#include "mobilefacenet.hpp"
#include <string>
#ifdef OV_VULKAN
#include "gpu.h"
#endif // OV_VULKAN

namespace ovface {
Mobilefacenet::Mobilefacenet() {
	mobileface_net_ = new ncnn::Net();
	initialized_ = false;
#ifdef OV_VULKAN
    mobileface_net_->opt.use_vulkan_compute = true;
#endif // OV_VULKAN
}

Mobilefacenet::~Mobilefacenet() {
	mobileface_net_->clear();
}

int Mobilefacenet::LoadModel(const char * root_path) {
	std::string param_file = std::string(root_path) + "/param";
	std::string bin_file = std::string(root_path) + "/bin";
	if (mobileface_net_->load_param(param_file.c_str()) == -1 ||
		mobileface_net_->load_model(bin_file.c_str()) == -1) {
		return 10000;
	}

	initialized_ = true;
	return 0;
}

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
    for(size_t i = 0; i < face.height; ++i) {
        const unsigned char* srcCursor = start_ptr + ((i + face.y) * img_width + face.x) * 3; 
        unsigned char* dstCursor = img_face + i * face.width * 3;
        memcpy(dstCursor, srcCursor, sizeof(unsigned char) * 3 * face.width);
    }

	ncnn::Mat in = ncnn::Mat::from_pixels_resize(img_face,
		ncnn::Mat::PIXEL_RGB, face.width, face.height, 112, 112);
	feature->resize(kFaceFeatureDim);
	ncnn::Extractor ex = mobileface_net_->create_extractor();
	ex.input("data", in);
	ncnn::Mat out;
	ex.extract("fc1", out);
	for (int i = 0; i < kFaceFeatureDim; ++i) {
		feature->at(i) = out[i];
	}

    free(img_face);
	return 0;
}

}


