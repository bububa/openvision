#ifndef _FACE_RECOGNIZER_H_
#define _FACE_RECOGNIZER_H_

#include <vector>
#include "../common/common.hpp"

namespace ov {
class Recognizer: public Estimator {
public:
	virtual ~Recognizer() {};
	virtual int ExtractFeature(const unsigned char* rgbdata, 
        int img_width, int img_height,
        const Rect& face,
        std::vector<float>* feature) = 0;

};

class RecognizerFactory {
public:
	virtual Recognizer* CreateRecognizer() = 0;
	virtual ~RecognizerFactory() {}

};

class MobilefacenetRecognizerFactory : public RecognizerFactory {
public:
	MobilefacenetRecognizerFactory() {};
	Recognizer* CreateRecognizer();
	~MobilefacenetRecognizerFactory() {}
};	

}

#endif // !_FACE_RECOGNIZER_H_

