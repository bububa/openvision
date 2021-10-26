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
typedef ov::FaceInfo FaceInfo;
typedef ov::TrackedFaceInfo TrackedFaceInfo;
typedef ov::HeadPose HeadPose;
#else

#define kFaceFeatureDim 128
#define kFaceNameDim 256

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


typedef struct FaceInfo {
	Rect location_;
	float score_;
	float keypoints_[10];
    bool mask_;
} FaceInfo;

typedef struct TrackedFaceInfo {
	FaceInfo face_info_;
	float iou_score_;
} TrackedFaceInfo;

typedef struct HeadPose
{
    float roll;
    float pitch;
    float yaw;
} HeadPose;

#endif

int get_gpu_count();
int create_gpu_instance();
void destroy_gpu_instance();

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

typedef struct FaceInfoVector {
    FaceInfo* faces;
    int length;
} FaceInfoVector;

void FreeFaceInfoVector(FaceInfoVector *p);
void NewFaceInfoVector(FaceInfoVector *v, int num);

typedef struct TrackedFaceInfoVector {
    TrackedFaceInfo* faces;
    int length;
} TrackedFaceInfoVector;

void FreeTrackedFaceInfoVector(TrackedFaceInfoVector *p);

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
