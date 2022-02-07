#ifndef _HAND_POSE3D_C_H_
#define _HAND_POSE3D_C_H_

#include "../common/common.h"

#ifdef __cplusplus
extern "C" {
#endif
typedef struct PalmObject {
  float score;
  float rotation;
  Point2fVector *rect;
  Point2fVector *landmarks;
  Point2fVector *skeleton;
  Point3dVector *skeleton3d;
} PalmObject;

typedef struct PalmObjectVector {
  PalmObject *items;
  int length;
} PalmObjectVector;

void FreePalmObject(PalmObject *obj);
void FreePalmObjectVector(PalmObjectVector *vec);

typedef void *IHandPose3DEstimator;
IHandPose3DEstimator new_mediapipe_hand();
void destroy_mediapipe_hand(IHandPose3DEstimator d);
int mediapipe_hand_load_model(IHandPose3DEstimator d, const char *palm_path,
                              const char *hand_path);
int mediapipe_hand_detect(IHandPose3DEstimator d, const unsigned char *rgbdata,
                          int img_width, int img_height, PalmObjectVector *vec);
#ifdef __cplusplus
}
#endif

#endif // !_HAND_POSE3D_C_H_
