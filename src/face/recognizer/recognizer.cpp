#include "../recognizer.h"
#include "./mobilefacenet/mobilefacenet.hpp"

IFaceRecognizer new_mobilefacenet() {
    return new ovface::Mobilefacenet();
}

int extract_feature(IFaceRecognizer r, const unsigned char* rgbdata, int img_width, int img_height, const Rect* face, FloatVector* feature) {
    std::vector<float>features;
    int ret = static_cast<ovface::Recognizer*>(r)->ExtractFeature(rgbdata, img_width, img_height, *face, &features);
    if (ret != 0) {
        return ret;
    }
    feature->length = features.size();
    feature->values = (float*)malloc(feature->length * sizeof(float));
    for (size_t i = 0; i < feature->length; ++i) {
        feature->values[i] = features.at(i);
    }
    return 0;
}

namespace ovface {
Recognizer* MobilefacenetRecognizerFactory::CreateRecognizer() {
	return new Mobilefacenet();
}

}
