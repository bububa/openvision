#ifndef _FACE_LANDMARKER_H_
#define _FACE_LANDMARKER_H_

#include "../common/common.hpp"

namespace ovface {
// 抽象类
class Landmarker: public ov::Estimator {
public:
	virtual ~Landmarker() {};
	virtual int ExtractKeypoints(const unsigned char* rgbdata, 
        int img_width, int img_height,
		const Rect& face, std::vector<Point2f>* keypoints) = 0;
};

// 工厂基类
class LandmarkerFactory {
public:
	virtual Landmarker* CreateLandmarker() = 0;
	virtual ~LandmarkerFactory() {}
};

// 不同landmark检测器工厂
class ZQLandmarkerFactory : public LandmarkerFactory {
public:
	ZQLandmarkerFactory(){}
	Landmarker* CreateLandmarker();
	~ZQLandmarkerFactory() {}
};

class InsightfaceLandmarkerFactory : public LandmarkerFactory {
public:
	InsightfaceLandmarkerFactory(){}
	Landmarker* CreateLandmarker();
	~InsightfaceLandmarkerFactory() {}
};

}

#endif // !_FACE_LANDMARKER_H_

