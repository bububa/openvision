#ifndef _POSE_COMMON_H_
#define _POSE_COMMON_H_

#include "../../common/common.h"

namespace ov {
struct Keypoint {
    Point2f p;
    float prob;
};

struct ROI {
    Rect rect;
    unsigned char *data;
    float score;
};
}
#endif // !_POSE_COMMON_H_
