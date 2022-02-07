#include "p2pnet.hpp"

#include <math.h>

#ifdef OV_VULKAN
#include "gpu.h"
#endif // OV_VULKAN

namespace ovcounter {

void P2PNet::shift(int w, int h, int stride, std::vector<float> anchor_points,
                   std::vector<float> &shifted_anchor_points) {
  std::vector<float> x_, y_;
  for (int i = 0; i < w; i++) {
    float x = (i + 0.5) * stride;
    x_.push_back(x);
  }
  for (int i = 0; i < h; i++) {
    float y = (i + 0.5) * stride;
    y_.push_back(y);
  }

  std::vector<float> shift_x(w * h, 0), shift_y(w * h, 0);
  for (int i = 0; i < h; i++) {
    for (int j = 0; j < w; j++) {
      shift_x[i * w + j] = x_[j];
    }
  }
  for (int i = 0; i < h; i++) {
    for (int j = 0; j < w; j++) {
      shift_y[i * w + j] = y_[i];
    }
  }

  std::vector<float> shifts(w * h * 2, 0);
  for (int i = 0; i < w * h; i++) {
    shifts[i * 2] = shift_x[i];
    shifts[i * 2 + 1] = shift_y[i];
  }

  shifted_anchor_points.resize(2 * w * h * anchor_points.size() / 2, 0);
  for (int i = 0; i < w * h; i++) {
    for (int j = 0; j < anchor_points.size() / 2; j++) {
      float x = anchor_points[j * 2] + shifts[i * 2];
      float y = anchor_points[j * 2 + 1] + shifts[i * 2 + 1];
      shifted_anchor_points[i * anchor_points.size() / 2 * 2 + j * 2] = x;
      shifted_anchor_points[i * anchor_points.size() / 2 * 2 + j * 2 + 1] = y;
    }
  }
}

void P2PNet::generate_anchor_points(int stride, int row, int line,
                                    std::vector<float> &anchor_points) {
  float row_step = (float)stride / row;
  float line_step = (float)stride / line;

  std::vector<float> x_, y_;
  for (int i = 1; i < line + 1; i++) {
    float x = (i - 0.5) * line_step - (float)stride / 2;
    x_.push_back(x);
  }
  for (int i = 1; i < row + 1; i++) {
    float y = (i - 0.5) * row_step - (float)stride / 2;
    y_.push_back(y);
  }
  std::vector<float> shift_x(row * line, 0), shift_y(row * line, 0);
  for (int i = 0; i < row; i++) {
    for (int j = 0; j < line; j++) {
      shift_x[i * line + j] = x_[j];
    }
  }
  for (int i = 0; i < row; i++) {
    for (int j = 0; j < line; j++) {
      shift_y[i * line + j] = y_[i];
    }
  }
  anchor_points.resize(row * line * 2, 0);
  for (int i = 0; i < row * line; i++) {
    float x = shift_x[i];
    float y = shift_y[i];
    anchor_points[i * 2] = x;
    anchor_points[i * 2 + 1] = y;
  }
}

void P2PNet::generate_anchor_points(int img_w, int img_h,
                                    std::vector<int> pyramid_levels, int row,
                                    int line,
                                    std::vector<float> &all_anchor_points) {
  std::vector<std::pair<int, int>> image_shapes;
  std::vector<int> strides;
  for (int i = 0; i < pyramid_levels.size(); i++) {
    int new_h = floor((img_h + pow(2, pyramid_levels[i]) - 1) /
                      pow(2, pyramid_levels[i]));
    int new_w = floor((img_w + pow(2, pyramid_levels[i]) - 1) /
                      pow(2, pyramid_levels[i]));
    image_shapes.push_back(std::make_pair(new_w, new_h));
    strides.push_back(pow(2, pyramid_levels[i]));
  }

  all_anchor_points.clear();
  for (int i = 0; i < pyramid_levels.size(); i++) {
    std::vector<float> anchor_points;
    generate_anchor_points(pow(2, pyramid_levels[i]), row, line, anchor_points);
    std::vector<float> shifted_anchor_points;
    shift(image_shapes[i].first, image_shapes[i].second, strides[i],
          anchor_points, shifted_anchor_points);
    all_anchor_points.insert(all_anchor_points.end(),
                             shifted_anchor_points.begin(),
                             shifted_anchor_points.end());
  }
}

int P2PNet::CrowdCount(const unsigned char *rgbdata, int img_width,
                       int img_height, std::vector<ov::Keypoint> *keypoints) {

  if (!initialized_) {
    return 10000;
  }
  if (rgbdata == 0) {
    return 10001;
  }

  // pad to multiple of 32
  int w = img_width;
  int h = img_height;
  float scale = 1.f;
  if (w > h) {
    scale = (float)target_size / w;
    w = target_size;
    h = h * scale;
  } else {
    scale = (float)target_size / h;
    h = target_size;
    w = w * scale;
  }

  ncnn::Mat input = ncnn::Mat::from_pixels_resize(rgbdata, ncnn::Mat::PIXEL_RGB,
                                                  img_width, img_height, w, h);

  // pad to target_size rectangle
  int wpad = (w + 31) / 32 * 32 - w;
  int hpad = (h + 31) / 32 * 32 - h;
  ncnn::Mat in_pad;
  ncnn::copy_make_border(input, in_pad, hpad / 2, hpad - hpad / 2, wpad / 2,
                         wpad - wpad / 2, ncnn::BORDER_CONSTANT, 0.f);

  std::vector<int> pyramid_levels(1, 3);
  std::vector<float> all_anchor_points;
  generate_anchor_points(in_pad.w, in_pad.h, pyramid_levels, 2, 2,
                         all_anchor_points);

  ncnn::Mat anchor_points =
      ncnn::Mat(2, all_anchor_points.size() / 2, all_anchor_points.data());

  ncnn::Extractor ex = net_->create_extractor();
  ex.set_light_mode(light_mode_);
  ex.set_num_threads(num_threads);

  in_pad.substract_mean_normalize(mean_vals1, norm_vals1);

  ex.input("input", in_pad);
  ex.input("anchor", anchor_points);

  ncnn::Mat score, points;
  ex.extract("pred_scores", score);
  ex.extract("pred_points", points);

  keypoints->clear();
  for (int i = 0; i < points.h; i++) {
    float *score_data = score.row(i);
    float *points_data = points.row(i);
    if (score_data[1] > 0.5) {
      ov::Keypoint kpt;
      int x = (points_data[0] - ((float)wpad / 2)) / scale;
      int y = (points_data[1] - ((float)hpad / 2)) / scale;
      kpt.p = ov::Point2f(x, y);
      kpt.score = score_data[1];
      keypoints->push_back(kpt);
    }
  }
  return 0;
}

} // namespace ovcounter
