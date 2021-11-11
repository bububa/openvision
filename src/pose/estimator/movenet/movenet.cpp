#include "movenet.hpp"
#include <math.h>
#include <string>

#ifdef OV_VULKAN
#include "gpu.h"
#endif // OV_VULKAN

namespace ovpose {

MoveNet::MoveNet(int model_type) : Estimator() {
  if (model_type == 0) {
    target_size = 192;
    kpt_scale = 0.02083333395421505;
    feature_size = 48;
  } else {
    target_size = 256;
    kpt_scale = 0.015625;
    feature_size = 64;
  }
  for (int i = 0; i < feature_size; i++) {
    std::vector<float> x, y;
    for (int j = 0; j < feature_size; j++) {
      x.push_back(j);
      y.push_back(i);
    }
    dist_y.push_back(y);
    dist_x.push_back(x);
  }
}

int MoveNet::ExtractKeypoints(const unsigned char *rgbdata, int img_width,
                              int img_height, const ov::Rect &rect,
                              std::vector<ov::Keypoint> *keypoints) {
  if (!initialized_) {
    return 10000;
  }
  if (rgbdata == 0) {
    return 10001;
  }
  keypoints->clear();
  int w = rect.width;
  int h = rect.height;
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

  //     size_t total_size = rect.width * rect.height * 3 * sizeof(unsigned
  //     char); unsigned char* data = (unsigned char*)malloc(total_size); const
  //     unsigned char *start_ptr = rgbdata;
  // #if defined(_OPENMP)
  // #pragma omp parallel for num_threads(num_threads)
  // #endif
  //     for(size_t i = 0; i < rect.height; ++i) {
  //         const unsigned char* srcCursor = start_ptr + ((i + rect.y) *
  //         img_width + rect.x) * 3; unsigned char* dstCursor = data + i *
  //         rect.width * 3; memcpy(dstCursor, srcCursor, sizeof(unsigned char)
  //         * 3 * rect.width);
  //     }

  //     ncnn::Mat in = ncnn::Mat::from_pixels_resize(data,
  //     ncnn::Mat::PIXEL_RGB, rect.width, rect.height, w, h);
  ncnn::Mat in = ncnn::Mat::from_pixels_roi_resize(
      rgbdata, ncnn::Mat::PIXEL_RGB, img_width, img_height, rect.x, rect.y,
      rect.width, rect.height, w, h);
  int wpad = target_size - w;
  int hpad = target_size - h;
  ncnn::Mat in_pad;
  ncnn::copy_make_border(in, in_pad, hpad / 2, hpad - hpad / 2, wpad / 2,
                         wpad - wpad / 2, ncnn::BORDER_CONSTANT, 0.f);
  //数据预处理
  in_pad.substract_mean_normalize(mean_vals, norm_vals);

  ncnn::Extractor ex = net_->create_extractor();
  ex.set_light_mode(light_mode_);
  ex.set_num_threads(num_threads);

  ex.input("input", in_pad);

  ncnn::Mat regress, center, heatmap, offset;

  ex.extract("regress", regress);
  ex.extract("offset", offset);
  ex.extract("heatmap", heatmap);
  ex.extract("center", center);

  float *center_data = (float *)center.data;
  float *heatmap_data = (float *)heatmap.data;
  float *offset_data = (float *)offset.data;

  // int top_index = 0;
  // float top_score = 0;

  int top_index = int(ov::argmax(center_data, center_data + center.h));
  float top_score = *std::max_element(center_data, center_data + center.h);

  int ct_y = (top_index / feature_size);
  int ct_x = top_index - ct_y * feature_size;

  std::vector<float> y_regress(num_joints), x_regress(num_joints);
  float *regress_data = (float *)regress.channel(ct_y).row(ct_x);
  for (size_t i = 0; i < num_joints; i++) {
    y_regress[i] = regress_data[i] + (float)ct_y;
    x_regress[i] = regress_data[i + num_joints] + (float)ct_x;
  }

  ncnn::Mat kpt_scores =
      ncnn::Mat(feature_size * feature_size, num_joints, sizeof(float));
  float *scores_data = (float *)kpt_scores.data;
  for (int i = 0; i < feature_size; i++) {
    for (int j = 0; j < feature_size; j++) {
      std::vector<float> score;
      for (int c = 0; c < num_joints; c++) {
        float y = (dist_y[i][j] - y_regress[c]) * (dist_y[i][j] - y_regress[c]);
        float x = (dist_x[i][j] - x_regress[c]) * (dist_x[i][j] - x_regress[c]);
        float dist_weight = sqrt(y + x) + 1.8;
        scores_data[c * feature_size * feature_size + i * feature_size + j] =
            heatmap_data[i * feature_size * num_joints + j * num_joints + c] /
            dist_weight;
      }
    }
  }
  std::vector<int> kpts_ys, kpts_xs;
  for (int i = 0; i < num_joints; i++) {
    // top_index = 0;
    // top_score = 0;
    top_index =
        int(ov::argmax(scores_data + feature_size * feature_size * i,
                       scores_data + feature_size * feature_size * (i + 1)));
    top_score =
        *std::max_element(scores_data + feature_size * feature_size * i,
                          scores_data + feature_size * feature_size * (i + 1));

    int top_y = (top_index / feature_size);
    int top_x = top_index - top_y * feature_size;
    kpts_ys.push_back(top_y);
    kpts_xs.push_back(top_x);
  }

  for (int i = 0; i < num_joints; i++) {
    float kpt_offset_x =
        offset_data[kpts_ys[i] * feature_size * num_joints * 2 +
                    kpts_xs[i] * num_joints * 2 + i * 2];
    float kpt_offset_y =
        offset_data[kpts_ys[i] * feature_size * num_joints * 2 +
                    kpts_xs[i] * num_joints * 2 + i * 2 + 1];

    float x = (kpts_xs[i] + kpt_offset_y) * kpt_scale * target_size;
    float y = (kpts_ys[i] + kpt_offset_x) * kpt_scale * target_size;

    ov::Keypoint kpt;
    kpt.p = ov::Point2f((x - ((float)wpad / 2)) / scale + rect.x,
                        (y - ((float)hpad / 2)) / scale + rect.y);
    kpt.score = heatmap_data[kpts_ys[i] * feature_size * num_joints +
                             kpts_xs[i] * num_joints + i];
    keypoints->push_back(kpt);
  }

  // free(data);
  return 0;
}

} // namespace ovpose
