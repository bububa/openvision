#include "animegan2.hpp"

#ifdef OV_VULKAN
#include "gpu.h"
#endif // OV_VULKAN

namespace ovstyletransfer {

int AnimeGan2::Transform(const unsigned char *rgbdata, int img_width,
                         int img_height, Image *out) {
  if (!initialized_) {
    return 10000;
  }
  if (rgbdata == 0) {
    return 10001;
  }

  ncnn::Mat in =
      ncnn::Mat::from_pixels_resize(rgbdata, ncnn::Mat::PIXEL_RGB, img_width,
                                    img_height, target_size, target_size);

  in.substract_mean_normalize(mean_vals, norm_vals);
  ncnn::Extractor ex = net_->create_extractor();
  ex.set_light_mode(light_mode_);
  ex.set_num_threads(num_threads);

  ex.input("input", in);
  ncnn::Mat output;
  ex.extract("out", output);

  for (int c = 0; c < 3; ++c) {
    float *pImage = output.channel(c);
    for (int i = 0; i < target_size * target_size; i++) {
      pImage[i] = pImage[i] * 127.5 + 127.5;
    }
  }

  ncnn::Mat outimg;
  ncnn::resize_bicubic(output, outimg, img_width, img_height);

  out->width = outimg.w;
  out->height = outimg.h;
  out->channels = outimg.c;
  out->data = (unsigned char *)malloc(outimg.total());
  outimg.to_pixels(out->data, ncnn::Mat::PIXEL_RGB);
  return 0;
}

} // namespace ovstyletransfer
