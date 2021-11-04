#include "scrfd.hpp"

#ifdef OV_VULKAN
#include "gpu.h"
#endif // OV_VULKAN

namespace ovface {

int ScrfdLandmarker::ExtractKeypoints(const unsigned char* rgbdata, 
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
    ov::Rect box = face; 
	const float enlarge_scale = 1.5f;
	EnlargeRect(enlarge_scale, &box);

	// 2 square the rect
	RectifyRect(&box);

    box = box & Rect(0, 0, img_width, img_height);

    size_t total_size = box.width * box.height * 3 * sizeof(unsigned char);
    unsigned char* img_face = (unsigned char*)malloc(total_size);
    const unsigned char *start_ptr = rgbdata;
#if defined(_OPENMP)
#pragma omp parallel for num_threads(num_threads)
#endif
    for(size_t i = 0; i < box.height; ++i) {
        const unsigned char* srcCursor = start_ptr + ((i + box.y) * img_width + box.x) * 3; 
        unsigned char* dstCursor = img_face + i * box.width * 3;
        memcpy(dstCursor, srcCursor, sizeof(unsigned char) * 3 * box.width);
    }
    
    ncnn::Extractor ex = net_->create_extractor();
    ncnn::Mat ncnn_in = ncnn::Mat::from_pixels_resize(img_face, ncnn::Mat::PIXEL_RGB, box.width, box.height, 192, 192);
    ncnn_in.substract_mean_normalize(means, norms);
    ex.input("input.1",ncnn_in);
    ncnn::Mat ncnn_out;
    ex.extract("482",ncnn_out);
    float *scoredata = (float*)ncnn_out.data;
    for(int i = 0; i < 468; i++)
    {
        ov::Point2f pt;
        pt.x = scoredata[i*3]*box.width/192 + box.x;
        pt.y = scoredata[i*3+1]*box.height/192 + box.y;
        keypoints->push_back(pt);
    }

    free(img_face);
    return 0;
}
}
