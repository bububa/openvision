#ifndef _COUNTER_P2PNET_H_
#define _COUNTER_P2PNET_H_

#include "../counter.hpp"

namespace ovcounter {

class P2PNet : public Counter {
public:
  int CrowdCount(const unsigned char *rgbdata, int img_width, int img_height,
                 std::vector<ov::Keypoint> *keypoints);

private:
  const int target_size = 640;
  const float mean_vals1[3] = {123.675f, 116.28f, 103.53f};
  const float norm_vals1[3] = {0.01712475f, 0.0175f, 0.01742919f};

  void generate_anchor_points(int stride, int row, int line,
                              std::vector<float> &anchor_points);
  void generate_anchor_points(int img_w, int img_h,
                              std::vector<int> pyramid_levels, int row,
                              int line, std::vector<float> &all_anchor_points);
  void shift(int w, int h, int stride, std::vector<float> anchor_points,
             std::vector<float> &shifted_anchor_points);
};

} // namespace ovcounter
#endif // !_COUNTER_P2PNET_H_
