#include "zqlandmarker.hpp"
#include <string>

#if MIRROR_VULKAN
#include "gpu.h"
#endif // MIRROR_VULKAN

namespace mirror {
ZQLandmarker::ZQLandmarker() {
	zq_landmarker_net_ = new ncnn::Net();
	initialized = false;
#if MIRROR_VULKAN
	ncnn::create_gpu_instance();	
    zq_landmarker_net_->opt.use_vulkan_compute = true;
#endif // MIRROR_VULKAN
}

ZQLandmarker::~ZQLandmarker() {
	zq_landmarker_net_->clear();
#if MIRROR_VULKAN
	ncnn::destroy_gpu_instance();
#endif // MIRROR_VULKAN	
}

int ZQLandmarker::LoadModel(const char * root_path) {
	std::string fl_param = std::string(root_path) + "/param";
	std::string fl_bin = std::string(root_path) + "/bin";
	if (zq_landmarker_net_->load_param(fl_param.c_str()) == -1 ||
		zq_landmarker_net_->load_model(fl_bin.c_str()) == -1) {
		return 10000;
	}
	initialized = true;
	return 0;
}

int ZQLandmarker::ExtractKeypoints(const unsigned char* rgbdata, 
    int img_width, int img_height,
	const Rect & face, std::vector<Point2f>* keypoints) {
	keypoints->clear();
	if (!initialized) {
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
	ncnn::Extractor ex = zq_landmarker_net_->create_extractor();
	ncnn::Mat in = ncnn::Mat::from_pixels_resize(img_face,
		ncnn::Mat::PIXEL_RGB, face.width, face.height, 112, 112);
	in.substract_mean_normalize(meanVals, normVals);
	ex.input("data", in);
	ncnn::Mat out;
	ex.extract("bn6_3", out);

	for (int i = 0; i < 106; ++i) {
		float x = abs(out[2 * i] * face.width) + face.x;
		float y = abs(out[2 * i + 1] * face.height) + face.y;
		keypoints->push_back(Point2f(x, y));
	}


    free(img_face);
	return 0;
}

}
