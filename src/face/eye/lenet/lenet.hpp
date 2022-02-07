#ifndef _FACE_EYE_LENET_H_
#define _FACE_EYE_LENET_H_
#include "../eye.hpp"

namespace ovface {
class LenetEye : public Eye {
public:
  int Status(const unsigned char *rgbdata, int img_width, int img_height,
             const ov::Rect rect, std::vector<float> &cls_scores);

private:
  const int target_width = 36;
  const int target_height = 28;
  const float mean_vals[1] = {60.f};
};
} // namespace ovface
#endif // !_FACE_EYE_LENET_H_
