#include "../hair.h"

#ifdef OV_VULKAN
#include "gpu.h"
#endif // OV_VULKAN

IHair new_hair() { return new ovface::Hair(); }

int hair_matting(IHair d, const unsigned char *rgbdata, int img_width,
                 int img_height, Image *out) {
  int ret = static_cast<ovface::Hair *>(d)->Matting(rgbdata, img_width,
                                                    img_height, out);
  if (ret != 0) {
    return ret;
  }
  return 0;
}

namespace ovface {

int Hair::Matting(const unsigned char *rgbdata, int img_width, int img_height,
                  Image *out) {
  if (!initialized_) {
    return 10000;
  }
  if (rgbdata == 0) {
    return 10001;
  }

  ncnn::Extractor ex = net_->create_extractor();
  ex.set_light_mode(light_mode_);
  ex.set_num_threads(num_threads);

  ncnn::Mat ncnn_in =
      ncnn::Mat::from_pixels_resize(rgbdata, ncnn::Mat::PIXEL_RGB, img_width,
                                    img_height, target_size, target_size);
  ncnn::Mat matting = ncnn::Mat(target_size, target_size, 3);

  ncnn_in.substract_mean_normalize(mean_vals, norm_vals);
  ex.input("input", ncnn_in);
  ncnn::Mat ncnn_out;
  ex.extract("1006", ncnn_out);

  for (int c = 0; c < 3; ++c) {
    float *pImage = matting.channel(c);
    for (int i = 0; i < target_size * target_size; i++) {
      const float alpha = ncnn_out[i];
      float value = 255 * alpha;
      value = std::max(std::min(value, 255.f), 0.f);
      pImage[i] = value;
    }
  }
  ncnn::Mat outimg;
  ncnn::resize_bicubic(matting, outimg, img_width, img_height);

  out->width = outimg.w;
  out->height = outimg.h;
  out->channels = outimg.c;
  out->data = (unsigned char *)malloc(outimg.total());
  outimg.to_pixels(out->data, ncnn::Mat::PIXEL_RGB);
  return 0;
}

} // namespace ovface
