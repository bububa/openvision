#ifndef _FACE_MOBILEFACENET_H_
#define _FACE_MOBILEFACENET_H_

#include "../recognizer.hpp"
#include <vector>
#include "net.h"

namespace ov {

class Mobilefacenet : public Recognizer {
public:
	Mobilefacenet();
	~Mobilefacenet();

	int LoadModel(const char* root_path);
	int ExtractFeature(const unsigned char* rgbdata, 
        int img_width, int img_height,
        const Rect& face,
        std::vector<float>* feature);

private:
	ncnn::Net* mobileface_net_;
	bool initialized_;
};

}

#endif // !_FACE_MOBILEFACENET_H_

