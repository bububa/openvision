#include "../pose3d.h"
#include "mediapipe/mediapipe.hpp"
#include <iostream>

void FreePalmObject(PalmObject *obj) {
  if (obj->rect != NULL) {
    FreePoint2fVector(obj->rect);
    obj->rect = NULL;
  }
  if (obj->skeleton != NULL) {
    FreePoint2fVector(obj->skeleton);
    obj->skeleton = NULL;
  }
  if (obj->skeleton3d != NULL) {
    FreePoint3dVector(obj->skeleton3d);
    obj->skeleton3d = NULL;
  }
  if (obj->landmarks != NULL) {
    FreePoint2fVector(obj->landmarks);
    obj->landmarks = NULL;
  }
}

void FreePalmObjectVector(PalmObjectVector *vec) {
  if (vec->items != NULL) {
    for (int i = 0; i < vec->length; i++) {
      FreePalmObject(&vec->items[i]);
    }
    free(vec->items);
    vec->items = NULL;
  }
}

IHandPose3DEstimator new_mediapipe_hand() {
  return new ovhand3d::MediapipeHand();
}

void destroy_mediapipe_hand(IHandPose3DEstimator d) {
  delete static_cast<ovhand3d::MediapipeHand *>(d);
}

int mediapipe_hand_load_model(IHandPose3DEstimator d, const char *palm_path,
                              const char *hand_path) {
  return static_cast<ovhand3d::MediapipeHand *>(d)->LoadModel(palm_path,
                                                              hand_path);
}

int mediapipe_hand_detect(IHandPose3DEstimator d, const unsigned char *rgbdata,
                          int img_width, int img_height,
                          PalmObjectVector *objects) {
  std::vector<ovhand3d::PalmObject> objs;

  int ret = static_cast<ovhand3d::MediapipeHand *>(d)->Detect(
      rgbdata, img_width, img_height, objs);
  if (ret != 0) {
    objects->length = 0;
    objects->items = NULL;
    return ret;
  }
  const size_t total_objs = objs.size();
  objects->length = total_objs;
  if (total_objs == 0) {
    objects->items = NULL;
    return 0;
  }

  objects->items = (PalmObject *)malloc(total_objs * sizeof(PalmObject));
  for (size_t i = 0; i < total_objs; ++i) {
    objects->items[i].score = objs[i].score;
    objects->items[i].rotation = objs[i].rotation;
    objects->items[i].rect = (Point2fVector *)malloc(sizeof(Point2fVector));
    objects->items[i].rect->length = 4;
    objects->items[i].rect->points = (Point2f *)malloc(4 * sizeof(Point2f));
    for (size_t j = 0; j < 4; ++j) {
      objects->items[i].rect->points[j] = objs[i].hand_pos[j];
    }
    objects->items[i].landmarks =
        (Point2fVector *)malloc(sizeof(Point2fVector));
    objects->items[i].landmarks->length = 7;
    objects->items[i].landmarks->points =
        (Point2f *)malloc(7 * sizeof(Point2f));
    for (size_t j = 0; j < 7; ++j) {
      objects->items[i].landmarks->points[j] = objs[i].landmarks[j];
    }
    const size_t total_skeleton = objs[i].skeleton.size();
    if (total_skeleton == 0) {
      objects->items[i].skeleton = NULL;
      objects->items[i].skeleton3d = NULL;
      continue;
    }
    const size_t total_skeleton3d = objs[i].skeleton3d.size();
    if (total_skeleton3d == 0) {
      objects->items[i].skeleton = NULL;
      objects->items[i].skeleton3d = NULL;
      continue;
    }
    objects->items[i].skeleton = (Point2fVector *)malloc(sizeof(Point2fVector));
    objects->items[i].skeleton->length = total_skeleton;
    objects->items[i].skeleton->points =
        (Point2f *)malloc(total_skeleton * sizeof(Point2f));
    objects->items[i].skeleton3d =
        (Point3dVector *)malloc(sizeof(Point3dVector));
    objects->items[i].skeleton3d->length = total_skeleton3d;
    objects->items[i].skeleton3d->points =
        (Point3d *)malloc(total_skeleton3d * sizeof(Point3d));
    for (size_t j = 0; j < total_skeleton; ++j) {
      objects->items[i].skeleton->points[j].x = objs[i].skeleton[j].x;
      objects->items[i].skeleton->points[j].y = objs[i].skeleton[j].y;
    }
    for (size_t j = 0; j < total_skeleton3d; ++j) {
      objects->items[i].skeleton3d->points[j].x = objs[i].skeleton3d[j].x;
      objects->items[i].skeleton3d->points[j].y = objs[i].skeleton3d[j].y;
      objects->items[i].skeleton3d->points[j].z = objs[i].skeleton3d[j].z;
    }
  }
  return 0;
}
