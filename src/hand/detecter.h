#ifndef _HAND_DETECTER_C_H_
#define _HAND_DETECTER_C_H_

#include "../common/common.h"

#ifdef __cplusplus
#include "detecter/detecter.hpp"
extern "C" {
#endif
    typedef void* IHandDetecter;
    IHandDetecter new_yolox();
    IHandDetecter new_nanodet();
    int extract_hand_rois(IHandDetecter d, const unsigned char* rgbdata,
        int img_width, int img_height,
        ObjectInfoVector* rois);
#ifdef __cplusplus
}
#endif
#endif // !_HAND_DETECTER_C_H_
