#ifndef _TRACKER_H_
#define _TRACKER_H_

#include "../common/common.hpp"

namespace ovtracker {
class Tracker : public ov::Estimator {
public:
  virtual int Track(const unsigned char *rgbdata, int img_width, int img_height,
                    ov::Rect *rect) = 0;
};
} // namespace ovtracker
#endif // !_TRACKER_H_
