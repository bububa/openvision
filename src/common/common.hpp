#ifndef _COMMON_H_
#define _COMMON_H_

#include <vector>
#include <string>
#include "config.h"
#include "net.h"
#ifdef OV_OPENMP 
#include <omp.h>
#endif

namespace ov {
const int threads_num = 2;

class Estimator {
public:
    virtual ~Estimator(){};
    virtual int LoadModel(const char* root_path) = 0;
};

// Wrapper for an individual cv::cvSize
typedef struct Size {
    int width;
    int height;
    Size(int _width = 0, int _height = 0): width(_width), height(_height) {}
} Size;

// Wrapper for an individual cv::cvPoint
typedef struct Point {
    int x;
    int y;
    Point(int _x = 0, int _y = 0): x(_x), y(_y) {}
    Point operator-(const Point &p2) {
        return Point(x - p2.x, y - p2.y);
    };
} Point;

// Wrapper for an individual cv::Point2f
typedef struct Point2f {
    float x;
    float y;
    Point2f(float _x = 0, float _y = 0): x(_x), y(_y) {}
} Point2f;

// Wrapper for an individual cv::Rect
typedef struct Rect {
    int x;
    int y;
    int width;
    int height;
    Rect(int _x = 0, int _y = 0, int _width = 0, int _height = 0): x(_x), y(_y), width(_width), height(_height) {}
    Point br() const {
        return Point(x + width, y + height);
    };
    int area() const {
        return  width * height;
    };
    Rect operator&(const Rect &r2) const {
        int inter_x = x;
        int inter_y = y;
        int inter_width = width;
        int inter_height = height;
        if (x < r2.x) {
            inter_x = r2.x;
        }
        if (y < r2.y) {
            inter_y = r2.y;
        }
        if (x + width > r2.x + r2.width) {
            inter_width = r2.x + r2.width - inter_x;
        }
        if (y + height > r2.y + r2.height) {
            inter_height = r2.y + r2.height - inter_y;
        }
        return Rect(inter_x, inter_y, inter_width, inter_height);
    };
} Rect;

struct ImageInfo {
    std::string label_;
    float score_;
};

struct Keypoint {
    ov::Point2f p;
    float prob;
};

struct ObjectInfo {
	Rect rect;
	float prob;
    int label;
    std::vector<Point2f> pts;
};

struct GridAndStride
{
    int grid0;
    int grid1;
    int stride;
};

int RatioAnchors(const Rect & anchor,
	const std::vector<float>& ratios, std::vector<Rect>* anchors);

int ScaleAnchors(const std::vector<Rect>& ratio_anchors,
	const std::vector<float>& scales, std::vector<Rect>* anchors);

int GenerateAnchors(const int & base_size,
	const std::vector<float>& ratios, const std::vector<float> scales,
	std::vector<Rect>* anchors);

float InterRectArea(const Rect & a,
	const Rect & b);

int ComputeIOU(const Rect & rect1,
	const Rect & rect2, float * iou,
	const std::string& type = "UNION");

template <typename T>
int const NMS(const std::vector<T>& inputs, std::vector<T>* result,
	const float& threshold, const std::string& type = "UNION") {
	result->clear();
    if (inputs.size() == 0)
        return -1;
    
    std::vector<T> inputs_tmp;
    inputs_tmp.assign(inputs.begin(), inputs.end());
    std::sort(inputs_tmp.begin(), inputs_tmp.end(),
    [](const T& a, const T& b) {
        return a.score_ > b.score_;
    });

    std::vector<int> indexes(inputs_tmp.size());

    for (int i = 0; i < indexes.size(); i++) {
        indexes[i] = i;
    }

    while (indexes.size() > 0) {
        int good_idx = indexes[0];
        result->push_back(inputs_tmp[good_idx]);
        std::vector<int> tmp_indexes = indexes;
        indexes.clear();
        for (int i = 1; i < tmp_indexes.size(); i++) {
            int tmp_i = tmp_indexes[i];
            float iou = 0.0f;
            ComputeIOU(inputs_tmp[good_idx].location_, inputs_tmp[tmp_i].location_, &iou, type);
            if (iou <= threshold) {
                indexes.push_back(tmp_i);
            }
        }
    }
    return 0;
}

void qsort_descent_inplace(std::vector<ObjectInfo>& objects, int left, int right);

void qsort_descent_inplace(std::vector<ObjectInfo>& objects); 

void nms_sorted_bboxes(const std::vector<ObjectInfo>& objects, std::vector<int>& picked, float nms_threshold);

int generate_grids_and_stride(const int target_size, std::vector<int>& strides, std::vector<GridAndStride>& grid_strides);

float sigmoid(float x);

void EnlargeRect(const float& scale, Rect* rect);
void RectifyRect(Rect* rect);

}

#endif // !_COMMON_H_
