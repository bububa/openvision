#ifndef _COMMON_C_H_
#define _COMMON_C_H_

#ifdef __cplusplus
#include "common.hpp"
extern "C" {
#endif

#ifdef __cplusplus
typedef ov::Size Size;
typedef ov::Point Point;
typedef ov::Point2f Point2f;
typedef ov::Rect Rect;
#else

// Wrapper for an individual cv::cvSize
typedef struct Size {
    int width;
    int height;
} Size;

// Wrapper for an individual cv::cvPoint
typedef struct Point {
    int x;
    int y;
} Point;

// Wrapper for an individual cv::Point2f
typedef struct Point2f {
    float x;
    float y;
} Point2f;


// Wrapper for an individual cv::Rect
typedef struct Rect {
    int x;
    int y;
    int width;
    int height;
} Rect;

#endif

typedef void* IEstimator;

int get_gpu_count();
int create_gpu_instance();
void destroy_gpu_instance();
int load_model(IEstimator e, const char* root_path);
void destroy_estimator(IEstimator e);

typedef struct Point2fVector {
    Point2f* points;
    int length;
} Point2fVector;

void FreePoint2fVector(Point2fVector *p);

typedef struct RectVector {
    Rect* rects;
    int length;
} RectVector;

void FreeRectVector(RectVector *p);

typedef struct FloatVector {
    float* values;
    int length;
} FloatVector;

void FreeFloatVector(FloatVector *p);

typedef struct Bytes {
    unsigned char* values;
    int length;
} Bytes;

void FreeBytes(Bytes *p);

#ifdef __cplusplus
}
#endif

#endif // !_COMMON_C_H_
