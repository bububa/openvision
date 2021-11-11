#ifndef _TRACKER_LIGHT_TRACK_H_
#define _TRACKER_LIGHT_TRACK_H_

#include "../tracker.hpp"
#include "net.h"

namespace ovtracker {

class LightTrack : public Tracker {
public:
  int Track(const unsigned char *rgbdata, int img_width, int img_height,
            ov::Rect *rect);

private:
  ncnn::Mat zf;
  ov::Rect template_box;
  ncnn::Mat temp;
  std::vector<float> grid_to_search_x;
  std::vector<float> grid_to_search_y;
  std::vector<float> window;
  ov::Point2f target_sz;
  ov::Point2f target_pos;
  const float mean_vals1[3] = {123.675f, 116.28f, 103.53f};
  const float norm_vals1[3] = {0.01712475f, 0.0175f, 0.01742919f};

  bool inited_;

  int init(const ov::Rect &rect, int screenWidth);
  ncnn::Mat get_template(ncnn::Mat templateImage, ov::Point2f pos);
  ncnn::Mat get_subwindow_tracking(ncnn::Mat im, ov::Point2f pos, int model_sz,
                                   int original_sz);
  void update(ncnn::Mat x_crop, ncnn::Mat zf_, ov::Point2f &target_pos_,
              ov::Point2f &target_sz_, float scale_z,
              std::vector<float> &cls_score);
};
} // namespace ovtracker
#endif // !_TRACKER_LIGHT_TRACK_H_
