#include "nanodet.hpp"
#include <string>

#ifdef OV_VULKAN
#include "gpu.h"
#endif // OV_VULKAN

namespace ovhand {
Nanodet::Nanodet() : 
    net_ (new ncnn::Net()),
	initialized_(false) {
#ifdef OV_VULKAN
    net_->opt.use_vulkan_compute = true;
#endif // OV_VULKAN
}

Nanodet::~Nanodet() {
    net_->clear();
}

int Nanodet::LoadModel(const char * root_path) {
	std::string param_file = std::string(root_path) + "/param";
	std::string bin_file = std::string(root_path) + "/bin";
	if (net_->load_param(param_file.c_str()) == -1 ||
		net_->load_model(bin_file.c_str()) == -1) {
		return 10000;
	}
	initialized_ = true;
	return 0;
}

int Nanodet::Detect(const unsigned char* rgbdata,
    int img_width, int img_height, 
    std::vector<HandROI>* rois) {
	if (!initialized_) {
		return 10000;
	}
	if (rgbdata == 0){
		return 10001;
    }

    const int target_size = 320;

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
    float wpad = 320-w;//(w + 31) / 32 * 32 - w;
    float hpad = 320-h;//(h + 31) / 32 * 32 - h;
    ncnn::Mat in_pad;
    ncnn::copy_make_border(in, in_pad, hpad / 2, hpad - hpad / 2, wpad / 2, wpad - wpad / 2, ncnn::BORDER_CONSTANT, 0.f);

    const float mean_vals[3] = {103.53f, 116.28f, 123.675f};
    const float norm_vals[3] = {1.f / 57.375f, 1.f / 57.12f, 1.f / 58.395f};
    in_pad.substract_mean_normalize(mean_vals, norm_vals);

    ncnn::Extractor ex = net_->create_extractor();
    //__android_log_print(ANDROID_LOG_WARN, "ncnn","input w:%d,h:%d",in_pad.w,in_pad.h);
    ex.input("input.1", in_pad);

    const float prob_threshold = 0.4f; 
    const float nms_threshold = 0.5f;
    std::vector<HandROI> proposals;
    // stride 8
    {
        ncnn::Mat cls_pred;
        ncnn::Mat dis_pred;
        ex.extract("cls_pred_stride_8", cls_pred);
        ex.extract("dis_pred_stride_8", dis_pred);

        std::vector<HandROI> objects8;
        generate_nanodet_proposals(cls_pred, dis_pred, 8, in_pad, prob_threshold, objects8);

        proposals.insert(proposals.end(), objects8.begin(), objects8.end());
    }

    // stride 16
    {
        ncnn::Mat cls_pred;
        ncnn::Mat dis_pred;
        ex.extract("cls_pred_stride_16", cls_pred);
        ex.extract("dis_pred_stride_16", dis_pred);

        std::vector<HandROI> objects16;
        generate_nanodet_proposals(cls_pred, dis_pred, 16, in_pad, prob_threshold, objects16);

        proposals.insert(proposals.end(), objects16.begin(), objects16.end());
    }

    // stride 32
    {
        ncnn::Mat cls_pred;
        ncnn::Mat dis_pred;
        ex.extract("cls_pred_stride_32", cls_pred);
        ex.extract("dis_pred_stride_32", dis_pred);

        std::vector<HandROI> objects32;
        generate_nanodet_proposals(cls_pred, dis_pred, 32, in_pad, prob_threshold, objects32);

        proposals.insert(proposals.end(), objects32.begin(), objects32.end());
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
        float x0 = (roi.rect.x - (wpad / 2)) / scale;
        float y0 = (roi.rect.y - (hpad / 2)) / scale;
        float x1 = (roi.rect.x + roi.rect.width - (wpad / 2)) / scale;
        float y1 = (roi.rect.y + roi.rect.height - (hpad / 2)) / scale;

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
