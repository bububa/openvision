#include "insightface.hpp"
#include <string>
#include "../../common/common.hpp"

#if MIRROR_VULKAN
#include "gpu.h"
#endif // MIRROR_VULKAN

namespace mirror {
InsightfaceLandmarker::InsightfaceLandmarker() {
	insightface_landmarker_net_ = new ncnn::Net();
	initialized = false;
#if MIRROR_VULKAN
	ncnn::create_gpu_instance();	
    insightface_landmarker_net_->opt.use_vulkan_compute = true;
#endif // MIRROR_VULKAN
}

InsightfaceLandmarker::~InsightfaceLandmarker() {
	insightface_landmarker_net_->clear();
#if MIRROR_VULKAN
	ncnn::destroy_gpu_instance();
#endif // MIRROR_VULKAN	
}

int InsightfaceLandmarker::LoadModel(const char * root_path) {
	std::string fl_param = std::string(root_path) + "/param";
	std::string fl_bin = std::string(root_path) + "/bin";
	if (insightface_landmarker_net_->load_param(fl_param.c_str()) == -1 ||
		insightface_landmarker_net_->load_model(fl_bin.c_str()) == -1) {
		return 10000;
	}
	initialized = true;
	return 0;
}

int InsightfaceLandmarker::ExtractKeypoints(const unsigned char* rgbdata, 
    int img_width, int img_height,
	const Rect & face, std::vector<Point2f>* keypoints) {
	keypoints->clear();
	if (!initialized) {
		return 10000;
	}

	if (rgbdata == 0){
		return 10001;
	}

	// 1 enlarge the face rect
	Rect face_enlarged = face; 
	const float enlarge_scale = 1.5f;
	EnlargeRect(enlarge_scale, &face_enlarged);

	// 2 square the rect
	RectifyRect(&face_enlarged);
	face_enlarged = face_enlarged & Rect(0, 0, img_width, img_height);

	// 3 crop the face
    size_t total_size = face_enlarged.width * face_enlarged.height * 3 * sizeof(unsigned char);
    unsigned char* img_face = (unsigned char*)malloc(total_size);
    const unsigned char *start_ptr = rgbdata;
    for(size_t i = 0; i < face_enlarged.height; ++i) {
        const unsigned char* srcCursor = start_ptr + ((i + face_enlarged.y) * img_width + face_enlarged.x) * 3; 
        unsigned char* dstCursor = img_face + i * face_enlarged.width * 3;
        memcpy(dstCursor, srcCursor, sizeof(unsigned char) * 3 * face_enlarged.width);
    }

	// 4 do inference
	ncnn::Extractor ex = insightface_landmarker_net_->create_extractor();
	ncnn::Mat in = ncnn::Mat::from_pixels_resize(img_face,
		ncnn::Mat::PIXEL_RGB, face_enlarged.width, face_enlarged.height, 192, 192);
	ex.input("data", in);
	ncnn::Mat out;
	ex.extract("fc1", out);

	for (int i = 0; i < 106; ++i) {
		float x = (out[2 * i] + 1.0f) * face_enlarged.width / 2 + face_enlarged.x;
		float y = (out[2 * i + 1] + 1.0f) * face_enlarged.height / 2 + face_enlarged.y;
		keypoints->push_back(Point2f(x, y));
	}

    free(img_face);

	return 0;
}

}
