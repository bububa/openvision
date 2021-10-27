#ifndef _REALESRGAN_C_H
#define _REALESRGAN_C_H

#include "../common/common.h"

#ifdef __cplusplus
#include "realesrgan.hpp"
extern "C" {
#endif
    typedef void* RealEsrgan;
    RealEsrgan new_realesrgan(int gpuid, bool _ttad_model);
    int realesrgan_process(RealEsrgan r, const unsigned char* rgbdata, int img_width, int img_height, int scale, Bytes* output); 
#ifdef __cplusplus
}
#endif
#endif // _REALESRGAN_C_H
