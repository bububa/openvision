#ifndef _FACE_HAIR_H_
#define _FACE_HAIR_H_

#include "../common/common.h"

namespace ovface {
class Hair : public ov::Estimator {
public:
  int Matting(const unsigned char *rgbdata, int img_width, int img_height,
              Image *out);

private:
  const int target_size = 288;
  const float mean_vals[3] = {123.675f, 116.28f, 103.53f};
  const float norm_vals[3] = {0.01712475f, 0.0175f, 0.01742919f};
};
} // namespace ovface
#endif // !_FACE_HAIR_H_
