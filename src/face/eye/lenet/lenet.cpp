#include "lenet.hpp"

#ifdef OV_VULKAN
#include "gpu.h"
#endif // OV_VULKAN

namespace ovface {
int LenetEye::Status(const unsigned char *rgbdata, int img_width,
                     int img_height, const ov::Rect rect,
                     std::vector<float> &cls_scores) {
  if (!initialized_) {
    return 10000;
  }
  if (rgbdata == 0) {
    return 10001;
  }
  cls_scores.clear();
  ncnn::Mat in = ncnn::Mat::from_pixels_roi_resize(
      rgbdata, ncnn::Mat::PIXEL_RGB2GRAY, img_width, img_height, rect.x, rect.y,
      rect.width, rect.height, target_width, target_height); // PIXEL_GRAY
  in.substract_mean_normalize(mean_vals, 0);
  ncnn::Extractor ex = net_->create_extractor();
  ex.set_light_mode(light_mode_);
  ex.set_num_threads(num_threads);
  ex.input("data", in);

  ncnn::Mat out;
  ex.extract("prob", out);

  cls_scores.resize(out.c);
  for (int c = 0; c < out.c; c++) {
    const float *prob = out.channel(c);
    cls_scores[c] = prob[0];
  }

  return 0;
}

} // namespace ovface
