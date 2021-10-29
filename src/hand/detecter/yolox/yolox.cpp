#include "yolox.hpp"
#include "../../../common/yolov5focus.hpp"
#include <string>

#ifdef OV_VULKAN
#include "gpu.h"
#endif // OV_VULKAN

namespace ovhand {

static void generate_yolox_proposals(std::vector<ov::GridAndStride> grid_strides, const ncnn::Mat& feat_blob, float prob_threshold, std::vector<ov::ObjectInfo>& objects)
{
    const int num_grid = feat_blob.h;

    const int num_class = feat_blob.w - 5;

    const int num_anchors = grid_strides.size();

    const float* feat_ptr = feat_blob.channel(0);
    for (int anchor_idx = 0; anchor_idx < num_anchors; anchor_idx++)
    {
        const int grid0 = grid_strides[anchor_idx].grid0;
        const int grid1 = grid_strides[anchor_idx].grid1;
        const int stride = grid_strides[anchor_idx].stride;

        // yolox/models/yolo_head.py decode logic
        //  outputs[..., :2] = (outputs[..., :2] + grids) * strides
        //  outputs[..., 2:4] = torch.exp(outputs[..., 2:4]) * strides
        float x_center = (feat_ptr[0] + grid0) * stride;
        float y_center = (feat_ptr[1] + grid1) * stride;
        float w = exp(feat_ptr[2]) * stride;
        float h = exp(feat_ptr[3]) * stride;
        float x0 = x_center - w * 0.5f;
        float y0 = y_center - h * 0.5f;

        float box_objectness = feat_ptr[4];
        for (int class_idx = 0; class_idx < num_class; class_idx++)
        {
            float box_cls_score = feat_ptr[5 + class_idx];
            float box_prob = box_objectness * box_cls_score;
            if (box_prob > prob_threshold)
            {
                ov::ObjectInfo obj;
                obj.rect.x = x0;
                obj.rect.y = y0;
                obj.rect.width = w;
                obj.rect.height = h;
                obj.label = class_idx;
                obj.prob = box_prob;

                objects.push_back(obj);
            }

        } // class loop
        feat_ptr += feat_blob.w;

    } // point anchor loop
}

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
    std::vector<ov::ObjectInfo>& rois) {
	if (!initialized_) {
		return 10000;
	}
	if (rgbdata == 0){
		return 10001;
    }

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

    // so for 0-255 input image, rgb_mean should multiply 255 and norm should div by std.
    in_pad.substract_mean_normalize(mean_vals, norm_vals);

    ncnn::Extractor ex = net_->create_extractor();
    ex.set_num_threads(4);
    ex.input("input", in_pad);
    ncnn::Mat out;
    ex.extract("output", out);

    std::vector<ov::ObjectInfo> proposals;
    {

        std::vector<int> strides = {8, 16, 32}; // might have stride=64
        std::vector<ov::GridAndStride> grid_strides;
        generate_grids_and_stride(target_size, strides, grid_strides);
        generate_yolox_proposals(grid_strides, out, prob_threshold, proposals);
    }

    // sort all proposals by score from highest to lowest
    qsort_descent_inplace(proposals);

    // apply nms with nms_threshold
    std::vector<int> picked;
    nms_sorted_bboxes(proposals, picked, nms_threshold);

    int count = picked.size();

    rois.resize(count);

    for (int i = 0; i < count; i++)
    {
        ov::ObjectInfo roi = proposals[picked[i]];

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

        rois[i] = roi;
    } 

    return 0;
}

}
