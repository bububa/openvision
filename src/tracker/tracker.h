#ifndef _TRACKER_C_H_
#define _TRACKER_C_H_

#include "../common/common.h"

#ifdef __cplusplus
#include "tracker.hpp"
extern "C" {
#endif
typedef void *ITracker;
ITracker new_light_track();
int track_object(ITracker d, const unsigned char *rgbdata, int img_width,
                 int img_height, Rect *rect);
#ifdef __cplusplus
}
#endif
#endif // !_TRACKER_C_H_
