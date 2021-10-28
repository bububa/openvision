#include "yolox.hpp"
#include "../../../common/yolov5focus.hpp"
#include <string>

#ifdef OV_VULKAN
#include "gpu.h"
#endif // OV_VULKAN

namespace ovhand {

Yolox::Yolox() : 
    net_ (new ncnn::Net()),
	initialized_(false) {
#ifdef OV_VULKAN
    net_->opt.use_vulkan_compute = true;
#endif // OV_VULKAN
}

Yolox::~Yolox() {
    net_->clear();
}

int Yolox::LoadModel(const char * root_path) {
    register_yolov5focus(net_);
	std::string param_file = std::string(root_path) + "/param";
	std::string bin_file = std::string(root_path) + "/bin";
	if (net_->load_param(param_file.c_str()) == -1 ||
		net_->load_model(bin_file.c_str()) == -1) {
		return 10000;
	}
	initialized_ = true;
	return 0;
}

int Yolox::Detect(const unsigned char* rgbdata,
    int img_width, int img_height, 
    std::vector<HandROI>* rois) {
	if (!initialized_) {
		return 10000;
	}
	if (rgbdata == 0){
		return 10001;
    }

    const int target_size = 416;

    int w = img_width;
    int h = img_height;
    float scale = 1.f;
    if (w > h) {
        scale = (float)target_size / w;
        w = target_size;
        h = h * scale;
    } else {
        scale = (float)target_size / h;
        h = target_size;
        w = w * scale;
    }

    ncnn::Mat in = ncnn::Mat::from_pixels_resize(rgbdata, ncnn::Mat::PIXEL_RGB, img_width, img_height, w, h);
    // pad to target_size rectangle
    // yolov5/utils/datasets.py letterbox
    int wpad = target_size-w;//(w + 31) / 32 * 32 - w;
    int hpad = target_size-h;//(h + 31) / 32 * 32 - h;
    ncnn::Mat in_pad;
    ncnn::copy_make_border(in, in_pad, 0, hpad, 0, wpad, ncnn::BORDER_CONSTANT, 114.f);

    const float mean_vals[3] = {255.f * 0.485f, 255.f * 0.456, 255.f * 0.406f};
    const float norm_vals[3] = {1 / (255.f * 0.229f), 1 / (255.f * 0.224f), 1 / (255.f * 0.225f)};
    // so for 0-255 input image, rgb_mean should multiply 255 and norm should div by std.
    in_pad.substract_mean_normalize(mean_vals, norm_vals);

    ncnn::Extractor ex = net_->create_extractor();
    ex.set_num_threads(4);
    ex.input("input", in_pad);
    ncnn::Mat out;
    ex.extract("output", out);

    const float prob_threshold = 0.45f; 
    const float nms_threshold = 0.65f;
    std::vector<HandROI> proposals;
    {

        std::vector<int> strides = {8, 16, 32}; // might have stride=64
        std::vector<GridAndStride> grid_strides;
        generate_grids_and_stride(target_size, strides, grid_strides);
        generate_yolox_proposals(grid_strides, out, prob_threshold, proposals);
    }

    // sort all proposals by score from highest to lowest
    qsort_descent_inplace(proposals);

    // apply nms with nms_threshold
    std::vector<int> picked;
    nms_sorted_bboxes(proposals, picked, nms_threshold);

    int count = picked.size();

    for (int i = 0; i < count; i++)
    {
        HandROI roi = proposals[picked[i]];

        // adjust offset to original unpadded
        float x0 = (roi.rect.x) / scale;
        float y0 = (roi.rect.y) / scale;
        float x1 = (roi.rect.x + roi.rect.width) / scale;
        float y1 = (roi.rect.y + roi.rect.height) / scale;

        // clip
        x0 = std::max(std::min(x0, (float)(img_width - 1)), 0.f);
        y0 = std::max(std::min(y0, (float)(img_height - 1)), 0.f);
        x1 = std::max(std::min(x1, (float)(img_width - 1)), 0.f);
        y1 = std::max(std::min(y1, (float)(img_height - 1)), 0.f);

        roi.rect.x = x0;
        roi.rect.y = y0;
        roi.rect.width = x1 - x0;
        roi.rect.height = y1 - y0;

        rois->push_back(roi);
    } 

    return 0;
}

}
