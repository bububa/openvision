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

namespace ov {

float CalculateSimilarity(const std::vector<float>&feature1, const std::vector<float>& feature2) {
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
