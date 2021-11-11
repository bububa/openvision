#include "../segmentor.h"
#include "deeplabv3plus/deeplabv3plus.hpp"
#include "erdnet/erdnet.hpp"
#include "rvm/rvm.hpp"

IPoseSegmentor new_deeplabv3plus_pose_segmentor() {
    return new ovpose::Deeplabv3plusSegmentor();
}

IPoseSegmentor new_erdnet_pose_segmentor() {
    return new ovpose::ERDNetSegmentor();
}

IPoseSegmentor new_rvm_pose_segmentor(int w, int h) {
    return new ovpose::RVM(w, h);
}

int pose_segment_matting(IPoseSegmentor s, const unsigned char* rgbdata,
    int img_width, int img_height,
    Image* out) {
    int ret = static_cast<ovpose::Segmentor*>(s)->Matting(rgbdata, img_width, img_height, out);
    if (ret != 0) {
        return ret;
    }
    return 0;
}

int pose_segment_merge(IPoseSegmentor s, const unsigned char* rgbdata,
    int img_width, int img_height,
    const unsigned char* bgdata,
    int bg_width, int bg_height,
    Image* out) {
    int ret = static_cast<ovpose::Segmentor*>(s)->Merge(rgbdata, img_width, img_height, bgdata, bg_width, bg_height, out);
    if (ret != 0) {
        return ret;
    }
    return 0;
}

namespace ovpose {

Segmentor* Deeplabv3plusFactory::CreateSegmentor() {
    return new Deeplabv3plusSegmentor();
}

Segmentor* ERDNetFactory::CreateSegmentor() {
    return new ERDNetSegmentor();
}

}
