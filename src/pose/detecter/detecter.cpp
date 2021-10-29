#include "../detecter.h"
#include "ultralight/ultralight.hpp"

IPoseDetecter new_ultralight() {
    return new ovpose::Ultralight();
}

int extract_pose_rois(IPoseDetecter d, const unsigned char* rgbdata, int img_width, int img_height, ObjectInfoVector* rois) {
	std::vector<ov::ObjectInfo> detected;
    int ret = static_cast<ovpose::Detecter*>(d)->ExtractROIs(rgbdata, img_width, img_height, &detected);
    if (ret != 0) {
        return ret;
    }

    rois->length = detected.size();
    rois->items = (ObjectInfo*)malloc(rois->length * sizeof(ObjectInfo));
    for (size_t i = 0; i < detected.size(); ++i) {
        ov::ObjectInfo o = detected[i];
        rois->items[i] = ObjectInfo{
            o.rect,
            o.prob,
            o.label,
            NULL
        };
    }
    return 0;
}

int extract_pose_keypoints(IPoseDetecter d, const unsigned char* rgbdata, int img_width, int img_height, const Rect* rect, KeypointVector* keypoints) {
    std::vector<ov::Keypoint> points;
    int ret = static_cast<ovpose::Detecter*>(d)->ExtractKeypoints(rgbdata, img_width, img_height, *rect, &points);
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

namespace ovpose {
Detecter* UltralightFactory::CreateDetecter() {
	return new Ultralight();
}
}
