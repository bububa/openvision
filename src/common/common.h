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
typedef ov::Keypoint Keypoint;
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

typedef struct Keypoint {
    Point2f p;
    float prob;
} Keypoint;


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
void Point2fVectorSetValue(Point2fVector *p, int i, const Point2f* val);

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

typedef struct KeypointVector {
    Keypoint* points;
    int length;
} KeypointVector;

void FreeKeypointVector(KeypointVector *p);
void KeypointVectorSetValue(KeypointVector *p, int i, const Keypoint* val);

typedef struct ObjectInfoC {
    Rect rect;
    float prob;
    int label;
    KeypointVector* pts;
} ObjectInfo;

void FreeObjectInfo(ObjectInfo *p);

typedef struct ObjectInfoVector {
    ObjectInfo* items;
    int length;
} ObjectInfoVector;

void FreeObjectInfoVector(ObjectInfoVector *p);

#ifdef __cplusplus
}
#endif

#endif // !_COMMON_C_H_
