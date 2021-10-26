#ifndef _FACE_INSIGHTFACE_LANDMARKER_H_
#define _FACE_INSIGHTFACE_LANDMARKER_H_

#include "../landmarker.hpp"
#include "net.h"

namespace ov{
class InsightfaceLandmarker : public Landmarker {
public:
	InsightfaceLandmarker();
	~InsightfaceLandmarker();

	int LoadModel(const char* root_path);
	int ExtractKeypoints(const unsigned char* rgbdata, 
        int img_width, int img_height,
		const Rect& face, std::vector<Point2f>* keypoints);

private:
	ncnn::Net* insightface_landmarker_net_;
	bool initialized;
};

}

#endif // !_FACE_INSIGHTFACE_LANDMARKER_H_

