#ifndef _FACE_EYE_H_
#define _FACE_EYE_H_

#include "../../common/common.hpp"
namespace ovface {
class Eye : public ov::Estimator {
public:
  virtual int Status(const unsigned char *rgbdata, int img_width,
                     int img_height, const ov::Rect rect,
                     std::vector<float> &cls_scores) = 0;
};
} // namespace ovface
#endif // !_FACE_EYE_H_
