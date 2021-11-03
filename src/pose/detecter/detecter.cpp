#include "../detecter.h"
#include "ultralight/ultralight.hpp"
#include "openpose/openpose.hpp"

IPoseDetecter new_ultralight() {
    return new ovpose::Ultralight();
}

IPoseDetecter new_openpose() {
    return new ovpose::OpenPose();
}

int detect_pose(IPoseDetecter d, const unsigned char* rgbdata, int img_width, int img_height, ObjectInfoVector* rois) {
	std::vector<ov::ObjectInfo> detected;
    int ret = static_cast<ovpose::Detecter*>(d)->Detect(rgbdata, img_width, img_height, &detected);
    if (ret != 0) {
        return ret;
    }

    rois->length = detected.size();
    rois->items = (ObjectInfo*)malloc(rois->length * sizeof(ObjectInfo));
    for (size_t i = 0; i < detected.size(); ++i) {
        ov::ObjectInfo o = detected[i];
        rois->items[i].rect = o.rect;
        rois->items[i].score = o.score;
        rois->items[i].label = o.label;
        if (o.pts.size() > 0) {
            rois->items[i].pts = (KeypointVector*)malloc(sizeof(KeypointVector));
            rois->items[i].pts->length = o.pts.size();
            rois->items[i].pts->points = (Keypoint*)malloc(o.pts.size() * sizeof(Keypoint));
            for (size_t j=0; j < o.pts.size(); ++j) {
                ov::Keypoint pt = o.pts.at(j);
                rois->items[i].pts->points[j] = pt; 
            }
        } else {
            rois->items[i].pts = NULL;
        }
    }
    return 0;
}

namespace ovpose {
Detecter* UltralightFactory::CreateDetecter() {
	return new Ultralight();
}
Detecter* OpenPoseFactory::CreateDetecter() {
	return new OpenPose();
}
}
