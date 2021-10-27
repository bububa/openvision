#include "ultralight.hpp"
#include <string>

#ifdef OV_VULKAN
#include "gpu.h"
#endif // OV_VULKAN

namespace ov {
Ultralight::Ultralight() {
    roi_net_ = new ncnn::Net();
    pose_net_ = new ncnn::Net();
	initialized_ = false;
#ifdef OV_VULKAN
    roi_net_->opt.use_vulkan_compute = true;
    pose_net_->opt.use_vulkan_compute = true;
#endif // OV_VULKAN
}

Ultralight::~Ultralight() {
    roi_net_->clear();
    pose_net_->clear();
}

int Ultralight::LoadModel(const char * root_path) {
	std::string roi_param_file = std::string(root_path) + "/roi.param";
	std::string roi_bin_file = std::string(root_path) + "/roi.bin";
	if (roi_net_->load_param(roi_param_file.c_str()) == -1 ||
		roi_net_->load_model(roi_bin_file.c_str()) == -1) {
		return 10000;
	}

	std::string pose_param_file = std::string(root_path) + "/pose.param";
	std::string pose_bin_file = std::string(root_path) + "/pose.bin";
	if (pose_net_->load_param(pose_param_file.c_str()) == -1 ||
		pose_net_->load_model(pose_bin_file.c_str()) == -1) {
		return 10000;
	}

	initialized_ = true;
	return 0;
}

int Ultralight::ExtractROIs(const unsigned char* rgbdata,
    int img_width, int img_height,
    std::vector<PoseROI>* rois) {
	if (!initialized_) {
		return 10000;
	}
	if (rgbdata == 0){
		return 10001;
	}
	ncnn::Mat in = ncnn::Mat::from_pixels_resize(rgbdata,
		ncnn::Mat::PIXEL_RGB, img_width, img_height, 320, 320);
    //数据预处理
    const float mean_vals[3] = {0.f, 0.f, 0.f};
    const float norm_vals[3] = {1/255.f, 1/255.f, 1/255.f};
    in.substract_mean_normalize(mean_vals, norm_vals);

    ncnn::Extractor ex = roi_net_->create_extractor();
    ex.set_num_threads(4);
    ex.input("data", in);
    ncnn::Mat out;
    ex.extract("output", out);

    for (int i = 0; i < out.h; i++)
    {
        float x1, y1, x2, y2, score, label;
        float pw,ph,cx,cy;
        const float* values = out.row(i);
        
        x1 = values[2] * img_width;
        y1 = values[3] * img_height;
        x2 = values[4] * img_width;
        y2 = values[5] * img_height;

        pw = x2-x1;
        ph = y2-y1;
        cx = x1+0.5*pw;
        cy = y1+0.5*ph;

        x1 = cx - 0.7*pw;
        y1 = cy - 0.6*ph;
        x2 = cx + 0.7*pw;
        y2 = cy + 0.6*ph;

        score = values[1];
        label = values[0];

        //处理坐标越界问题
        if(x1<0) x1=0;
        if(y1<0) y1=0;
        if(x2<0) x2=0;
        if(y2<0) y2=0;

        if(x1>img_width) x1=img_width;
        if(y1>img_height) y1=img_height;
        if(x2>img_width) x2=img_width;
        if(y2>img_height) y2=img_height;
        //截取人体ROI
        //printf("x1:%f y1:%f x2:%f y2:%f\n",x1,y1,x2,y2);
        Rect rect = Rect(x1, y1, x2-x1, y2-y1);
        size_t total_size = rect.width * rect.height * 3 * sizeof(unsigned char);
        PoseROI roi;
        roi.rect = rect;
        roi.score = score;
        roi.data = (unsigned char*)malloc(total_size);
        const unsigned char *start_ptr = rgbdata;
        for(size_t i = 0; i < rect.height; ++i) {
            const unsigned char* srcCursor = start_ptr + ((i + rect.y) * img_width + rect.x) * 3; 
            unsigned char* dstCursor = roi.data + i * rect.width * 3;
            memcpy(dstCursor, srcCursor, sizeof(unsigned char) * 3 * rect.width);
        }
        rois->push_back(roi);
    }
    return 0;
}

int Ultralight::ExtractKeypoints(const PoseROI& roi, std::vector<PoseKeypoint>* keypoints) {
    keypoints->clear();
    int w = roi.rect.width;
    int h = roi.rect.height;
    ncnn::Mat in = ncnn::Mat::from_pixels_resize(roi.data, ncnn::Mat::PIXEL_RGB, w, h, 192, 256);
    //数据预处理
    const float mean_vals[3] = {0.485f * 255.f, 0.456f * 255.f, 0.406f * 255.f};
    const float norm_vals[3] = {1 / 0.229f / 255.f, 1 / 0.224f / 255.f, 1 / 0.225f / 255.f};
    in.substract_mean_normalize(mean_vals, norm_vals);

    ncnn::Extractor ex = pose_net_->create_extractor();
    ex.set_num_threads(4);
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

        PoseKeypoint keypoint;
        keypoint.p = Point2f(max_x * w / (float)out.w+roi.rect.x, max_y * h / (float)out.h+roi.rect.y);
        keypoint.prob = max_prob;
        keypoints->push_back(keypoint);
    }
    return 0;
}

}
