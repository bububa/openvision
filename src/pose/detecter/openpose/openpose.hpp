#ifndef _POSE_OPENPOSE_H_
#define _POSE_OPENPOSE_H_ 

#include "../detecter.hpp"
#include <vector>
#include "net.h"

namespace ovpose {

struct HumanPoseByPeaksIndices {
    std::vector<int> peaksIndices;
    int nJoints;
    float score;
    explicit HumanPoseByPeaksIndices(const int keypointsNumber) : peaksIndices(std::vector<int>(keypointsNumber, -1)),
              nJoints(0),
              score(0.0f) {};
};

struct TwoJointsConnection {
    int firstJointIdx;
    int secondJointIdx;
    float score;
    TwoJointsConnection(const int firstJointIdx_,
                        const int secondJointIdx_,
                        const float score_): firstJointIdx(firstJointIdx_), secondJointIdx(secondJointIdx_), score(score_) {};
};

class OpenPose : public Detecter {
public:
    int Detect(const unsigned char* rgbadata,
        int img_width, int img_height,
        std::vector<ov::ObjectInfo>* rois);

private:
    const float mean_vals[3] = {127.5f, 127.5f, 127.5f};
    const float norm_vals[3] = {1.0f / 255.0f, 1.0f / 255.0f, 1.0f / 255.0f};
    const float minPeaksDistance = 3.0f;
    const int keypointsNumber = 18;
    const float midPointsScoreThreshold = 0.05f;
    const float foundMidPointsRatioThreshold = 0.8f;
    const int minJointsNumber = 3;
    const float minSubsetScore = 0.2f;

    void postProcess(const ncnn::Mat &pafs, const ncnn::Mat &heatmaps, 
            std::vector<ov::ObjectInfo>& poses, 
            int img_h, int img_w, 
            int net_h, int net_w);
    void findPeaks(const std::vector<ov::Image> &heatMaps, 
            const float minPeaksDistance,
            std::vector<std::vector<ov::Keypoint> > &allPeaks,
            int heatMapId);
    void groupPeaksToPoses(const std::vector<std::vector<ov::Keypoint> > &allPeaks,
            const std::vector<ov::Image> &pafs,
            std::vector<ov::ObjectInfo> &poses,
            const size_t keypointsNumber,
            const float midPointsScoreThreshold,
            const float foundMidPointsRatioThreshold,
            const int minJointsNumber,
            const float minSubsetScore); 
};

}

#endif // !_POSE_OPENPOSE_H_

