#include "styletransfer.h"
#include "animegan2/animegan2.hpp"

IStyleTransfer new_animegan2_style_transfer() {
  return new ovstyletransfer::AnimeGan2();
}

int style_transform(IStyleTransfer s, const unsigned char *rgbdata,
                    int img_width, int img_height, Image *out) {
  int ret = static_cast<ovstyletransfer::StyleTransfer *>(s)->Transform(
      rgbdata, img_width, img_height, out);
  if (ret != 0) {
    return ret;
  }
  return 0;
}
