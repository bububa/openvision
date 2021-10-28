#ifndef _HAND_COMMON_H_
#define _HAND_COMMON_H_

#include "../../common/common.h"
#include "net.h"
#include <vector>

namespace ovhand {
struct HandROI {
    ov::Rect rect;
    int label;
    float prob;
};
struct GridAndStride
{
    int grid0;
    int grid1;
    int stride;
};

inline float intersection_area(const HandROI& a, const HandROI& b);

void qsort_descent_inplace(std::vector<HandROI>& objects, int left, int right);

void qsort_descent_inplace(std::vector<HandROI>& objects);

void nms_sorted_bboxes(const std::vector<HandROI>& objects, std::vector<int>& picked, float nms_threshold);

int generate_grids_and_stride(const int target_size, std::vector<int>& strides, std::vector<GridAndStride>& grid_strides);

void generate_yolox_proposals(std::vector<GridAndStride> grid_strides, const ncnn::Mat& feat_blob, float prob_threshold, std::vector<HandROI>& objects);

void generate_nanodet_proposals(const ncnn::Mat& cls_pred, const ncnn::Mat& dis_pred, int stride, const ncnn::Mat& in_pad, float prob_threshold, std::vector<HandROI>& objects);
}
#endif // !_HAND_COMMON_H_
