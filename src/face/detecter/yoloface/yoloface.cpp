#include "yoloface.hpp"
#include "../../../common/yolov5focus.hpp"
#include <string>
#include <float.h>

#ifdef OV_VULKAN
#include "gpu.h"
#endif // OV_VULKAN

namespace ovface {

static void generate_proposals(const ncnn::Mat& anchors, int stride, const ncnn::Mat& in_pad, const ncnn::Mat& feat_blob, float prob_threshold, std::vector<ov::ObjectInfo>& objects)
{
    const int num_grid = feat_blob.h;

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

    const int num_class = feat_blob.w - 5-10;

    const int num_anchors = anchors.w / 2;

    for (int q = 0; q < num_anchors; q++)
    {
        const float anchor_w = anchors[q * 2];
        const float anchor_h = anchors[q * 2 + 1];

        const ncnn::Mat feat = feat_blob.channel(q);

        for (int i = 0; i < num_grid_y; i++)
        {
            for (int j = 0; j < num_grid_x; j++)
            {
                const float* featptr = feat.row(i * num_grid_x + j);

                // find class index with max class score
                int class_index = 0;
                float class_score = -FLT_MAX;
                for (int k = 0; k < num_class; k++)
                {
                    float score = featptr[5 +10+ k];
                    if (score > class_score)
                    {
                        class_index = k;
                        class_score = score;
                    }
                }

                float box_score = featptr[4];

				float confidence = ov::sigmoid(box_score); //* sigmoid(class_score);

                if (confidence >= prob_threshold)
                {
                    // yolov5/models/yolo.py Detect forward
                    // y = x[i].sigmoid()
                    // y[..., 0:2] = (y[..., 0:2] * 2. - 0.5 + self.grid[i].to(x[i].device)) * self.stride[i]  # xy
                    // y[..., 2:4] = (y[..., 2:4] * 2) ** 2 * self.anchor_grid[i]  # wh

                    float dx = ov::sigmoid(featptr[0]);
                    float dy = ov::sigmoid(featptr[1]);
                    float dw = ov::sigmoid(featptr[2]);
                    float dh = ov::sigmoid(featptr[3]);

                    float pb_cx = (dx * 2.f - 0.5f + j) * stride;
                    float pb_cy = (dy * 2.f - 0.5f + i) * stride;

                    float pb_w = pow(dw * 2.f, 2) * anchor_w;
                    float pb_h = pow(dh * 2.f, 2) * anchor_h;

                    float x0 = pb_cx - pb_w * 0.5f;
                    float y0 = pb_cy - pb_h * 0.5f;
                    float x1 = pb_cx + pb_w * 0.5f;
                    float y1 = pb_cy + pb_h * 0.5f;

					


                    ov::ObjectInfo obj;
                    obj.rect.x = x0;
                    obj.rect.y = y0;
                    obj.rect.width = x1 - x0;
                    obj.rect.height = y1 - y0;
                    obj.label = class_index;
                    obj.score = confidence;

					for (int l = 0; l < 5; l++)
					{
						float x = featptr[2 * l + 5] * anchor_w + j * stride;
						float y = featptr[2 * l + 1 + 5] * anchor_h + i * stride;
						obj.pts.push_back(ov::Point2f(x, y));
					}
                    objects.push_back(obj);
                }
            }
        }
    }
}


int YoloFace::LoadModel(const char * root_path) {
    register_yolov5focus(net_);
    return Estimator::LoadModel(root_path);
}

int YoloFace::DetectFace(const unsigned char* rgbdata,
    int img_width, int img_height, 
    std::vector<FaceInfo>* faces) {
    faces->clear();
	if (!initialized_) {
		return 10000;
	}
	if (rgbdata == 0){
		return 10001;
    }

    // letterbox pad to multiple of 32
    int w = img_width;
    int h = img_height;
    float scale = 1.f;
    if (w > h)
    {
        scale = (float)target_size / w;
        w = target_size;
        h = h * scale;
    }
    else
    {
        scale = (float)target_size / h;
        h = target_size;
        w = w * scale;
    }

    ncnn::Mat in = ncnn::Mat::from_pixels_resize(rgbdata, ncnn::Mat::PIXEL_RGB, img_width, img_height, w, h);

    // pad to target_size rectangle
    // yolov5/utils/datasets.py letterbox
	int wpad = (w + 31) / 32 * 32 - w;
	int hpad = (h + 31) / 32 * 32 - h;
    ncnn::Mat in_pad;
    ncnn::copy_make_border(in, in_pad, hpad / 2, hpad - hpad / 2, wpad / 2, wpad - wpad / 2, ncnn::BORDER_CONSTANT, 114.f);


    in_pad.substract_mean_normalize(0, norm_vals);

    ncnn::Extractor ex = net_->create_extractor();

    ex.input("data", in_pad);

    std::vector<ov::ObjectInfo> proposals;

    // anchor setting from yolov5/models/yolov5s.yaml

    // stride 8
    {
        ncnn::Mat out;
        ex.extract("981", out);

        ncnn::Mat anchors(6);
        anchors[0] = 4.f;
        anchors[1] = 5.f;
        anchors[2] = 8.f;
        anchors[3] = 10.f;
        anchors[4] = 13.f;
        anchors[5] = 16.f;

        std::vector<ov::ObjectInfo> objects8;
        generate_proposals(anchors, 8, in_pad, out, prob_threshold, objects8);

        proposals.insert(proposals.end(), objects8.begin(), objects8.end());
    }

    // stride 16
    {
        ncnn::Mat out;
        ex.extract("983", out);

        ncnn::Mat anchors(6);
        anchors[0] = 23.f;
        anchors[1] = 29.f;
        anchors[2] = 43.f;
        anchors[3] = 55.f;
        anchors[4] = 73.f;
        anchors[5] = 105.f;

        std::vector<ov::ObjectInfo> objects16;
        generate_proposals(anchors, 16, in_pad, out, prob_threshold, objects16);

        proposals.insert(proposals.end(), objects16.begin(), objects16.end());
    }

    // stride 32
    {
        ncnn::Mat out;
        ex.extract("985", out);

        ncnn::Mat anchors(6);
        anchors[0] = 146.f;
        anchors[1] = 217.f;
        anchors[2] = 231.f;
        anchors[3] = 300.f;
        anchors[4] = 335.f;
        anchors[5] = 433.f;

        std::vector<ov::ObjectInfo> objects32;
        generate_proposals(anchors, 32, in_pad, out, prob_threshold, objects32);

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
        ov::ObjectInfo obj = proposals[picked[i]];

        // adjust offset to original unpadded
        float x0 = (obj.rect.x - (float(wpad) / 2)) / scale;
        float y0 = (obj.rect.y - (float(hpad) / 2)) / scale;
        float x1 = (obj.rect.x + obj.rect.width - (float(wpad) / 2)) / scale;
        float y1 = (obj.rect.y + obj.rect.height - (float(hpad) / 2)) / scale;

		for (int j = 0; j < obj.pts.size(); j++)
		{
			float ptx = (obj.pts[j].x - (float(wpad) / 2)) / scale;
			float pty = (obj.pts[j].y - (float(hpad) / 2)) / scale;
			obj.pts[j] = ov::Point2f(ptx, pty);
		}

        // clip
        x0 = std::max(std::min(x0, (float)(img_width - 1)), 0.f);
        y0 = std::max(std::min(y0, (float)(img_height - 1)), 0.f);
        x1 = std::max(std::min(x1, (float)(img_width - 1)), 0.f);
        y1 = std::max(std::min(y1, (float)(img_height - 1)), 0.f);

        obj.rect.x = x0;
        obj.rect.y = y0;
        obj.rect.width = x1 - x0;
        obj.rect.height = y1 - y0;

        FaceInfo info;
        info.rect = obj.rect;
        for (int k = 0; k < 5; ++k) {
            info.keypoints_[k] = obj.pts[k].x;
            info.keypoints_[k + 5] = obj.pts[k].y;
        }

        faces->push_back(info);
    }


    return 0;
}

}
