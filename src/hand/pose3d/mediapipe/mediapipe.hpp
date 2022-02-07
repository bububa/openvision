#ifndef _HAND_POSE3D_MEDIAPIPE_H_
#define _HAND_POSE3D_MEDIAPIPE_H_

#include "../../../common/common.hpp"
#include <net.h>

namespace ovhand3d {

struct PalmObject {
  float score;
  ov::Point2f landmarks[7];
  float rotation;

  float hand_cx;
  float hand_cy;
  float hand_w;
  float hand_h;
  ov::Point2f hand_pos[4];

  std::vector<ov::Point2f> skeleton;
  std::vector<ov::Point3d> skeleton3d;
};

struct DetectRegion {
  float score;
  ov::Point2f topleft;
  ov::Point2f btmright;
  ov::Point2f landmarks[7];

  float rotation;
  ov::Point2f roi_center;
  ov::Point2f roi_size;
  ov::Point2f roi_coord[4];
};

struct Anchor {
  float x_center, y_center, w, h;
};

struct AnchorsParams {
  int input_size_width;
  int input_size_height;

  float min_scale;
  float max_scale;

  float anchor_offset_x;
  float anchor_offset_y;

  int num_layers;
  std::vector<int> feature_map_width;
  std::vector<int> feature_map_height;
  std::vector<int> strides;
  std::vector<float> aspect_ratios;
};

class MediapipeHand : public ov::EstimatorBase {
public:
  MediapipeHand();
  ~MediapipeHand();
  int LoadModel(const char *palm_model, const char *hand_model);
  int Detect(const unsigned char *rgbdata, int img_width, int img_heidht,
             std::vector<PalmObject> &objects);
  float GetLandmarks(ncnn::Mat in, float tm[6],
                     std::vector<ov::Point2f> &skeleton,
                     std::vector<ov::Point3d> &skeleton3d);
  void set_light_mode(bool mode);
  void set_num_threads(int n);

private:
  ncnn::Net *palm_net_ = NULL;
  ncnn::Net *hand_net_ = NULL;
  ncnn::PoolAllocator palm_workspace_allocator_;
  ncnn::UnlockedPoolAllocator palm_blob_allocator_;
  ncnn::PoolAllocator hand_workspace_allocator_;
  ncnn::UnlockedPoolAllocator hand_blob_allocator_;
  bool initialized_ = false;
  bool light_mode_ = true;
  std::vector<Anchor> anchors;
  float prob_threshold = 0.55f;
  float nms_threshold = 0.3f;
  const int target_size = 192;
  const float mean_vals[3] = {0.f, 0.f, 0.f};
  const float norm_vals[3] = {1 / 255.f, 1 / 255.f, 1 / 255.f};
};
} // namespace ovhand3d
#endif // !_HAND_POSE3D_MEDIAPIPE_H_
