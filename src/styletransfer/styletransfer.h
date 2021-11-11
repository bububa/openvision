#ifndef _STYLE_TRANSFER_C_H_
#define _STYLE_TRANSFER_C_H_

#include "../common/common.h"

#ifdef __cplusplus
#include "styletransfer.hpp"
extern "C" {
#endif
typedef void *IStyleTransfer;
IStyleTransfer new_animegan2_style_transfer();
int style_transform(IStyleTransfer s, const unsigned char *rgbdata,
                    int img_width, int img_height, Image *out);
#ifdef __cplusplus
}
#endif
#endif // !_STYLE_TRANSFER_C_H_
