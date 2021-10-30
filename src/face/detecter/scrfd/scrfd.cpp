#include "scrfd.hpp"

#ifdef OV_VULKAN
#include "gpu.h"
#endif // OV_VULKAN

namespace ovface {
static void generate_scrfd_proposals(const ncnn::Mat& anchors, int feat_stride, const ncnn::Mat& score_blob, const ncnn::Mat& bbox_blob, const ncnn::Mat& kps_blob, float prob_threshold, std::vector<FaceInfo>& faceobjects)
{
    int w = score_blob.w;
    int h = score_blob.h;

    // generate face proposal from bbox deltas and shifted anchors
    const int num_anchors = anchors.h;

    for (int q = 0; q < num_anchors; q++)
    {
        const float* anchor = anchors.row(q);

        const ncnn::Mat score = score_blob.channel(q);
        const ncnn::Mat bbox = bbox_blob.channel_range(q * 4, 4);

        // shifted anchor
        float anchor_y = anchor[1];

        float anchor_w = anchor[2] - anchor[0];
        float anchor_h = anchor[3] - anchor[1];

        for (int i = 0; i < h; i++)
        {
            float anchor_x = anchor[0];

            for (int j = 0; j < w; j++)
            {
                int index = i * w + j;

                float prob = score[index];

                if (prob >= prob_threshold)
                {
                    // insightface/detection/scrfd/mmdet/models/dense_heads/scrfd_head.py _get_bboxes_single()
                    float dx = bbox.channel(0)[index] * feat_stride;
                    float dy = bbox.channel(1)[index] * feat_stride;
                    float dw = bbox.channel(2)[index] * feat_stride;
                    float dh = bbox.channel(3)[index] * feat_stride;

                    // insightface/detection/scrfd/mmdet/core/bbox/transforms.py distance2bbox()
                    float cx = anchor_x + anchor_w * 0.5f;
                    float cy = anchor_y + anchor_h * 0.5f;

                    float x0 = cx - dx;
                    float y0 = cy - dy;
                    float x1 = cx + dw;
                    float y1 = cy + dh;

                    FaceInfo obj;
                    obj.rect.x = x0;
                    obj.rect.y = y0;
                    obj.rect.width = x1 - x0 + 1;
                    obj.rect.height = y1 - y0 + 1;
                    obj.score = prob;

                    if (!kps_blob.empty())
                    {
                        const ncnn::Mat kps = kps_blob.channel_range(q * 10, 10);

                        obj.keypoints_[0] = cx + kps.channel(0)[index] * feat_stride;
                        obj.keypoints_[5] = cy + kps.channel(1)[index] * feat_stride;
                        obj.keypoints_[1] = cx + kps.channel(2)[index] * feat_stride;
                        obj.keypoints_[6] = cy + kps.channel(3)[index] * feat_stride;
                        obj.keypoints_[2] = cx + kps.channel(4)[index] * feat_stride;
                        obj.keypoints_[7] = cy + kps.channel(5)[index] * feat_stride;
                        obj.keypoints_[3] = cx + kps.channel(6)[index] * feat_stride;
                        obj.keypoints_[8] = cy + kps.channel(7)[index] * feat_stride;
                        obj.keypoints_[4] = cx + kps.channel(8)[index] * feat_stride;
                        obj.keypoints_[9] = cy + kps.channel(9)[index] * feat_stride;
                    }

                    faceobjects.push_back(obj);
                }

                anchor_x += feat_stride;
            }

            anchor_y += feat_stride;
        }
    }
}

int Scrfd::DetectFace(const unsigned char* rgbdata,
    int img_width, int img_height, 
    std::vector<FaceInfo>* faces) {
    faces->clear();
	if (!initialized_) {
		return 10000;
	}
	if (rgbdata == 0){
		return 10001;
    }

    // pad to multiple of 32
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
    float wpad = (float)(w + 31) / 32 * 32 - w;
    float hpad = (float)(h + 31) / 32 * 32 - h;
    ncnn::Mat in_pad;
    ncnn::copy_make_border(in, in_pad, hpad / 2, hpad - hpad / 2, wpad / 2, wpad - wpad / 2, ncnn::BORDER_CONSTANT, 0.f);

    const float mean_vals[3] = {127.5f, 127.5f, 127.5f};
    const float norm_vals[3] = {1/128.f, 1/128.f, 1/128.f};
    in_pad.substract_mean_normalize(mean_vals, norm_vals);

    ncnn::Extractor ex = net_->create_extractor();

    ex.input("input.1", in_pad);

    std::vector<FaceInfo> faceproposals;

    // stride 8
    {
        ncnn::Mat score_blob, bbox_blob, kps_blob;
        ex.extract("score_8", score_blob);
        ex.extract("bbox_8", bbox_blob);
        if (has_kps)
            ex.extract("kps_8", kps_blob);

        const int base_size = 16;
        const int feat_stride = 8;
        ncnn::Mat ratios(1);
        ratios[0] = 1.f;
        ncnn::Mat scales(2);
        scales[0] = 1.f;
        scales[1] = 2.f;
        ncnn::Mat anchors = ov::generate_anchors(base_size, ratios, scales);

        std::vector<FaceInfo> faceobjects32;
        generate_scrfd_proposals(anchors, feat_stride, score_blob, bbox_blob, kps_blob, prob_threshold, faceobjects32);

        faceproposals.insert(faceproposals.end(), faceobjects32.begin(), faceobjects32.end());
    }

    // stride 16
    {
        ncnn::Mat score_blob, bbox_blob, kps_blob;
        ex.extract("score_16", score_blob);
        ex.extract("bbox_16", bbox_blob);
        if (has_kps)
            ex.extract("kps_16", kps_blob);

        const int base_size = 64;
        const int feat_stride = 16;
        ncnn::Mat ratios(1);
        ratios[0] = 1.f;
        ncnn::Mat scales(2);
        scales[0] = 1.f;
        scales[1] = 2.f;
        ncnn::Mat anchors = ov::generate_anchors(base_size, ratios, scales);

        std::vector<FaceInfo> faceobjects16;
        generate_scrfd_proposals(anchors, feat_stride, score_blob, bbox_blob, kps_blob, prob_threshold, faceobjects16);

        faceproposals.insert(faceproposals.end(), faceobjects16.begin(), faceobjects16.end());
    }

    // stride 32
    {
        ncnn::Mat score_blob, bbox_blob, kps_blob;
        ex.extract("score_32", score_blob);
        ex.extract("bbox_32", bbox_blob);
        if (has_kps)
            ex.extract("kps_32", kps_blob);

        const int base_size = 256;
        const int feat_stride = 32;
        ncnn::Mat ratios(1);
        ratios[0] = 1.f;
        ncnn::Mat scales(2);
        scales[0] = 1.f;
        scales[1] = 2.f;
        ncnn::Mat anchors = ov::generate_anchors(base_size, ratios, scales);

        std::vector<FaceInfo> faceobjects8;
        generate_scrfd_proposals(anchors, feat_stride, score_blob, bbox_blob, kps_blob, prob_threshold, faceobjects8);

        faceproposals.insert(faceproposals.end(), faceobjects8.begin(), faceobjects8.end());
    }

    // sort all proposals by score from highest to lowest
    qsort_descent_inplace(faceproposals);

    // apply nms with nms_threshold
    std::vector<int> picked;
    nms_sorted_bboxes(faceproposals, picked, nms_threshold);

    int face_count = picked.size();

    for (int i = 0; i < face_count; i++)
    {
        FaceInfo obj = faceproposals[picked[i]];

        // adjust offset to original unpadded
        float x0 = (obj.rect.x - (wpad / 2)) / scale;
        float y0 = (obj.rect.y - (hpad / 2)) / scale;
        float x1 = (obj.rect.x + obj.rect.width - (wpad / 2)) / scale;
        float y1 = (obj.rect.y + obj.rect.height - (hpad / 2)) / scale;

        x0 = std::max(std::min(x0, (float)img_width - 1), 0.f);
        y0 = std::max(std::min(y0, (float)img_height - 1), 0.f);
        x1 = std::max(std::min(x1, (float)img_width - 1), 0.f);
        y1 = std::max(std::min(y1, (float)img_height - 1), 0.f);

        obj.rect.x = x0;
        obj.rect.y = y0;
        obj.rect.width = x1 - x0;
        obj.rect.height = y1 - y0;

        if (has_kps)
        {
            float x0 = (obj.keypoints_[0] - (wpad / 2)) / scale;
            float y0 = (obj.keypoints_[5] - (hpad / 2)) / scale;
            float x1 = (obj.keypoints_[1] - (wpad / 2)) / scale;
            float y1 = (obj.keypoints_[6] - (hpad / 2)) / scale;
            float x2 = (obj.keypoints_[2] - (wpad / 2)) / scale;
            float y2 = (obj.keypoints_[7] - (hpad / 2)) / scale;
            float x3 = (obj.keypoints_[3] - (wpad / 2)) / scale;
            float y3 = (obj.keypoints_[8] - (hpad / 2)) / scale;
            float x4 = (obj.keypoints_[4] - (wpad / 2)) / scale;
            float y4 = (obj.keypoints_[9] - (hpad / 2)) / scale;

            obj.keypoints_[0] = std::max(std::min(x0, (float)img_width - 1), 0.f);
            obj.keypoints_[5] = std::max(std::min(y0, (float)img_height - 1), 0.f);
            obj.keypoints_[1] = std::max(std::min(x1, (float)img_width - 1), 0.f);
            obj.keypoints_[6] = std::max(std::min(y1, (float)img_height - 1), 0.f);
            obj.keypoints_[2] = std::max(std::min(x2, (float)img_width - 1), 0.f);
            obj.keypoints_[7] = std::max(std::min(y2, (float)img_height - 1), 0.f);
            obj.keypoints_[3] = std::max(std::min(x3, (float)img_width - 1), 0.f);
            obj.keypoints_[8] = std::max(std::min(y3, (float)img_height - 1), 0.f);
            obj.keypoints_[4] = std::max(std::min(x4, (float)img_width - 1), 0.f);
            obj.keypoints_[9] = std::max(std::min(y4, (float)img_height - 1), 0.f);
        }
        faces->push_back(obj);
    }
    return 0;
}
}
