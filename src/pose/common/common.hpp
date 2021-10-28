#ifndef _POSE_COMMON_H_
#define _POSE_COMMON_H_

#include "../../common/common.h"

namespace ovpose {
struct PoseKeypoint {
    ov::Point2f p;
    float prob;
};

struct PoseROI {
    ov::Rect rect;
    unsigned char *data;
    float score;
};
}
#endif // !_POSE_COMMON_H_
