#include "../detecter.h"
#include "yolox/yolox.hpp"
#include "nanodet/nanodet.hpp"

IHandDetecter new_yolox() {
    return new ovhand::Yolox();
}

IHandDetecter new_nanodet() {
    return new ovhand::Nanodet();
}

int extract_hand_rois(IHandDetecter d, const unsigned char* rgbdata, int img_width, int img_height, ObjectInfoVector* rois) {
	std::vector<ov::ObjectInfo> detected;
    int ret = static_cast<ovhand::Detecter*>(d)->Detect(rgbdata, img_width, img_height, detected);
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

namespace ovhand {
Detecter* YoloxFactory::CreateDetecter() {
	return new Yolox();
}
Detecter* NanodetFactory::CreateDetecter() {
	return new Nanodet();
}
}
