#ifndef _COMMON_H_
#define _COMMON_H_

#include <vector>
#include <string>
#include <iostream>
#include "config.h"
#include "net.h"
#ifdef OV_OPENMP 
#include <omp.h>
#endif

namespace ov {

class EstimatorBase {
public:
    EstimatorBase();
    virtual ~EstimatorBase();
    virtual void set_num_threads(int n);
protected:
    int num_threads = 2;
};

class Estimator : public EstimatorBase {
public:
    Estimator();
    virtual ~Estimator();
    virtual int LoadModel(const char* root_path);
    virtual void set_num_threads(int n);
protected:
    ncnn::Net* net_;
    bool initialized_ = false;
};

// Wrapper for an individual cv::cvSize
struct Size {
    int width;
    int height;
    Size(int _width = 0, int _height = 0): width(_width), height(_height) {}
};
//
// Wrapper for an individual cv::cvSize2f
struct Size2f {
    float width;
    float height;
    Size2f(float _width = 0, float _height = 0): width(_width), height(_height) {}
};

// Wrapper for an individual cv::cvPoint
struct Point {
    int x;
    int y;
    Point(int _x = 0, int _y = 0): x(_x), y(_y) {}
    Point operator-(const Point &p2) {
        return Point(x - p2.x, y - p2.y);
    };
};

// Wrapper for an individual cv::Point2f
struct Point2f {
    float x;
    float y;
    Point2f(float _x = 0, float _y = 0): x(_x), y(_y) {}
    Point2f operator*(float f) const {
        return Point2f(x * f, y * f);
    };
    Point2f operator/(float f) const {
        return Point2f(x / f, y / f);
    };
    Point2f operator+(const Point2f &p2) const {
        return Point2f(x + p2.x, y + p2.y);
    };
    Point2f operator-(const Point2f &p2) const {
        return Point2f(x - p2.x, y - p2.y);
    };
};

// Wrapper for an individual cv::Rect
struct Rect {
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
};

struct Keypoint {
    Point2f p;
    float score;
    int id;
    Keypoint(){};
    Keypoint(const Point2f p_): p(p_){};
};

struct ObjectInfo {
	Rect rect;
	float score;
    int label;
    std::vector<Keypoint> pts;
};

struct Image {
    std::vector<float> data;
    int width;
    int height;
    int channels;
    float at(const Point& p) const {
        return data.at((p.x + p.y * width) * channels);
    };
    float atChannel(const Point& p, int channel) const {
        return data.at((p.x + p.y * width) * channels + channel);
    };
    Image(){};
    Image(const ncnn::Mat& mat): width(mat.w), height(mat.h), channels(mat.c) {
        int data_size = mat.total();
        float* ptr = (float*)malloc(data_size * sizeof(float));
        memcpy(ptr, mat.data, data_size * sizeof(float));
        data.clear();
        data.resize(data_size);
        data.assign(ptr, ptr + data_size);
        free(ptr);
        ptr=NULL;
    };
};

struct GridAndStride
{
    int grid0;
    int grid1;
    int stride;
};

template<typename T, std::size_t N>
constexpr std::size_t arraySize(const T (&)[N]) noexcept {
    return N;
}

static inline int cvRound(double x) {
    int y;
    if(x >= (int)x+0.5)
       y = (int)x++;
    else
       y = (int)x;
    return y;
};

int RatioAnchors(const Rect & anchor,
	const std::vector<float>& ratios, std::vector<Rect>* anchors, int threads_num);

int ScaleAnchors(const std::vector<Rect>& ratio_anchors,
	const std::vector<float>& scales, std::vector<Rect>* anchors, int threads_num);

int GenerateAnchors(const int & base_size,
	const std::vector<float>& ratios, const std::vector<float> scales,
	std::vector<Rect>* anchors,
    int threads_num);

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
        return a.score > b.score;
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
            ComputeIOU(inputs_tmp[good_idx].rect, inputs_tmp[tmp_i].rect, &iou, type);
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

// insightface/detection/scrfd/mmdet/core/anchor/anchor_generator.py gen_single_level_base_anchors()
ncnn::Mat generate_anchors(int base_size, const ncnn::Mat& ratios, const ncnn::Mat& scales);

int generate_grids_and_stride(const int target_size, std::vector<int>& strides, std::vector<GridAndStride>& grid_strides);

float sigmoid(float x);

void EnlargeRect(const float& scale, Rect* rect);
void RectifyRect(Rect* rect);

}

#endif // !_COMMON_H_
