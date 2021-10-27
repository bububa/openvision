#include "../detecter.h"
#include "ultralight/ultralight.hpp"

IDetecter new_ultralight() {
    return new ov::Ultralight();
}

int extract_pose_rois(IDetecter d, const unsigned char* rgbdata, int img_width, int img_height, PoseROIVector* rois) {
	std::vector<PoseROI> detected;
    int ret = static_cast<ov::Detecter*>(d)->ExtractROIs(rgbdata, img_width, img_height, &detected);
    if (ret != 0) {
        return ret;
    }

    rois->length = detected.size();
    rois->items = (PoseROI*)malloc(rois->length * sizeof(PoseROI));
    for (size_t i = 0; i < detected.size(); ++i) {
        rois->items[i] = detected[i];
    }
    return 0;
}

int extract_pose_keypoints(IDetecter d, const PoseROI* roi, PoseKeypointVector* keypoints) {
    std::vector<PoseKeypoint> points;
    int ret = static_cast<ov::Detecter*>(d)->ExtractKeypoints(*roi, &points);
    if (ret != 0) {
        return ret;
    }
    keypoints->length = points.size();
    keypoints->points = (PoseKeypoint*)malloc(keypoints->length * sizeof(PoseKeypoint));
    for (size_t i = 0; i < points.size(); ++i) {
        keypoints->points[i] = points[i];
    }
    return 0;
}

namespace ov{
Detecter* UltralightFactory::CreateDetecter() {
	return new Ultralight();
}
}
