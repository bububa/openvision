#ifndef _POSE_SEGMENTOR_H_
#define _POSE_SEGMENTOR_H_

#include "../../common/common.h"

namespace ovpose {

class Segmentor: public ov::Estimator {
public:
    virtual int Matting(const unsigned char* rgbdata,
        int img_width, int img_height,
        Image* out) = 0;
    virtual int Merge(const unsigned char* rgbdata,
        int img_width, int img_height,
        const unsigned char* bgdata,
        int bg_width, int bg_height,
        Image* out) = 0;
};

class SegmentorFactory {
public:
    virtual Segmentor* CreateSegmentor() = 0;
    virtual ~SegmentorFactory() {};
};

class Deeplabv3plusFactory: public SegmentorFactory{
public:
    Deeplabv3plusFactory() {}
    ~Deeplabv3plusFactory() {}
    Segmentor* CreateSegmentor();
};

class ERDNetFactory: public SegmentorFactory{
public:
    ERDNetFactory() {}
    ~ERDNetFactory() {}
    Segmentor* CreateSegmentor();
};

}

#endif //!_POSE_SEGMENTOR_H_
