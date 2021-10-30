#ifndef _FACE_INSIGHTFACE_LANDMARKER_H_
#define _FACE_INSIGHTFACE_LANDMARKER_H_

#include "../landmarker.hpp"
#include "net.h"

namespace ovface {
class InsightfaceLandmarker : public Landmarker {
public:
	int ExtractKeypoints(const unsigned char* rgbdata, 
        int img_width, int img_height,
		const ov::Rect& face, std::vector<ov::Point2f>* keypoints);

};

}

#endif // !_FACE_INSIGHTFACE_LANDMARKER_H_

