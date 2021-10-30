#ifndef _POSE_DETECTER_H_
#define _POSE_DETECTER_H_

#include "../common/common.hpp"
#include <vector>
namespace ovpose {

class Detecter: public ov::Estimator {
public:
    virtual int ExtractROIs(const unsigned char* rgbadata,
        int img_width, int img_height,
        std::vector<ov::ObjectInfo>* rois) = 0;
    virtual int ExtractKeypoints(const unsigned char* rgbdata,
        int img_width, int img_height,
        const ov::Rect& rect, std::vector<ov::Keypoint>* keypoints) = 0;
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
