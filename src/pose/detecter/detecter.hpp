#ifndef _POSE_DETECTER_H_
#define _POSE_DETECTER_H_

#include "../common/common.hpp"
#include <vector>
namespace ov {

class Detecter {
public:
    virtual ~Detecter(){};
    virtual int LoadModel(const char* root_path) = 0;
    virtual int ExtractROIs(const unsigned char* rgbadata,
        int img_width, int img_height,
        std::vector<ROI>* rois) = 0;
    virtual int ExtractKeypoints(const ROI& roi, std::vector<Keypoint>* keypoints) = 0;
};

class DetecterFactory {
public:
    virtual Detecter* CreateDetecter() = 0;
    virtual ~DetecterFactory() {};
};

class UtralightFactory: public DetecterFactory {
public:
    UtralightFactory() {}
    ~UtralightFactory() {}
    Detecter* CreateDetecter();
};
}
#endif // !_POSE_DETECTER_H
