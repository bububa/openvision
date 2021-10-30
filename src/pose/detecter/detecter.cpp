#include "../detecter.h"
#include "ultralight/ultralight.hpp"

IPoseDetecter new_ultralight() {
    return new ovpose::Ultralight();
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
        rois->items[i] = ObjectInfo{
            o.rect,
            o.score,
            o.label,
            NULL
        };
    }
    return 0;
}

namespace ovpose {
Detecter* UltralightFactory::CreateDetecter() {
	return new Ultralight();
}
}
