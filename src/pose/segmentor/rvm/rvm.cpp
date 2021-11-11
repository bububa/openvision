#include "rvm.hpp"
#include <math.h>
#include <string>

#ifdef OV_VULKAN
#include "gpu.h"
#endif // OV_VULKAN

namespace ovpose {

RVM::RVM(int w, int h) : Segmentor() {
  target_width = w;
  target_height = h;
  r1i = ncnn::Mat(target_width / 4, target_height / 4, 16);
  r2i = ncnn::Mat(target_width / 8, target_height / 8, 20);
  r3i = ncnn::Mat(target_width / 16, target_height / 16, 40);
  r4i = ncnn::Mat(target_width / 32, target_height / 32, 64);
  // init 0.
  r1i.fill(0.f);
  r2i.fill(0.f);
  r3i.fill(0.f);
  r4i.fill(0.f);
}

int RVM::Matting(const unsigned char *rgbdata, int img_width, int img_height,
                 Image *out) {
  if (!initialized_) {
    return 10000;
  }
  if (rgbdata == 0) {
    return 10001;
  }

  ncnn::Mat in =
      ncnn::Mat::from_pixels_resize(rgbdata, ncnn::Mat::PIXEL_RGB, img_width,
                                    img_height, target_height, target_height);
  ncnn::Mat in1 = ncnn::Mat::from_pixels_resize(
      rgbdata, ncnn::Mat::PIXEL_RGB, img_width, img_height, target_width / 2,
      target_height / 2);
  ncnn::Mat matting = ncnn::Mat(target_width, target_height, 3);

  in.substract_mean_normalize(mean_vals, norm_vals);
  in1.substract_mean_normalize(mean_vals1, norm_vals1);

  ncnn::Extractor ex = net_->create_extractor();
  ex.set_light_mode(light_mode_);
  ex.set_num_threads(num_threads);

  ex.input("src1", in1);
  ex.input("src2", in);
  ex.input("r1i", r1i);
  ex.input("r2i", r2i);
  ex.input("r3i", r3i);
  ex.input("r4i", r4i);

  ncnn::Mat r1o, r2o, r3o, r4o;
  ex.extract("r4o", r4o);
  ex.extract("r3o", r3o);
  ex.extract("r2o", r2o);
  ex.extract("r1o", r1o);

  ncnn::Mat fgr, pha;
  ex.extract("pha", pha);
  ex.extract("fgr", fgr);

  float *phaptr = pha;
  std::cout << "c:" << pha.c << ", w:" << pha.w << ", h:" << pha.h
            << ", v:" << phaptr[0] << std::endl;
  for (int c = 0; c < 3; ++c) {
    float *pImage = matting.channel(c);
    for (int i = 0; i < target_width * target_height; i++) {
      pImage[i] = pha[i] * 255;
    }
  }

  ncnn::Mat outimg;
  ncnn::resize_bicubic(matting, outimg, img_width, img_height);

  out->width = outimg.w;
  out->height = outimg.h;
  out->channels = outimg.c;
  out->data = (unsigned char *)malloc(outimg.total());
  outimg.to_pixels(out->data, ncnn::Mat::PIXEL_RGB);

  r1i.clone_from(r1o); // deepcopy
  r2i.clone_from(r2o); // deepcopy
  r3i.clone_from(r3o); // deepcopy
  r4i.clone_from(r4o); // deepcopy

  context_is_update = true;
  return 0;
}

int RVM::Merge(const unsigned char *rgbdata, int img_width, int img_height,
               const unsigned char *bgdata, int bg_width, int bg_height,
               Image *out) {
  if (!initialized_) {
    return 10000;
  }
  if (rgbdata == 0) {
    return 10001;
  }

  ncnn::Mat in =
      ncnn::Mat::from_pixels_resize(rgbdata, ncnn::Mat::PIXEL_RGB, img_width,
                                    img_height, target_height, target_width);
  ncnn::Mat in1 = ncnn::Mat::from_pixels_resize(
      rgbdata, ncnn::Mat::PIXEL_RGB, img_width, img_height, target_height / 2,
      target_width / 2);
  ncnn::Mat bg =
      ncnn::Mat::from_pixels_resize(bgdata, ncnn::Mat::PIXEL_RGB, bg_width,
                                    bg_height, target_width, target_height);
  ncnn::Mat matting = in.clone();

  in.substract_mean_normalize(mean_vals, norm_vals);
  in1.substract_mean_normalize(mean_vals1, norm_vals1);

  ncnn::Extractor ex = net_->create_extractor();
  ex.set_light_mode(light_mode_);
  ex.set_num_threads(num_threads);

  ex.input("src1", in1);
  ex.input("src2", in);
  ex.input("r1i", r1i);
  ex.input("r2i", r2i);
  ex.input("r3i", r3i);
  ex.input("r4i", r4i);

  ncnn::Mat r1o, r2o, r3o, r4o;
  ex.extract("r4o", r4o);
  ex.extract("r3o", r3o);
  ex.extract("r2o", r2o);
  ex.extract("r1o", r1o);

  ncnn::Mat fgr, pha;
  ex.extract("fgr", fgr);
  ex.extract("pha", pha);

  for (int c = 0; c < 3; ++c) {
    float *pImage = matting.channel(c);
    for (int i = 0; i < target_width * target_height; i++) {
      float alpha = pha[i];
      float value =
          fgr.channel(c)[i] * 255 * alpha + bg.channel(c)[i] * (1 - alpha);
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

  r1i.clone_from(r1o); // deepcopy
  r2i.clone_from(r2o); // deepcopy
  r3i.clone_from(r3o); // deepcopy
  r4i.clone_from(r4o); // deepcopy

  context_is_update = true;

  return 0;
}

} // namespace ovpose
