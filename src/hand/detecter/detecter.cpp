#include "../detecter.h"
#include "yolox/yolox.hpp"
#include "nanodet/nanodet.hpp"

IHandDetecter new_yolox() {
    return new ovhand::Yolox();
}

IHandDetecter new_nanodet() {
    return new ovhand::Nanodet();
}

int extract_hand_rois(IHandDetecter d, const unsigned char* rgbdata, int img_width, int img_height, HandROIVector* rois) {
	std::vector<HandROI> detected;
    int ret = static_cast<ovhand::Detecter*>(d)->Detect(rgbdata, img_width, img_height, &detected);
    if (ret != 0) {
        return ret;
    }

    rois->length = detected.size();
    rois->items = (HandROI*)malloc(rois->length * sizeof(HandROI));
    for (size_t i = 0; i < detected.size(); ++i) {
        rois->items[i] = detected[i];
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
