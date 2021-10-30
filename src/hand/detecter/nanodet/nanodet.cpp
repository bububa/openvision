#include "nanodet.hpp"
#include <string>
#include <float.h>

#ifdef OV_VULKAN
#include "gpu.h"
#endif // OV_VULKAN

namespace ovhand {

static void generate_nanodet_proposals(const ncnn::Mat& cls_pred, const ncnn::Mat& dis_pred, int stride, const ncnn::Mat& in_pad, float prob_threshold, std::vector<ov::ObjectInfo>& objects)
{

    const int num_grid = cls_pred.h;

    int num_grid_x;
    int num_grid_y;
    if (in_pad.w > in_pad.h)
    {
        num_grid_x = in_pad.w / stride;
        num_grid_y = num_grid / num_grid_x;
    }
    else
    {
        num_grid_y = in_pad.h / stride;
        num_grid_x = num_grid / num_grid_y;
    }

    const int num_class = cls_pred.w;
    const int reg_max_1 = dis_pred.w / 4;
    //__android_log_print(ANDROID_LOG_WARN, "ncnn","cls_pred h %d, w %d",cls_pred.h,cls_pred.w);
    //__android_log_print(ANDROID_LOG_WARN, "ncnn","%d,%d,%d,%d",num_grid_x,num_grid_y,num_class,reg_max_1);
    for (int i = 0; i < num_grid_y; i++)
    {
        for (int j = 0; j < num_grid_x; j++)
        {
            const int idx = i * num_grid_x + j;

            const float* scores = cls_pred.row(idx);

            // find label with max score
            int label = -1;
            float score = -FLT_MAX;
            for (int k = 0; k < num_class; k++)
            {
                if (scores[k] > score)
                {
                    label = k;
                    score = scores[k];
                }
            }

            if (score >= prob_threshold)
            {
                ncnn::Mat bbox_pred(reg_max_1, 4, (void*)dis_pred.row(idx));
                {
                    ncnn::Layer* softmax = ncnn::create_layer("Softmax");

                    ncnn::ParamDict pd;
                    pd.set(0, 1); // axis
                    pd.set(1, 1);
                    softmax->load_param(pd);

                    ncnn::Option opt;
                    // opt.num_threads = 1;
                    opt.use_packing_layout = false;

                    softmax->create_pipeline(opt);

                    softmax->forward_inplace(bbox_pred, opt);

                    softmax->destroy_pipeline(opt);

                    delete softmax;
                }

                float pred_ltrb[4];
                for (int k = 0; k < 4; k++)
                {
                    float dis = 0.f;
                    const float* dis_after_sm = bbox_pred.row(k);
                    for (int l = 0; l < reg_max_1; l++)
                    {
                        dis += l * dis_after_sm[l];
                    }

                    pred_ltrb[k] = dis * stride;
                }

                float pb_cx = (j + 0.5f) * stride;
                float pb_cy = (i + 0.5f) * stride;

                float x0 = pb_cx - pred_ltrb[0];
                float y0 = pb_cy - pred_ltrb[1];
                float x1 = pb_cx + pred_ltrb[2];
                float y1 = pb_cy + pred_ltrb[3];

                ov::ObjectInfo obj;
                obj.rect.x = x0;
                obj.rect.y = y0;
                obj.rect.width = x1 - x0;
                obj.rect.height = y1 - y0;
                obj.label = label;
                obj.score= score;

                objects.push_back(obj);
            }
        }
    }
}

int Nanodet::Detect(const unsigned char* rgbdata,
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
    float wpad = 320-w;//(w + 31) / 32 * 32 - w;
    float hpad = 320-h;//(h + 31) / 32 * 32 - h;
    ncnn::Mat in_pad;
    ncnn::copy_make_border(in, in_pad, hpad / 2, hpad - hpad / 2, wpad / 2, wpad - wpad / 2, ncnn::BORDER_CONSTANT, 0.f);

    in_pad.substract_mean_normalize(mean_vals, norm_vals);

    ncnn::Extractor ex = net_->create_extractor();
    //__android_log_print(ANDROID_LOG_WARN, "ncnn","input w:%d,h:%d",in_pad.w,in_pad.h);
    ex.input("input.1", in_pad);

    std::vector<ov::ObjectInfo> proposals;
    // stride 8
    {
        ncnn::Mat cls_pred;
        ncnn::Mat dis_pred;
        ex.extract("cls_pred_stride_8", cls_pred);
        ex.extract("dis_pred_stride_8", dis_pred);

        std::vector<ov::ObjectInfo> objects8;
        generate_nanodet_proposals(cls_pred, dis_pred, 8, in_pad, prob_threshold, objects8);

        proposals.insert(proposals.end(), objects8.begin(), objects8.end());
    }

    // stride 16
    {
        ncnn::Mat cls_pred;
        ncnn::Mat dis_pred;
        ex.extract("cls_pred_stride_16", cls_pred);
        ex.extract("dis_pred_stride_16", dis_pred);

        std::vector<ov::ObjectInfo> objects16;
        generate_nanodet_proposals(cls_pred, dis_pred, 16, in_pad, prob_threshold, objects16);

        proposals.insert(proposals.end(), objects16.begin(), objects16.end());
    }

    // stride 32
    {
        ncnn::Mat cls_pred;
        ncnn::Mat dis_pred;
        ex.extract("cls_pred_stride_32", cls_pred);
        ex.extract("dis_pred_stride_32", dis_pred);

        std::vector<ov::ObjectInfo> objects32;
        generate_nanodet_proposals(cls_pred, dis_pred, 32, in_pad, prob_threshold, objects32);

        proposals.insert(proposals.end(), objects32.begin(), objects32.end());
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

        rois[i] = roi;
    }
    // sort objects by area
    struct
    {
        bool operator()(const ov::ObjectInfo& a, const ov::ObjectInfo& b) const
        {
            return a.rect.area() > b.rect.area();
        }
    } objects_area_greater;
    std::sort(rois.begin(), rois.end(), objects_area_greater);
    return 0;
}
}
