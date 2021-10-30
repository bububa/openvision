#ifndef _FACE_ANTICONV_H_
#define _FACE_ANTICONV_H_

#include "../detecter.hpp"
#include "net.h"

namespace ovface {
using ANCHORS = std::vector<ov::Rect>;
class AntiConv : public Detecter {
public:
	int LoadModel(const char* root_path);
	int DetectFace(const unsigned char* rgbdata, 
        int img_width, int img_height,
        std::vector<FaceInfo>* faces);

private:
	std::vector<ANCHORS> anchors_generated_;
	const int RPNs_[3] = { 32, 16, 8 };
	const Size inputSize_ = { 640, 640 };
	const float iouThreshold_ = 0.4f;
	const float scoreThreshold_ = 0.8f;
    const float maskThreshold_ = 0.2f;

};

}

#endif // !_FACE_ANTICONV_H_
