#ifndef _POSE_COMMON_H_
#define _POSE_COMMON_H_

#include "../../common/common.h"

namespace ov {
struct PoseKeypoint {
    Point2f p;
    float prob;
};

struct PoseROI {
    Rect rect;
    unsigned char *data;
    float score;
};
}
#endif // !_POSE_COMMON_H_
