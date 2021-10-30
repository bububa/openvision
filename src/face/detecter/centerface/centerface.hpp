#ifndef _FACE_CENTERFACE_H_
#define _FACE_CENTERFACE_H_

#include "../detecter.hpp"
#include <vector>
#include "net.h"

namespace ovface {
class CenterFace : public Detecter {
public:
	int DetectFace(const unsigned char* rgbdata,
        int img_width, int img_height,
        std::vector<FaceInfo>* faces);

private:
    const float scoreThreshold_ = 0.5f;
    const float nmsThreshold_ = 0.5f;
};

}

#endif // !_FACE_CENTERFACE_H_
