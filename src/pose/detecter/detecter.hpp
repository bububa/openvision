#ifndef _POSE_DETECTER_H_
#define _POSE_DETECTER_H_

#include "../common/common.hpp"
#include <vector>
namespace ovpose {

class Detecter: public ov::Estimator {
public:
    virtual int Detect(const unsigned char* rgbadata,
        int img_width, int img_height,
        std::vector<ov::ObjectInfo>* rois) = 0;
};

class DetecterFactory {
public:
    virtual Detecter* CreateDetecter() = 0;
    virtual ~DetecterFactory() {};
};

class UltralightFactory: public DetecterFactory {
public:
    UltralightFactory() {}
    ~UltralightFactory() {}
    Detecter* CreateDetecter();
};

class OpenPoseFactory: public DetecterFactory {
public:
    OpenPoseFactory() {}
    ~OpenPoseFactory() {}
    Detecter* CreateDetecter();
};
}
#endif // !_POSE_DETECTER_H
