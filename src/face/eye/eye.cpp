#include "../eye.h"
#include "lenet/lenet.hpp"

IEye new_lenet_eye() { return new ovface::LenetEye(); }

int eye_status(IEye d, const unsigned char *rgbdata, int img_width,
               int img_height, const Rect *rect, FloatVector *cls_scores) {
  std::vector<float> scores;
  int ret = static_cast<ovface::Eye *>(d)->Status(rgbdata, img_width,
                                                  img_height, *rect, scores);
  if (ret != 0) {
    return ret;
  }
  cls_scores->length = scores.size();
  cls_scores->values = (float *)malloc(cls_scores->length * sizeof(float));
  for (int i = 0; i < cls_scores->length; ++i) {
    cls_scores->values[i] = scores[i];
  }
  return 0;
}
