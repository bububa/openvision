#ifndef _COUNTER_H_
#define _COUNTER_H_

#include "../common/common.hpp"

namespace ovcounter {
class Counter : public ov::Estimator {
public:
  virtual int CrowdCount(const unsigned char *rgbdata, int img_width,
                         int img_height, std::vector<ov::Keypoint> *pts) = 0;
};
} // namespace ovcounter
#endif // !_COUNTER_H_
