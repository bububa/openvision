#include "insightface.hpp"
#include <string>

#ifdef OV_VULKAN
#include "gpu.h"
#endif // OV_VULKAN

namespace ovface {

int InsightfaceLandmarker::ExtractKeypoints(const unsigned char* rgbdata, 
    int img_width, int img_height,
	const ov::Rect & face, std::vector<ov::Point2f>* keypoints) {
	keypoints->clear();
	if (!initialized_) {
		return 10000;
	}

	if (rgbdata == 0){
		return 10001;
	}

	// 1 enlarge the face rect
    ov::Rect face_enlarged = face; 
	const float enlarge_scale = 1.5f;
	EnlargeRect(enlarge_scale, &face_enlarged);

	// 2 square the rect
	RectifyRect(&face_enlarged);
	face_enlarged = face_enlarged & Rect(0, 0, img_width, img_height);

	// 3 crop the face
    size_t total_size = face_enlarged.width * face_enlarged.height * 3 * sizeof(unsigned char);
    unsigned char* img_face = (unsigned char*)malloc(total_size);
    const unsigned char *start_ptr = rgbdata;
#if defined(_OPENMP)
#pragma omp parallel for num_threads(num_threads)
#endif
    for(size_t i = 0; i < face_enlarged.height; ++i) {
        const unsigned char* srcCursor = start_ptr + ((i + face_enlarged.y) * img_width + face_enlarged.x) * 3; 
        unsigned char* dstCursor = img_face + i * face_enlarged.width * 3;
        memcpy(dstCursor, srcCursor, sizeof(unsigned char) * 3 * face_enlarged.width);
    }
	ncnn::Mat in = ncnn::Mat::from_pixels_resize(img_face,
		ncnn::Mat::PIXEL_RGB, face_enlarged.width, face_enlarged.height, 192, 192);

	// 4 do inference
	ncnn::Extractor ex = net_->create_extractor();
    ex.set_light_mode(light_mode_);
    ex.set_num_threads(num_threads);
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
