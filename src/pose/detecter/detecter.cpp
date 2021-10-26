#include "../detecter.h"
#include "utralight/utralight.hpp"

IDetecter new_utralight() {
    return new ov::Utralight();
}

void destroy_detecter(IDetecter d) {
    delete static_cast<ov::Detecter*>(d);
}

int detecter_load_model(IDetecter d, const char *root_path){
    return static_cast<ov::Detecter*>(d)->LoadModel(root_path);
}

int extract_rois(IDetecter d, const unsigned char* rgbdata, int img_width, int img_height, ROIVector* rois) {
	std::vector<ROI> detected;
    int ret = static_cast<ov::Detecter*>(d)->ExtractROIs(rgbdata, img_width, img_height, &detected);
    if (ret != 0) {
        return ret;
    }

    rois->length = detected.size();
    rois->items = (ROI*)malloc(rois->length * sizeof(ROI));
    for (size_t i = 0; i < detected.size(); ++i) {
        rois->items[i] = detected[i];
    }
    return 0;
}

int extract_keypoints(IDetecter d, const ROI* roi, KeypointVector* keypoints) {
    std::vector<Keypoint> points;
    int ret = static_cast<ov::Detecter*>(d)->ExtractKeypoints(*roi, &points);
    if (ret != 0) {
        return ret;
    }
    keypoints->length = points.size();
    keypoints->points = (Keypoint*)malloc(keypoints->length * sizeof(Keypoint));
    for (size_t i = 0; i < points.size(); ++i) {
        keypoints->points[i] = points[i];
    }
    return 0;
}

namespace ov{
Detecter* UtralightFactory::CreateDetecter() {
	return new Utralight();
}
}
