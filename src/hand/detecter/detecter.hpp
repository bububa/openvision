#ifndef _HAND_DETECTER_H_
#define _HAND_DETECTER_H_

#include "../common/common.hpp"
namespace ovhand {
class Detecter: public ov::Estimator {
public:
    virtual ~Detecter() {};
    virtual int Detect(const unsigned char*rgbdata,
        int img_width, int img_height,
        std::vector<HandROI>* rois) = 0;
};

class DetecterFactory {
public:
    virtual Detecter* CreateDetecter() = 0;
    virtual ~DetecterFactory() {};
};

class YoloxFactory: public DetecterFactory {
public:
    YoloxFactory() {}
    ~YoloxFactory() {}
    Detecter* CreateDetecter();
};

class NanodetFactory: public DetecterFactory {
public:
    NanodetFactory() {}
    ~NanodetFactory() {}
    Detecter* CreateDetecter();
};
}
#endif // !_HAND_DETECTER_H_
