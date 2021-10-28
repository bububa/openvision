#ifndef _POSE_DETECTER_H_
#define _POSE_DETECTER_H_

#include "../common/common.hpp"
#include <vector>
namespace ovpose {

class Detecter: public ov::Estimator {
public:
    virtual ~Detecter(){};
    virtual int ExtractROIs(const unsigned char* rgbadata,
        int img_width, int img_height,
        std::vector<PoseROI>* rois) = 0;
    virtual int ExtractKeypoints(const PoseROI& roi, std::vector<PoseKeypoint>* keypoints) = 0;
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
}
#endif // !_POSE_DETECTER_H
