#ifndef _STYLE_TRANSFER_H_
#define _STYLE_TRANSFER_H_

#include "../common/common.h"

namespace ovstyletransfer {
class StyleTransfer : public ov::Estimator {
public:
  virtual int Transform(const unsigned char *rgbdata, int img_width,
                        int img_height, Image *out) = 0;
};
} // namespace ovstyletransfer
#endif // !_STYLE_TRANSFER_H_
