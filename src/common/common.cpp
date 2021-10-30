#include "common.h"
#include <algorithm>
#include <math.h>
#include <float.h>
#include "cpu.h"

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

int get_big_cpu_count() {
    return ncnn::get_big_cpu_count();
}

void set_omp_num_threads(int n) {
#ifdef OV_OPENMP
    ncnn::set_omp_num_threads(n);
#endif
}

int load_model(IEstimator d, const char *root_path) {
    return static_cast<ov::Estimator*>(d)->LoadModel(root_path);
}

void destroy_estimator(IEstimator d) {
    delete static_cast<ov::Estimator*>(d);
}

void set_num_threads(IEstimator d, int n) {
    static_cast<ov::Estimator*>(d)->set_num_threads(n);
}

void FreePoint2fVector(Point2fVector* p) {
    if (p->points != NULL) {
        free(p->points);
        p->points = NULL;
    }
}

void Point2fVectorSetValue(Point2fVector *p, int i, const Point2f* val) {
    if (p->points == NULL || i >= p->length) {
        return;
    }
    p->points[i] = *val;
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

void FreeKeypointVector(KeypointVector *p) {
    if (p->points != NULL) {
        free(p->points);
        p->points = NULL;
    }
}

void KeypointVectorSetValue(KeypointVector *p, int i, const Keypoint* val) {
    if (p->points == NULL || i >= p->length) {
        return;
    }
    p->points[i] = *val;
}

void FreeObjectInfo(ObjectInfo *p) {
    if (p->pts != NULL) {
        FreeKeypointVector(p->pts);
        free(p->pts);
        p->pts = NULL;
    }
}

void FreeObjectInfoVector(ObjectInfoVector *p) {
    if (p->items!=NULL) {
        for (int i=0; i < p->length; i ++) {
            FreeObjectInfo(&p->items[i]);
        }
        free(p->items);
        p->items= NULL;
    }
}
namespace ov {

Estimator::Estimator() {
    net_ = new ncnn::Net();
	initialized_ = false;
#ifdef OV_VULKAN
    net_->opt.use_vulkan_compute = true;
#endif // OV_VULKAN
}

Estimator::~Estimator() {
    if (net_) {
        net_->clear();
    }
}

int Estimator::LoadModel(const char * root_path) {
	std::string param_file = std::string(root_path) + "/param";
	std::string bin_file = std::string(root_path) + "/bin";
	if (net_->load_param(param_file.c_str()) == -1 ||
		net_->load_model(bin_file.c_str()) == -1) {
		return 10000;
	}

	initialized_ = true;

	return 0;
}

void Estimator::set_num_threads(int n) {
    num_threads = n;
    if (net_) {
        net_->opt.num_threads = n;
    }
}


int RatioAnchors(const Rect & anchor,
	const std::vector<float>& ratios, 
	std::vector<Rect>* anchors, int threads_num) {
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
	const std::vector<float>& scales, std::vector<Rect>* anchors, int threads_num) {
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
	std::vector<Rect>* anchors,
    int threads_num) {
	anchors->clear();
	Rect anchor = Rect(0, 0, base_size, base_size);
	std::vector<Rect> ratio_anchors;
	RatioAnchors(anchor, ratios, &ratio_anchors, threads_num);
	ScaleAnchors(ratio_anchors, scales, anchors, threads_num);
	
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

void qsort_descent_inplace(std::vector<ObjectInfo>& objects, int left, int right)
{
    int i = left;
    int j = right;
    float p = objects[(left + right) / 2].score;

    while (i <= j)
    {
        while (objects[i].score > p)
            i++;

        while (objects[j].score < p)
            j--;

        if (i <= j)
        {
            // swap
            std::swap(objects[i], objects[j]);

            i++;
            j--;
        }
    }

    #pragma omp parallel sections
    {
        #pragma omp section
        {
            if (left < j) qsort_descent_inplace(objects, left, j);
        }
        #pragma omp section
        {
            if (i < right) qsort_descent_inplace(objects, i, right);
        }
    }
}

void qsort_descent_inplace(std::vector<ObjectInfo>& objects) 
{
    if (objects.empty())
        return;

    qsort_descent_inplace(objects, 0, objects.size() - 1);
}

void nms_sorted_bboxes(const std::vector<ObjectInfo>& objects, std::vector<int>& picked, float nms_threshold)
{
    picked.clear();

    const int n = objects.size();

    std::vector<float> areas(n);
    for (int i = 0; i < n; i++)
    {
        areas[i] = objects[i].rect.area();
    }

    for (int i = 0; i < n; i++)
    {
        const ObjectInfo& a = objects[i];

        int keep = 1;
        for (int j = 0; j < (int)picked.size(); j++)
        {
            const ObjectInfo& b = objects[picked[j]];

            // intersection over union
            float inter_area = InterRectArea(a.rect, b.rect);
            float union_area = areas[i] + areas[picked[j]] - inter_area;
            // float IoU = inter_area / union_area
            if (inter_area / union_area > nms_threshold)
                keep = 0;
        }

        if (keep)
            picked.push_back(i);
    }
}
//
// insightface/detection/scrfd/mmdet/core/anchor/anchor_generator.py gen_single_level_base_anchors()
ncnn::Mat generate_anchors(int base_size, const ncnn::Mat& ratios, const ncnn::Mat& scales)
{
    int num_ratio = ratios.w;
    int num_scale = scales.w;

    ncnn::Mat anchors;
    anchors.create(4, num_ratio * num_scale);

    const float cx = 0;
    const float cy = 0;

    for (int i = 0; i < num_ratio; i++)
    {
        float ar = ratios[i];

        int r_w = round(base_size / sqrt(ar));
        int r_h = round(r_w * ar); //round(base_size * sqrt(ar));

        for (int j = 0; j < num_scale; j++)
        {
            float scale = scales[j];

            float rs_w = r_w * scale;
            float rs_h = r_h * scale;

            float* anchor = anchors.row(i * num_scale + j);

            anchor[0] = cx - rs_w * 0.5f;
            anchor[1] = cy - rs_h * 0.5f;
            anchor[2] = cx + rs_w * 0.5f;
            anchor[3] = cy + rs_h * 0.5f;
        }
    }

    return anchors;
}

int generate_grids_and_stride(const int target_size, std::vector<int>& strides, std::vector<GridAndStride>& grid_strides)
{
    for (auto stride : strides)
    {
        int num_grid = target_size / stride;
        for (int g1 = 0; g1 < num_grid; g1++)
        {
            for (int g0 = 0; g0 < num_grid; g0++)
            {
                grid_strides.push_back((GridAndStride){g0, g1, stride});
            }
        }
    }

    return 0;
}

float sigmoid(float x)
{
    return static_cast<float>(1.f / (1.f + exp(-x)));
}

}
