#ifndef _FACE_MOBILEFACENET_H_
#define _FACE_MOBILEFACENET_H_

#include "../recognizer.hpp"
#include <vector>
#include "net.h"

namespace ovface {

class Mobilefacenet : public Recognizer {
public:
	int ExtractFeature(const unsigned char* rgbdata, 
        int img_width, int img_height,
        const ov::Rect& face,
        std::vector<float>* feature);

};

}

#endif // !_FACE_MOBILEFACENET_H_

