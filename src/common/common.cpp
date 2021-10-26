#include "common.h"
#include <algorithm>
#include <iostream>
#include <math.h>

#ifdef OV_VULKAN
#include "gpu.h"
#endif // OV_VULKAN

int get_gpu_count() {
#ifdef OV_VULKAN
    return ncnn::get_gpu_count();
#endif // OV_VULKAN
    return 0;
}

int create_gpu_instance() {
#ifdef OV_VULKAN
    return ncnn::create_gpu_instance();
#endif // OV_VULKAN
    return 0;
}

void destroy_gpu_instance() {
#ifdef OV_VULKAN
    ncnn::destroy_gpu_instance();
#endif // OV_VULKAN
}

void FreePoint2fVector(Point2fVector* p) {
    if (p->points != NULL) {
        free(p->points);
        p->points = NULL;
    }
}

void FreeFloatVector(FloatVector *p) {
    if (p->values != NULL) {
        free(p->values);
        p->values = NULL;
    }
}

void FreeBytes(Bytes *p) {
    if (p->values != NULL) {
        free(p->values);
        p->values = NULL;
    }
}

namespace ov {

int RatioAnchors(const Rect & anchor,
	const std::vector<float>& ratios, 
	std::vector<Rect>* anchors) {
	anchors->clear();
	Point center = Point(anchor.x + (anchor.width - 1) * 0.5f,
		anchor.y + (anchor.height - 1) * 0.5f);
	float anchor_size = anchor.width * anchor.height;
#ifdef OV_OPENMP 
#pragma omp parallel for num_threads(threads_num)
#endif
	for (int i = 0; i < static_cast<int>(ratios.size()); ++i) {
		float ratio = ratios.at(i);
		float anchor_size_ratio = anchor_size / ratio;
		float curr_anchor_width = sqrt(anchor_size_ratio);
		float curr_anchor_height = curr_anchor_width * ratio;
		float curr_x = center.x - (curr_anchor_width - 1)* 0.5f;
		float curr_y = center.y - (curr_anchor_height - 1)* 0.5f;

		Rect curr_anchor = Rect(curr_x, curr_y,
			curr_anchor_width - 1, curr_anchor_height - 1);
		anchors->push_back(curr_anchor);
	}
	return 0;
}

int ScaleAnchors(const std::vector<Rect>& ratio_anchors,
	const std::vector<float>& scales, std::vector<Rect>* anchors) {
	anchors->clear();
#if defined(_OPENMP)
#pragma omp parallel for num_threads(threads_num)
#endif
	for (int i = 0; i < static_cast<int>(ratio_anchors.size()); ++i) {
		Rect anchor = ratio_anchors.at(i);
		Point2f center = Point2f(anchor.x + anchor.width * 0.5f,
			anchor.y + anchor.height * 0.5f);
		for (int j = 0; j < static_cast<int>(scales.size()); ++j) {
			float scale = scales.at(j);
			float curr_width = scale * (anchor.width + 1);
			float curr_height = scale * (anchor.height + 1);
			float curr_x = center.x - curr_width * 0.5f;
			float curr_y = center.y - curr_height * 0.5f;
			Rect curr_anchor = Rect(curr_x, curr_y,
				curr_width, curr_height);
			anchors->push_back(curr_anchor);
		}
	}

	return 0;
}

int GenerateAnchors(const int & base_size,
	const std::vector<float>& ratios, 
	const std::vector<float> scales,
	std::vector<Rect>* anchors) {
	anchors->clear();
	Rect anchor = Rect(0, 0, base_size, base_size);
	std::vector<Rect> ratio_anchors;
	RatioAnchors(anchor, ratios, &ratio_anchors);
	ScaleAnchors(ratio_anchors, scales, anchors);
	
	return 0;
}

float InterRectArea(const Rect & a, const Rect & b) {
	Point left_top = Point(std::max(a.x, b.x), std::max(a.y, b.y));
	Point right_bottom = Point(std::min(a.br().x, b.br().x), std::min(a.br().y, b.br().y));
	Point diff = right_bottom - left_top;
	return (std::max(diff.x + 1, 0) * std::max(diff.y + 1, 0));
}

int ComputeIOU(const Rect & rect1,
	const Rect & rect2, float * iou,
	const std::string& type) {

	float inter_area = InterRectArea(rect1, rect2);
	if (type == "UNION") {
		*iou = inter_area / (rect1.area() + rect2.area() - inter_area);
	}
	else {
		*iou = inter_area / std::min(rect1.area(), rect2.area());
	}

	return 0;
}


void EnlargeRect(const float& scale, Rect* rect) {
	float offset_x = (scale - 1.f) / 2.f * rect->width;
    float offset_y = (scale - 1.f) / 2.f * rect->height;
    rect->x -= offset_x;
    rect->y -= offset_y;
    rect->width = scale * rect->width;
    rect->height = scale * rect->height;
}

void RectifyRect(Rect* rect) {
	int max_side = std::max(rect->width, rect->height);
	int offset_x = (max_side - rect->width) / 2;
	int offset_y = (max_side - rect->height) / 2;

	rect->x -= offset_x;
	rect->y -= offset_y;
	rect->width = max_side;
	rect->height = max_side;    
}

}
