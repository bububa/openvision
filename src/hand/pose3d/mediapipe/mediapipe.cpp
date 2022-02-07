#include "mediapipe.hpp"
#include "mat.h"
#include <math.h>

namespace ovhand3d {

static float calculate_scale(float min_scale, float max_scale, int stride_index,
                             int num_strides) {
  if (num_strides == 1)
    return (min_scale + max_scale) * 0.5f;
  else
    return min_scale +
           (max_scale - min_scale) * 1.0 * stride_index / (num_strides - 1.0f);
}

static void generate_anchors(std::vector<Anchor> &anchors,
                             const AnchorsParams &anchor_params) {
  int layer_id = 0;
  for (int layer_id = 0; layer_id < anchor_params.strides.size();) {
    std::vector<float> anchor_height;
    std::vector<float> anchor_width;
    std::vector<float> aspect_ratios;
    std::vector<float> scales;

    int last_same_stride_layer = layer_id;
    while (last_same_stride_layer < (int)anchor_params.strides.size() &&
           anchor_params.strides[last_same_stride_layer] ==
               anchor_params.strides[layer_id]) {
      const float scale =
          calculate_scale(anchor_params.min_scale, anchor_params.max_scale,
                          last_same_stride_layer, anchor_params.strides.size());
      {
        for (int aspect_ratio_id = 0;
             aspect_ratio_id < (int)anchor_params.aspect_ratios.size();
             aspect_ratio_id++) {
          aspect_ratios.push_back(anchor_params.aspect_ratios[aspect_ratio_id]);
          scales.push_back(scale);
        }

        const float scale_next =
            last_same_stride_layer == (int)anchor_params.strides.size() - 1
                ? 1.0f
                : calculate_scale(
                      anchor_params.min_scale, anchor_params.max_scale,
                      last_same_stride_layer + 1, anchor_params.strides.size());
        scales.push_back(sqrt(scale * scale_next));
        aspect_ratios.push_back(1.0);
      }
      last_same_stride_layer++;
    }

    for (int i = 0; i < (int)aspect_ratios.size(); ++i) {
      const float ratio_sqrts = sqrt(aspect_ratios[i]);
      anchor_height.push_back(scales[i] / ratio_sqrts);
      anchor_width.push_back(scales[i] * ratio_sqrts);
    }

    int feature_map_height = 0;
    int feature_map_width = 0;
    const int stride = anchor_params.strides[layer_id];
    feature_map_height = ceil(1.0f * anchor_params.input_size_height / stride);
    feature_map_width = ceil(1.0f * anchor_params.input_size_width / stride);

    for (int y = 0; y < feature_map_height; ++y) {
      for (int x = 0; x < feature_map_width; ++x) {
        for (int anchor_id = 0; anchor_id < (int)anchor_height.size();
             ++anchor_id) {
          const float x_center =
              (x + anchor_params.anchor_offset_x) * 1.0f / feature_map_width;
          const float y_center =
              (y + anchor_params.anchor_offset_y) * 1.0f / feature_map_height;

          Anchor new_anchor;
          new_anchor.x_center = x_center;
          new_anchor.y_center = y_center;

          new_anchor.w = 1.0f;
          new_anchor.h = 1.0f;

          anchors.push_back(new_anchor);
        }
      }
    }
    layer_id = last_same_stride_layer;
  }
}

static void create_ssd_anchors(int input_w, int input_h,
                               std::vector<Anchor> &anchors) {
  AnchorsParams anchor_options;
  anchor_options.num_layers = 4;
  anchor_options.min_scale = 0.1484375;
  anchor_options.max_scale = 0.75;
  anchor_options.input_size_height = 192;
  anchor_options.input_size_width = 192;
  anchor_options.anchor_offset_x = 0.5f;
  anchor_options.anchor_offset_y = 0.5f;
  anchor_options.strides.push_back(8);
  anchor_options.strides.push_back(16);
  anchor_options.strides.push_back(16);
  anchor_options.strides.push_back(16);
  anchor_options.aspect_ratios.push_back(1.0);
  generate_anchors(anchors, anchor_options);
}

static int decode_bounds(std::list<DetectRegion> &region_list,
                         float score_thresh, int input_img_w, int input_img_h,
                         float *scores_ptr, float *bboxes_ptr,
                         std::vector<Anchor> &anchors) {
  DetectRegion region;
  int i = 0;
  for (auto &anchor : anchors) {
    float score = ov::sigmoid(scores_ptr[i]);

    if (score > score_thresh) {
      float *p = bboxes_ptr + (i * 18);

      float cx = p[0] / input_img_w + anchor.x_center;
      float cy = p[1] / input_img_h + anchor.y_center;
      float w = p[2] / input_img_w;
      float h = p[3] / input_img_h;

      ov::Point2f topleft, btmright;
      topleft.x = cx - w * 0.5f;
      topleft.y = cy - h * 0.5f;
      btmright.x = cx + w * 0.5f;
      btmright.y = cy + h * 0.5f;

      region.score = score;
      region.topleft = topleft;
      region.btmright = btmright;

      for (int j = 0; j < 7; j++) {
        float lx = p[4 + (2 * j) + 0];
        float ly = p[4 + (2 * j) + 1];
        lx += anchor.x_center * input_img_w;
        ly += anchor.y_center * input_img_h;
        lx /= (float)input_img_w;
        ly /= (float)input_img_h;

        region.landmarks[j].x = lx;
        region.landmarks[j].y = ly;
      }

      region_list.push_back(region);
    }
    i++;
  }
  return 0;
}

static float calc_intersection_over_union(DetectRegion &region0,
                                          DetectRegion &region1) {
  float sx0 = region0.topleft.x;
  float sy0 = region0.topleft.y;
  float ex0 = region0.btmright.x;
  float ey0 = region0.btmright.y;
  float sx1 = region1.topleft.x;
  float sy1 = region1.topleft.y;
  float ex1 = region1.btmright.x;
  float ey1 = region1.btmright.y;

  float xmin0 = std::min(sx0, ex0);
  float ymin0 = std::min(sy0, ey0);
  float xmax0 = std::max(sx0, ex0);
  float ymax0 = std::max(sy0, ey0);
  float xmin1 = std::min(sx1, ex1);
  float ymin1 = std::min(sy1, ey1);
  float xmax1 = std::max(sx1, ex1);
  float ymax1 = std::max(sy1, ey1);

  float area0 = (ymax0 - ymin0) * (xmax0 - xmin0);
  float area1 = (ymax1 - ymin1) * (xmax1 - xmin1);
  if (area0 <= 0 || area1 <= 0)
    return 0.0f;

  float intersect_xmin = std::max(xmin0, xmin1);
  float intersect_ymin = std::max(ymin0, ymin1);
  float intersect_xmax = std::min(xmax0, xmax1);
  float intersect_ymax = std::min(ymax0, ymax1);

  float intersect_area = std::max(intersect_ymax - intersect_ymin, 0.0f) *
                         std::max(intersect_xmax - intersect_xmin, 0.0f);

  return intersect_area / (area0 + area1 - intersect_area);
}

static int non_max_suppression(std::list<DetectRegion> &region_list,
                               std::list<DetectRegion> &region_nms_list,
                               float iou_thresh) {
  region_list.sort([](DetectRegion &v1, DetectRegion &v2) {
    return v1.score > v2.score ? true : false;
  });

  for (auto itr = region_list.begin(); itr != region_list.end(); itr++) {
    DetectRegion region_candidate = *itr;

    int ignore_candidate = false;
    for (auto itr_nms = region_nms_list.rbegin();
         itr_nms != region_nms_list.rend(); itr_nms++) {
      DetectRegion region_nms = *itr_nms;

      float iou = calc_intersection_over_union(region_candidate, region_nms);
      if (iou >= iou_thresh) {
        ignore_candidate = true;
        break;
      }
    }

    if (!ignore_candidate) {
      region_nms_list.push_back(region_candidate);
      if (region_nms_list.size() >= 5)
        break;
    }
  }
  return 0;
}

static float normalize_radians(float angle) {
  return angle - 2 * M_PI * floor((angle - (-M_PI)) / (2 * M_PI));
}

static void compute_rotation(DetectRegion &region) {
  float x0 = region.landmarks[0].x;
  float y0 = region.landmarks[0].y;
  float x1 = region.landmarks[2].x;
  float y1 = region.landmarks[2].y;

  float target_angle = M_PI * 0.5f;
  float rotation = target_angle - atan2(-(y1 - y0), x1 - x0);

  region.rotation = normalize_radians(rotation);
}

void rot_vec(ov::Point2f &vec, float rotation) {
  float sx = vec.x;
  float sy = vec.y;
  vec.x = sx * cos(rotation) - sy * sin(rotation);
  vec.y = sx * sin(rotation) + sy * cos(rotation);
}

void compute_detect_to_roi(DetectRegion &region, const int &target_size,
                           PalmObject &palm) {
  float width = region.btmright.x - region.topleft.x;
  float height = region.btmright.y - region.topleft.y;
  float palm_cx = region.topleft.x + width * 0.5f;
  float palm_cy = region.topleft.y + height * 0.5f;

  float hand_cx;
  float hand_cy;
  float rotation = region.rotation;
  float shift_x = 0.0f;
  float shift_y = -0.5f;

  if (rotation == 0.0f) {
    hand_cx = palm_cx + (width * shift_x);
    hand_cy = palm_cy + (height * shift_y);
  } else {
    float dx =
        (width * shift_x) * cos(rotation) - (height * shift_y) * sin(rotation);
    float dy =
        (width * shift_x) * sin(rotation) + (height * shift_y) * cos(rotation);
    hand_cx = palm_cx + dx;
    hand_cy = palm_cy + dy;
  }

  float long_side = std::max(width, height);
  width = long_side;
  height = long_side;
  float hand_w = width * 2.6f;
  float hand_h = height * 2.6f;

  palm.hand_cx = hand_cx;
  palm.hand_cy = hand_cy;
  palm.hand_w = hand_w;
  palm.hand_h = hand_h;

  float dx = hand_w * 0.5f;
  float dy = hand_h * 0.5f;

  palm.hand_pos[0].x = -dx;
  palm.hand_pos[0].y = -dy;
  palm.hand_pos[1].x = +dx;
  palm.hand_pos[1].y = -dy;
  palm.hand_pos[2].x = +dx;
  palm.hand_pos[2].y = +dy;
  palm.hand_pos[3].x = -dx;
  palm.hand_pos[3].y = +dy;

  for (int i = 0; i < 4; i++) {
    rot_vec(palm.hand_pos[i], rotation);
    palm.hand_pos[i].x += hand_cx;
    palm.hand_pos[i].y += hand_cy;
  }

  for (int i = 0; i < 7; i++) {
    palm.landmarks[i] = region.landmarks[i];
  }

  palm.rotation = rotation;

  palm.score = region.score;
}

static void pack_detect_result(std::vector<DetectRegion> &detect_results,
                               std::list<DetectRegion> &region_list,
                               const int &target_size,
                               std::vector<PalmObject> &palmlist) {
  for (auto &region : region_list) {
    compute_rotation(region);
    PalmObject palm;
    compute_detect_to_roi(region, target_size, palm);
    palmlist.push_back(palm);
    detect_results.push_back(region);
  }
}

MediapipeHand::MediapipeHand() : ov::EstimatorBase() {
  palm_blob_allocator_.set_size_compare_ratio(0.f);
  palm_workspace_allocator_.set_size_compare_ratio(0.f);
  hand_blob_allocator_.set_size_compare_ratio(0.f);
  hand_workspace_allocator_.set_size_compare_ratio(0.f);
  palm_net_ = new ncnn::Net();
  hand_net_ = new ncnn::Net();
  initialized_ = false;
  if (num_threads > 0) {
    palm_net_->opt.num_threads = num_threads;
    hand_net_->opt.num_threads = num_threads;
  }
  palm_net_->opt.blob_allocator = &palm_blob_allocator_;
  palm_net_->opt.workspace_allocator = &palm_workspace_allocator_;
  palm_net_->opt.lightmode = light_mode_;
  hand_net_->opt.blob_allocator = &hand_blob_allocator_;
  hand_net_->opt.workspace_allocator = &hand_workspace_allocator_;
  hand_net_->opt.lightmode = light_mode_;
#ifdef OV_VULKAN
  palm_net_->opt.use_vulkan_compute = true;
  hand_net_->opt.use_vulkan_compute = true;
#endif // OV_VULKAN
}

MediapipeHand::~MediapipeHand() {
  if (palm_net_) {
    palm_net_->clear();
  }
  if (hand_net_) {
    hand_net_->clear();
  }
  palm_workspace_allocator_.clear();
  palm_blob_allocator_.clear();
  hand_workspace_allocator_.clear();
  hand_blob_allocator_.clear();
}

void MediapipeHand::set_num_threads(int n) {
  EstimatorBase::set_num_threads(n);
  if (palm_net_) {
    palm_net_->opt.num_threads = n;
  }
  if (hand_net_) {
    hand_net_->opt.num_threads = n;
  }
}

void MediapipeHand::set_light_mode(bool mode) {
  if (palm_net_) {
    palm_net_->opt.lightmode = mode;
  }
  if (hand_net_) {
    hand_net_->opt.lightmode = mode;
  }
  light_mode_ = mode;
}

int MediapipeHand::LoadModel(const char *palm_path, const char *hand_path) {
  std::string palm_param_file = std::string(palm_path) + "/param";
  std::string palm_bin_file = std::string(palm_path) + "/bin";
  std::string hand_param_file = std::string(hand_path) + "/param";
  std::string hand_bin_file = std::string(hand_path) + "/bin";
  if (palm_net_->load_param(palm_param_file.c_str()) == -1 ||
      palm_net_->load_model(palm_bin_file.c_str()) == -1) {
    return 10000;
  }
  if (hand_net_->load_param(hand_param_file.c_str()) == -1 ||
      hand_net_->load_model(hand_bin_file.c_str()) == -1) {
    return 10000;
  }

  initialized_ = true;
  anchors.clear();
  create_ssd_anchors(target_size, target_size, anchors);

  return 0;
}

int MediapipeHand::Detect(const unsigned char *rgbdata, int img_width,
                          int img_height, std::vector<PalmObject> &objects) {
  if (!initialized_) {
    return 10000;
  }
  if (rgbdata == 0) {
    return 10001;
  }
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

  ncnn::Mat in = ncnn::Mat::from_pixels_resize(rgbdata, ncnn::Mat::PIXEL_RGB,
                                               img_width, img_height, w, h);

  int wpad = target_size - w;
  int hpad = target_size - h;
  ncnn::Mat in_pad;
  ncnn::copy_make_border(in, in_pad, hpad / 2, hpad - hpad / 2, wpad / 2,
                         wpad - wpad / 2, ncnn::BORDER_CONSTANT, 0.f);
  const float norm_vals[3] = {1 / 255.f, 1 / 255.f, 1 / 255.f};
  in_pad.substract_mean_normalize(0, norm_vals);

  ncnn::Extractor ex = palm_net_->create_extractor();
  ncnn::Mat cls, reg;
  ex.input("input", in_pad);
  ex.extract("cls", cls);
  ex.extract("reg", reg);

  float *scores = (float *)cls.data;
  float *bboxes = (float *)reg.data;

  std::list<DetectRegion> region_list, region_nms_list;
  std::vector<DetectRegion> detect_results;

  decode_bounds(region_list, prob_threshold, target_size, target_size, scores,
                bboxes, anchors);
  non_max_suppression(region_list, region_nms_list, nms_threshold);
  objects.clear();
  pack_detect_result(detect_results, region_nms_list, target_size, objects);

  for (int i = 0; i < objects.size(); i++) {
    objects[i].hand_pos[0].x =
        (objects[i].hand_pos[0].x * target_size - ((float)wpad / 2)) / scale;
    objects[i].hand_pos[0].y =
        (objects[i].hand_pos[0].y * target_size - ((float)hpad / 2)) / scale;
    objects[i].hand_pos[1].x =
        (objects[i].hand_pos[1].x * target_size - ((float)wpad / 2)) / scale;
    objects[i].hand_pos[1].y =
        (objects[i].hand_pos[1].y * target_size - ((float)hpad / 2)) / scale;
    objects[i].hand_pos[2].x =
        (objects[i].hand_pos[2].x * target_size - ((float)wpad / 2)) / scale;
    objects[i].hand_pos[2].y =
        (objects[i].hand_pos[2].y * target_size - ((float)hpad / 2)) / scale;
    objects[i].hand_pos[3].x =
        (objects[i].hand_pos[3].x * target_size - ((float)wpad / 2)) / scale;
    objects[i].hand_pos[3].y =
        (objects[i].hand_pos[3].y * target_size - ((float)hpad / 2)) / scale;

    for (int j = 0; j < 7; j++) {
      objects[i].landmarks[j].x =
          (objects[i].landmarks[j].x * target_size - ((float)wpad / 2)) / scale;
      objects[i].landmarks[j].y =
          (objects[i].landmarks[j].y * target_size - ((float)hpad / 2)) / scale;
    }

    const float srcPts[8] = {
        objects[i].hand_pos[0].x, objects[i].hand_pos[0].y,
        objects[i].hand_pos[1].x, objects[i].hand_pos[1].y,
        objects[i].hand_pos[2].x, objects[i].hand_pos[2].y,
        objects[i].hand_pos[3].x, objects[i].hand_pos[3].y,
    };

    const float dstPts[8] = {
        0, 0, 224, 0, 224, 224, 0, 224,
    };

    float tm[6];
    unsigned char *trans_mat =
        (unsigned char *)malloc(224 * 224 * 3 * sizeof(unsigned char));
    ncnn::get_affine_transform(dstPts, srcPts, 4, tm);

    ncnn::warpaffine_bilinear_c3(rgbdata, img_width, img_height, trans_mat, 224,
                                 224, tm);

    ncnn::Mat trans_image =
        ncnn::Mat::from_pixels(trans_mat, ncnn::Mat::PIXEL_RGB, 224, 224);

    float score = GetLandmarks(trans_image, tm, objects[i].skeleton,
                               objects[i].skeleton3d);

    free(trans_mat);
  }
  return 0;
}

float MediapipeHand::GetLandmarks(ncnn::Mat in, float tm[6],
                                  std::vector<ov::Point2f> &skeleton,
                                  std::vector<ov::Point3d> &skeleton3d) {

  const float norm_vals[3] = {1 / 255.f, 1 / 255.f, 1 / 255.f};
  in.substract_mean_normalize(NULL, norm_vals);
  ncnn::Mat points, score;
  {
    ncnn::Extractor ex = hand_net_->create_extractor();
    ex.input("input", in);
    ex.extract("points", points);
    ex.extract("score", score);
  }

  float *points_data = (float *)points.data;
  float *score_data = (float *)score.data;
  for (int i = 0; i < 21; i++) {
    ov::Point3d pt3d;
    pt3d.x = points_data[i * 3];
    pt3d.y = points_data[i * 3 + 1];
    pt3d.z = points_data[i * 3 + 2];

    ov::Point2f pt;
    pt.x = pt3d.x * tm[0] + pt3d.y * tm[1] + tm[2];
    pt.y = pt3d.x * tm[3] + pt3d.y * tm[4] + tm[5];

    skeleton.push_back(pt);

    pt3d.x /= 224.f;
    pt3d.y /= 224.f;
    skeleton3d.push_back(pt3d);
  }
  return score_data[0];
}

} // namespace ovhand3d
