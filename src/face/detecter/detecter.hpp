#ifndef _FACE_DETECTER_H_
#define _FACE_DETECTER_H_

#include "../common/common.hpp"

namespace ovface {
// 抽象类
class Detecter: public ov::Estimator {
public:
	virtual ~Detecter() {};
	virtual int DetectFace(const unsigned char* rgbdata, 
        int img_width, int img_height,
        std::vector<FaceInfo>* faces) = 0;

};

// 工厂基类
class DetecterFactory {
public:
	virtual Detecter* CreateDetecter() = 0;
	virtual ~DetecterFactory() {}
};

// 不同人脸检测器
class CenterfaceFactory : public DetecterFactory {
public:
	CenterfaceFactory() {}
	~CenterfaceFactory() {}
	Detecter* CreateDetecter();
};

class MtcnnFactory : public DetecterFactory {
public:
	MtcnnFactory() {}
	~MtcnnFactory() {}
	Detecter* CreateDetecter();

};

class RetinafaceFactory : public DetecterFactory {
public:
	RetinafaceFactory() {}
	~RetinafaceFactory() {}
	Detecter* CreateDetecter();
};

class AnticonvFactory : public DetecterFactory {
public:
	AnticonvFactory() {}
	~AnticonvFactory() {}
	Detecter* CreateDetecter();
};

}

#endif // !_FACE_DETECTER_H_

