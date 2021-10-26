#include "../recognizer.h"
#include "./mobilefacenet/mobilefacenet.hpp"

IRecognizer new_mobilefacenet() {
    return new ov::Mobilefacenet();
}

void destroy_recognizer(IRecognizer r) {
    delete static_cast<ov::Recognizer*>(r);
}

int recognizer_load_model(IRecognizer r, const char* root_path) {
    return static_cast<ov::Recognizer*>(r)->LoadModel(root_path); 
}

int extract_feature(IRecognizer r, const unsigned char* rgbdata, int img_width, int img_height, const Rect* face, FloatVector* feature) {
    std::vector<float>features;
    int ret = static_cast<ov::Recognizer*>(r)->ExtractFeature(rgbdata, img_width, img_height, *face, &features);
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

namespace ov {
Recognizer* MobilefacenetRecognizerFactory::CreateRecognizer() {
	return new Mobilefacenet();
}

}
