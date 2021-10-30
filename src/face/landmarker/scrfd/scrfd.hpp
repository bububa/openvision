#ifndef _FACE_SCRFD_LANDMARKER_H_
#define _FACE_SCRFD_LANDMARKER_H_

#include "../landmarker.hpp"
#include "net.h"

namespace ovface {
class ScrfdLandmarker : public Landmarker {
public:
	int ExtractKeypoints(const unsigned char* rgbdata, 
        int img_width, int img_height,
		const ov::Rect& face, std::vector<ov::Point2f>* keypoints);

private:
    const float means[3] = { 127.5f, 127.5f,  127.5f };
    const float norms[3] = { 1/127.5f, 1 / 127.5f, 1 / 127.5f };
};

}

#endif // !_FACE_SCRFD_ANDMARKER_H_

