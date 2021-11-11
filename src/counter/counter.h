#ifndef _COUNTER_C_H_
#define _COUNTER_C_H_

#include "../common/common.h"

#ifdef __cplusplus
#include "counter.hpp"
extern "C" {
#endif
typedef void *ICounter;
ICounter new_p2pnet_crowd_counter();
int crowd_count(ICounter d, const unsigned char *rgbdata, int img_width,
                int img_height, KeypointVector *pts);
#ifdef __cplusplus
}
#endif
#endif // !_COUNTER_C_H_
