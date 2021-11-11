#include "lighttrack.hpp"

#include <math.h>

#ifdef OV_VULKAN
#include "gpu.h"
#endif // OV_VULKAN

namespace ovtracker {

static std::vector<float> sc_f(std::vector<float> w, std::vector<float> h,
                               float sz) {
  std::vector<float> pad(16 * 16, 0);
  std::vector<float> sz2;
  for (int i = 0; i < 16; i++) {
    for (int j = 0; j < 16; j++) {
      pad[i * 16 + j] = (w[i * 16 + j] + h[i * 16 + j]) * 0.5;
    }
  }
  for (int i = 0; i < 16; i++) {
    for (int j = 0; j < 16; j++) {
      float t = sqrt((w[i * 16 + j] + pad[i * 16 + j]) *
                     (h[i * 16 + j] + pad[i * 16 + j])) /
                sz;

      sz2.push_back(std::max(t, (float)1.0 / t));
    }
  }

  return sz2;
}

static std::vector<float> rc_f(std::vector<float> w, std::vector<float> h,
                               ov::Point2f target_sz) {
  float ratio = target_sz.x / target_sz.y;
  std::vector<float> sz2;
  for (int i = 0; i < 16; i++) {
    for (int j = 0; j < 16; j++) {
      float t = ratio / (w[i * 16 + j] / h[i * 16 + j]);
      sz2.push_back(std::max(t, (float)1.0 / t));
    }
  }

  return sz2;
}

static float sz_whFun(ov::Point2f wh) {
  float pad = (wh.x + wh.y) * 0.5;
  float sz2 = (wh.x + pad) * (wh.y + pad);
  return sqrt(sz2);
}

ncnn::Mat LightTrack::get_subwindow_tracking(ncnn::Mat im, ov::Point2f pos,
                                             int model_sz, int original_sz) {
  float c = (float)(original_sz + 1) / 2;
  int context_xmin = round(pos.x - c);
  int context_xmax = context_xmin + original_sz - 1;
  int context_ymin = round(pos.y - c);
  int context_ymax = context_ymin + original_sz - 1;

  int left_pad = int(std::max(0, -context_xmin));
  int top_pad = int(std::max(0, -context_ymin));
  int right_pad = int(std::max(0, context_xmax - im.w + 1));
  int bottom_pad = int(std::max(0, context_ymax - im.h + 1));

  context_xmin += left_pad;
  context_xmax += left_pad;
  context_ymin += top_pad;
  context_ymax += top_pad;
  ncnn::Mat im_path_original;

  if (top_pad > 0 || left_pad > 0 || right_pad > 0 || bottom_pad > 0) {
    ncnn::Mat te_im =
        ncnn::Mat(im.w + top_pad + bottom_pad, im.h + left_pad + right_pad, 3);
    // te_im(cv::Rect(left_pad, top_pad, im.cols, im.rows)) = im;
    ncnn::copy_make_border(im, te_im, top_pad, bottom_pad, left_pad, right_pad,
                           ncnn::BORDER_CONSTANT, 0.f);
    // im_path_original = te_im(cv::Rect(context_xmin, context_ymin,
    // context_xmax - context_xmin + 1, context_ymax - context_ymin + 1));
    ncnn::copy_cut_border(te_im, im_path_original, context_ymin,
                          context_ymax + 1, context_xmin, context_xmax + 1);
  } else {
    // im_path_original = im(cv::Rect(context_xmin, context_ymin, context_xmax -
    // context_xmin + 1, context_ymax - context_ymin + 1));
    ncnn::copy_cut_border(im, im_path_original, context_ymin, context_ymax + 1,
                          context_xmin, context_xmax + 1);
  }

  ncnn::Mat im_path;
  ncnn::resize_bicubic(im_path_original, im_path, model_sz, model_sz);

  return im_path;
}

ncnn::Mat LightTrack::get_template(ncnn::Mat templateImage, ov::Point2f pos) {
  for (int i = 0; i < 16; i++) {
    for (int j = 0; j < 16; j++) {
      grid_to_search_x[i * 16 + j] = j * 16;
      grid_to_search_y[i * 16 + j] = i * 16;
    }
  }

  std::vector<float> hanning(16, 0);
  for (int i = 0; i < 16; i++) {
    float w = 0.5 - 0.5 * cos(2 * M_PI * i / 15);
    hanning[i] = w;
  }
  for (int i = 0; i < 16; i++) {

    for (int j = 0; j < 16; j++) {
      window[i * 16 + j] = hanning[i] * hanning[j];
    }
  }
  float hc_z = target_sz.y + 0.5 * (target_sz.x + target_sz.y);
  float wc_z = target_sz.x + 0.5 * (target_sz.x + target_sz.y);
  float s_z = sqrt(wc_z * hc_z);
  ncnn::Mat temp = get_subwindow_tracking(templateImage, pos, 127, s_z);

  ncnn::Extractor ex = net_->create_extractor();
  ex.set_light_mode(light_mode_);
  ex.set_num_threads(num_threads);

  ncnn::Mat ncnn_in1;
  ncnn::resize_bicubic(temp, ncnn_in1, 127, 127);
  ncnn_in1.substract_mean_normalize(mean_vals1, norm_vals1);

  ex.input("input", ncnn_in1);
  ncnn::Mat zf_;
  ex.extract("745", zf_);

  return zf_;
}

int LightTrack::init(const ov::Rect &rect, int screenWidth) {
  if (!inited_) {

    float scale = (double)temp.w / (double)screenWidth;
    template_box.x = rect.x * scale;
    template_box.y = rect.y * scale;
    template_box.width = rect.width * scale;
    template_box.height = rect.height * scale;
    target_pos = ov::Point2f(template_box.x + (float)template_box.width / 2,
                             template_box.y + (float)template_box.height / 2);
    target_sz.x = template_box.width;
    target_sz.y = template_box.height;

    zf = get_template(temp, target_pos);
    inited_ = true;
  }

  return 0;
}

void LightTrack::update(ncnn::Mat x_crop, ncnn::Mat zf_,
                        ov::Point2f &target_pos_, ov::Point2f &target_sz_,
                        float scale_z, std::vector<float> &cls_score) {
  target_sz_.x = target_sz_.x * scale_z;
  target_sz_.y = target_sz_.y * scale_z;

  ncnn::Extractor ex = net_->create_extractor();
  ex.set_light_mode(light_mode_);
  ex.set_num_threads(num_threads);

  ncnn::Mat ncnn_in1 = x_crop;
  ncnn_in1.substract_mean_normalize(mean_vals1, norm_vals1);

  ex.input("input", ncnn_in1);
  ex.input("temp", zf_);

  ncnn::Mat cls, reg;
  ex.extract("cls", cls);
  ex.extract("reg", reg);

  float *cls_data = (float *)cls.data;
  cls_score.clear();
  for (int i = 0; i < 16 * 16; i++) {
    cls_score.push_back(ov::sigmoid(cls_data[i]));
  }

  std::vector<float> pred_x1(16 * 16, 0), pred_y1(16 * 16, 0),
      pred_x2(16 * 16, 0), pred_y2(16 * 16, 0);

  float *reg_data1 = reg.channel(0);
  float *reg_data2 = reg.channel(1);
  float *reg_data3 = reg.channel(2);
  float *reg_data4 = reg.channel(3);
  for (int j = 0; j < 16; j++) {
    for (int k = 0; k < 16; k++) {
      pred_x1[j * 16 + k] =
          grid_to_search_x[j * 16 + k] - reg_data1[j * 16 + k];
      pred_y1[j * 16 + k] =
          grid_to_search_y[j * 16 + k] - reg_data2[j * 16 + k];
      pred_x2[j * 16 + k] =
          grid_to_search_x[j * 16 + k] + reg_data3[j * 16 + k];
      pred_y2[j * 16 + k] =
          grid_to_search_y[j * 16 + k] + reg_data4[j * 16 + k];
    }
  }

  std::vector<float> w(16 * 16, 0), h(16 * 16, 0);
  for (int i = 0; i < 16; i++) {
    for (int j = 0; j < 16; j++) {
      w[i * 16 + j] = pred_x2[i * 16 + j] - pred_x1[i * 16 + j];
      h[i * 16 + j] = pred_y2[i * 16 + j] - pred_y1[i * 16 + j];
    }
  }

  float sz_wh = sz_whFun(target_sz_);
  std::vector<float> s_c = sc_f(w, h, sz_wh);
  std::vector<float> r_c = rc_f(w, h, target_sz_);

  std::vector<float> penalty(16 * 16, 0);
  for (int i = 0; i < 16 * 16; i++) {
    penalty[i] = exp(-1 * (s_c[i] * r_c[i] - 1) * 0.062);
  }

  std::vector<float> pscore(16 * 16, 0);
  int r_max = 0, c_max = 0;
  float maxScore = 0;
  for (int i = 0; i < 16 * 16; i++) {
    pscore[i] = (penalty[i] * cls_score[i]) * (1 - 0.225) + window[i] * 0.225;
    if (pscore[i] > maxScore) {
      maxScore = pscore[i];
      r_max = floor(i / 16);
      c_max = ((float)i / 16 - r_max) * 16;
    }
  }

  float predx1 = pred_x1[r_max * 16 + c_max];
  float predy1 = pred_y1[r_max * 16 + c_max];
  float predx2 = pred_x2[r_max * 16 + c_max];
  float predy2 = pred_y2[r_max * 16 + c_max];

  float pred_xs = (predx1 + predx2) / 2;
  float pred_ys = (predy1 + predy2) / 2;
  float pred_w = predx2 - predx1;
  float pred_h = predy2 - predy1;

  float diff_xs = pred_xs - 256.f / 2;
  float diff_ys = pred_ys - 256.f / 2;

  diff_xs = diff_xs / scale_z;
  diff_ys = diff_ys / scale_z;
  pred_w = pred_w / scale_z;
  pred_h = pred_h / scale_z;

  target_sz_.x = target_sz_.x / scale_z;
  target_sz_.y = target_sz_.y / scale_z;

  float lr =
      penalty[r_max * 16 + c_max] * cls_score[r_max * 16 + c_max] * 0.765;
  float res_xs = target_pos_.x + diff_xs;
  float res_ys = target_pos_.y + diff_ys;
  float res_w = pred_w * lr + (1 - lr) * target_sz_.x;
  float res_h = pred_h * lr + (1 - lr) * target_sz_.y;

  target_pos_.x = res_xs;
  target_pos_.y = res_ys;
  target_sz_.x = target_sz_.x * (1 - lr) + lr * res_w;
  target_sz_.y = target_sz_.y * (1 - lr) + lr * res_h;
}

int LightTrack::Track(const unsigned char *rgbdata, int img_width,
                      int img_height, ov::Rect *rect) {

  if (!initialized_) {
    return 10000;
  }
  if (rgbdata == 0) {
    return 10001;
  }
  init(*rect, img_width);
  float hc_z = target_sz.y + 0.5 * (target_sz.x + target_sz.y);
  float wc_z = target_sz.x + 0.5 * (target_sz.x + target_sz.y);
  float s_z = sqrt(wc_z * hc_z);
  float scale_z = 127. / s_z;
  float d_search = (256. - 127.) / 2;
  float pad = d_search / scale_z;
  int s_x = round(s_z + 2 * pad);

  ncnn::Mat img = ncnn::Mat::from_pixels(rgbdata, ncnn::Mat::PIXEL_RGB,
                                         img_width, img_height);
  ncnn::Mat x_crop = get_subwindow_tracking(img, target_pos, 256, s_x);

  std::vector<float> cls_score;
  update(x_crop, zf, target_pos, target_sz, scale_z, cls_score);

  target_pos.x = std::max(0.f, std::min((float)img.w, target_pos.x));
  target_pos.y = std::max(0.f, std::min((float)img.h, target_pos.y));
  target_sz.x = std::max(10.f, std::min((float)img.w, target_sz.x));
  target_sz.y = std::max(10.f, std::min((float)img.h, target_sz.y));
  rect->x = target_pos.x - target_sz.x / 2;
  rect->y = target_pos.y - target_sz.y / 2;
  rect->width = target_sz.x;
  rect->height = target_sz.y;
  return 0;
}

} // namespace ovtracker
