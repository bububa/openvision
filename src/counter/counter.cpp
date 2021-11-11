#include "counter.h"
#include "p2pnet/p2pnet.hpp"

ICounter new_p2pnet_crowd_counter() { return new ovcounter::P2PNet(); }

int crowd_count(ICounter d, const unsigned char *rgbdata, int img_width,
                int img_height, KeypointVector *keypoints) {
  std::vector<ov::Keypoint> pts;
  int ret = static_cast<ovcounter::P2PNet *>(d)->CrowdCount(rgbdata, img_width,
                                                            img_height, &pts);
  if (ret != 0) {
    return ret;
  }
  keypoints->length = pts.size();
  keypoints->points =
      (ov::Keypoint *)malloc(keypoints->length * sizeof(ov::Keypoint));
  for (size_t i = 0; i < keypoints->length; ++i) {
    keypoints->points[i] = pts.at(i);
  }
  return 0;
}
