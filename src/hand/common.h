#ifndef _HAND_COMMON_C_H_
#define _HAND_COMMON_C_H_

#include "../common/common.h"
#ifdef __cplusplus
#include "common/common.hpp"
extern "C" {
#endif
#ifdef __cplusplus
typedef ovhand::HandROI HandROI;
#else

typedef struct HandROI {
    Rect rect;
    int label;
    float prob;
} HandROI;

#endif
typedef struct HandROIVector {
    HandROI* items;
    int length;
} HandROIVector;

void FreeHandROIVector(HandROIVector *p);
#ifdef __cplusplus
}
#endif
#endif // !_HAND_COMMON_C_H_
