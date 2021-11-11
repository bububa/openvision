#include "tracker.h"
#include "lighttrack/lighttrack.hpp"

ITracker new_light_track() { return new ovtracker::LightTrack(); }

int track_object(ITracker d, const unsigned char *rgbdata, int img_width,
                 int img_height, Rect *rect) {
  int ret = static_cast<ovtracker::Tracker *>(d)->Track(rgbdata, img_width,
                                                        img_height, rect);
  if (ret != 0) {
    return ret;
  }
  return 0;
}
