#ifndef _FACE_ZQLANDMARKER_H_
#define _FACE_ZQLANDMARKER_H_

#include "../landmarker.hpp"
#include "net.h"

namespace ovface {
class ZQLandmarker : public Landmarker {
public:
	int ExtractKeypoints(const unsigned char* rgbdata, 
        int img_width, int img_height,
		const ov::Rect& face, std::vector<ov::Point2f>* keypoints);

private:
	const float meanVals[3] = { 127.5f, 127.5f, 127.5f };
	const float normVals[3] = { 0.0078125f, 0.0078125f, 0.0078125f };
};

}

#endif // !_FACE_ZQLANDMARKER_H_

