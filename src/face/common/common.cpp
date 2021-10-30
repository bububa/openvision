#include "../common.h"
#include <math.h>

#ifdef OV_VULKAN
#include "gpu.h"
#endif // OV_VULKAN

void FreeRectVector(RectVector *p) {
    if (p->rects != NULL) {
        free(p->rects);
        p->rects = NULL;
    }
}

void FreeFaceInfoVector(FaceInfoVector *p) {
    if (p->faces != NULL) {
        free(p->faces);
        p->faces = NULL;
    }
}

void NewFaceInfoVector(FaceInfoVector *v, int num) {
    v->length = num;
    v->faces = (FaceInfo*)(malloc(num * sizeof(FaceInfo)));
}

void FreeTrackedFaceInfoVector(TrackedFaceInfoVector *p) {
    if (p->faces != NULL) {
        free(p->faces);
        p->faces = NULL;
    }
}

namespace ovface {

void qsort_descent_inplace(std::vector<FaceInfo>& objects, int left, int right)
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

void qsort_descent_inplace(std::vector<FaceInfo>& objects) 
{
    if (objects.empty())
        return;

    qsort_descent_inplace(objects, 0, objects.size() - 1);
}

void nms_sorted_bboxes(const std::vector<FaceInfo>& objects, std::vector<int>& picked, float nms_threshold)
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
        const FaceInfo& a = objects[i];

        int keep = 1;
        for (int j = 0; j < (int)picked.size(); j++)
        {
            const FaceInfo& b = objects[picked[j]];

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

float CalculateSimilarity(const std::vector<float>&feature1, const std::vector<float>& feature2, int threads_num) {
	if (feature1.size() != feature2.size()) {
		return 10003;
	}
	float inner_product = 0.0f;
	float feature_norm1 = 0.0f;
	float feature_norm2 = 0.0f;
#ifdef OV_OPENMP
#pragma omp parallel for num_threads(threads_num)
#endif
	for(int i = 0; i < kFaceFeatureDim; ++i) {
		inner_product += feature1[i] * feature2[i];
		feature_norm1 += feature1[i] * feature1[i];
		feature_norm2 += feature2[i] * feature2[i];
	}
	return inner_product / sqrt(feature_norm1) / sqrt(feature_norm2);
}

}
