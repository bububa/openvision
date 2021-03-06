#ifndef _RETINAFACE_H_
#define _RETINAFACE_H_

#include "../detecter.hpp"
#include "net.h"

namespace ovface {
using ANCHORS = std::vector<ov::Rect>;
class RetinaFace : public Detecter {
public:
	int LoadModel(const char* root_path);
	int DetectFace(const unsigned char* rgbdata,
        int img_width, int img_height,
        std::vector<FaceInfo>* faces);

private:
	std::vector<ANCHORS> anchors_generated_;
	const int RPNs_[3] = { 32, 16, 8 };
	const Size inputSize_ = { 300, 300 };
	const float iouThreshold_ = 0.4f;
	const float scoreThreshold_ = 0.8f;

};

}

#endif // !_RETINAFACE_H_

