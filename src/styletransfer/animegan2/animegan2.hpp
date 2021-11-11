#ifndef _STYLE_TRANSFER_ANIMEGAN2_H_
#define _STYLE_TRANSFER_ANIMEGAN2_H_

#include "../styletransfer.hpp"
#include "net.h"

namespace ovstyletransfer {
class AnimeGan2 : public StyleTransfer {
public:
  int Transform(const unsigned char *rgbdata, int img_width, int img_height,
                Image *out);

private:
  const float target_size = 512;
  const float mean_vals[3] = {127.5f, 127.5f, 127.5f};
  const float norm_vals[3] = {1 / 127.5f, 1 / 127.5f, 1 / 127.5f};
};
} // namespace ovstyletransfer
#endif // !_STYLE_TRANSFER_ANIMEGAN2_H_
