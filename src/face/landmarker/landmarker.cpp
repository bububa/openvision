#include "landmarker.h"
#include "zqlandmarker/zqlandmarker.hpp"
#include "insightface/insightface.hpp"

ILandmarker new_zq() {
    return new mirror::ZQLandmarker();
}

ILandmarker new_insightface() {
    return new mirror::InsightfaceLandmarker();
}

void destroy_landmarker(ILandmarker m) {
    delete static_cast<mirror::Landmarker*>(m);
}

int landmarker_load_model(ILandmarker m, const char *root_path) {
    return static_cast<mirror::Landmarker*>(m)->LoadModel(root_path);
}

int extract_keypoints(
    ILandmarker m, 
    const unsigned char* rgbdata, 
    int img_width, 
    int img_height, 
    const Rect* face, 
    Point2fVector* keypoints) {
    std::vector<Point2f> points;
    int ret = static_cast<mirror::Landmarker*>(m)->ExtractKeypoints(rgbdata, img_width, img_height, *face, &points);
    if (ret != 0) {
        return ret;
    }
    keypoints->length = points.size();
    keypoints->points = (Point2f *)malloc(keypoints->length * sizeof(Point2f));
    for (size_t i = 0; i < points.size(); ++i) {
        keypoints->points[i] = points[i];
    }
    return 0;
}

namespace mirror {
Landmarker* ZQLandmarkerFactory::CreateLandmarker() {
	return new ZQLandmarker();
}

Landmarker* InsightfaceLandmarkerFactory::CreateLandmarker() {
	return new InsightfaceLandmarker();
}

}
