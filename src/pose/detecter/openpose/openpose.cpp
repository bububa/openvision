#include "openpose.hpp"
#include "mat.h"

#ifdef OV_VULKAN
#include "gpu.h"
#endif // OV_VULKAN

namespace ovpose {

static const std::pair<int, int> limbIdsHeatmap[] = {
        {1, 2}, {1, 5}, {2, 3}, {3, 4}, {5, 6},
        {6, 7}, {1, 8}, {8, 9}, {9, 10}, {1, 11},
        {11, 12}, {12, 13}, {1, 0}, {0, 14}, {14, 16},
        {0, 15}, {15, 17}, {2, 16}, {5, 17}
};
static const std::pair<int, int> limbIdsPaf[] = {
        {12, 13}, {20, 21}, {14, 15}, {16, 17}, {22, 23},
        {24, 25}, {0, 1}, {2, 3}, {4, 5}, {6, 7},
        {8, 9}, {10, 11}, {28, 29}, {30, 31}, {34, 35},
        {32, 33}, {36, 37}, {18, 19}, {26, 27}
};

int OpenPose::Detect(const unsigned char* rgbdata,
    int img_width, int img_height,
    std::vector<ov::ObjectInfo>* rois) {
	if (!initialized_) {
		return 10000;
	}
	if (rgbdata == 0){
		return 10001;
	}
    int w = img_width;
    int h = img_height;
    int net_w = 456;
    int net_h = 456;
    float scale = 1.0f;
    if (w > h) {
        scale = (float) net_w / w;
        w = net_w;
        h = h * scale;
    } else {
        scale = (float) net_h / h;
        h = net_h;
        w = w * scale;
    }

    ncnn::Mat in = ncnn::Mat::from_pixels_resize(rgbdata, ncnn::Mat::PIXEL_RGB, img_width, img_height, w, h);
    in.substract_mean_normalize(mean_vals, norm_vals);

    // forward
    ncnn::Mat pafs;
    ncnn::Mat heatmaps;
    ncnn::Extractor ex = net_->create_extractor();
    ex.input("data", in);
    ex.extract("stage_1_output_1_heatmaps", heatmaps);  // or stage_0_output_1_heatmaps
    ex.extract("stage_1_output_0_pafs", pafs);          // or stage_0_output_0_pafs

    // postprocess
    this->postProcess(pafs, heatmaps, *rois, img_height, img_width, h, w);

    return 0;
}

void OpenPose::findPeaks(const std::vector<ov::Image> &heatMaps,
               const float minPeaksDistance,
               std::vector<std::vector<ov::Keypoint> > &allPeaks,
               int heatMapId) {
    const float threshold = 0.1f;
    std::vector<ov::Point2f> peaks;
    const ov::Image &heatMap = heatMaps[heatMapId];
    int heatMapStep = heatMap.channels * heatMap.width;
    for (int y = -1; y < heatMap.height + 1; y++) {
        for (int x = -1; x < heatMap.width + 1; x++) {
            float val = 0;
            if (x >= 0
                && y >= 0
                && x < heatMap.width
                && y < heatMap.height) {
                val = heatMap.data[y * heatMapStep + x];
                val = val >= threshold ? val : 0;
            }

            float left_val = 0;
            if (y >= 0
                && x < (heatMap.width- 1)
                && y < heatMap.height) {
                left_val = heatMap.data[y * heatMapStep + x + 1];
                left_val = left_val >= threshold ? left_val : 0;
            }

            float right_val = 0;
            if (x > 0
                && y >= 0
                && y < heatMap.height) {
                right_val = heatMap.data[y * heatMapStep + x - 1];
                right_val = right_val >= threshold ? right_val : 0;
            }

            float top_val = 0;
            if (x >= 0
                && x < heatMap.width
                && y < (heatMap.height- 1)) {
                top_val = heatMap.data[(y + 1) * heatMapStep + x];
                top_val = top_val >= threshold ? top_val : 0;
            }

            float bottom_val = 0;
            if (x >= 0
                && y > 0
                && x < heatMap.width) {
                bottom_val = heatMap.data[(y - 1) * heatMapStep + x];
                bottom_val = bottom_val >= threshold ? bottom_val : 0;
            }

            if ((val > left_val)
                && (val > right_val)
                && (val > top_val)
                && (val > bottom_val)) {
                peaks.push_back(ov::Point2f(x, y));
            }
        }
    }
    std::sort(peaks.begin(), peaks.end(), [](const ov::Point2f &a, const ov::Point2f &b) {
        return a.x < b.x;
    });
    std::vector<bool> isActualPeak(peaks.size(), true);
    int peakCounter = 0;
    std::vector<ov::Keypoint> &peaksWithScoreAndID = allPeaks[heatMapId];
    for (size_t i = 0; i < peaks.size(); i++) {
        if (isActualPeak[i]) {
            for (size_t j = i + 1; j < peaks.size(); j++) {
                if (sqrt((peaks[i].x - peaks[j].x) * (peaks[i].x - peaks[j].x) +
                         (peaks[i].y - peaks[j].y) * (peaks[i].y - peaks[j].y)) < minPeaksDistance) {
                    isActualPeak[j] = false;
                }
            }
            ov::Keypoint pt;
            pt.id = peakCounter++;
            pt.p = peaks[i];
            pt.score = heatMap.data[peaks[i].x + peaks[i].y * heatMapStep];
            peaksWithScoreAndID.push_back(pt);
        }
    }
}

void OpenPose::groupPeaksToPoses(const std::vector<std::vector<ov::Keypoint> > &allPeaks,
                                         const std::vector<ov::Image> &pafs,
                                         std::vector<ov::ObjectInfo>& poses,
                                         const size_t keypointsNumber,
                                         const float midPointsScoreThreshold,
                                         const float foundMidPointsRatioThreshold,
                                         const int minJointsNumber,
                                         const float minSubsetScore) {
    std::vector<ov::Keypoint> candidates;
    for (const auto &peaks : allPeaks) {
        candidates.insert(candidates.end(), peaks.begin(), peaks.end());
    }
    std::vector<HumanPoseByPeaksIndices> subset(0, HumanPoseByPeaksIndices(keypointsNumber));
    for (size_t k = 0; k < ov::arraySize(limbIdsPaf); k++) {
        std::vector<TwoJointsConnection> connections;
        const int mapIdxOffset = 0; // keypointsNumber + 1;
        std::pair<ov::Image, ov::Image> scoreMid = {pafs[limbIdsPaf[k].first - mapIdxOffset],
                                                pafs[limbIdsPaf[k].second - mapIdxOffset]};
        const int idxJointA = limbIdsHeatmap[k].first;  // first - 1;
        const int idxJointB = limbIdsHeatmap[k].second; // second - 1;
        const std::vector<ov::Keypoint> &candA = allPeaks[idxJointA];
        const std::vector<ov::Keypoint> &candB = allPeaks[idxJointB];
        const size_t nJointsA = candA.size();
        const size_t nJointsB = candB.size();
        if (nJointsA == 0 && nJointsB == 0) {
            continue;
        } else if (nJointsA == 0) {
            for (size_t i = 0; i < nJointsB; i++) {
                int num = 0;
                for (size_t j = 0; j < subset.size(); j++) {
                    if (subset[j].peaksIndices[idxJointB] == candB[i].id) {
                        num++;
                        continue;
                    }
                }
                if (num == 0) {
                    HumanPoseByPeaksIndices personKeypoints(keypointsNumber);
                    personKeypoints.peaksIndices[idxJointB] = candB[i].id;
                    personKeypoints.nJoints = 1;
                    personKeypoints.score = candB[i].score;
                    subset.push_back(personKeypoints);
                }
            }
            continue;
        } else if (nJointsB == 0) {
            for (size_t i = 0; i < nJointsA; i++) {
                int num = 0;
                for (size_t j = 0; j < subset.size(); j++) {
                    if (subset[j].peaksIndices[idxJointA] == candA[i].id) {
                        num++;
                        continue;
                    }
                }
                if (num == 0) {
                    HumanPoseByPeaksIndices personKeypoints(keypointsNumber);
                    personKeypoints.peaksIndices[idxJointA] = candA[i].id;
                    personKeypoints.nJoints = 1;
                    personKeypoints.score = candA[i].score;
                    subset.push_back(personKeypoints);
                }
            }
            continue;
        }

        std::vector<TwoJointsConnection> tempJointConnections;
        for (size_t i = 0; i < nJointsA; i++) {
            for (size_t j = 0; j < nJointsB; j++) {
                ov::Point2f pt = candA[i].p * 0.5 + candB[j].p * 0.5;
                ov::Point mid = ov::Point(ov::cvRound(pt.x), ov::cvRound(pt.y));
                ov::Point2f vec = candB[j].p - candA[i].p;
                double norm_vec = sqrt(vec.x * vec.x + vec.y * vec.y); 
                if (norm_vec == 0) {
                    continue;
                }
                vec = vec / norm_vec;
                float score = vec.x * scoreMid.first.at(mid) + vec.y * scoreMid.second.at(mid);
                int height_n = pafs[0].height/ 2;
                float suc_ratio = 0.0f;
                float mid_score = 0.0f;
                const int mid_num = 10;
                const float scoreThreshold = -100.0f;
                if (score > scoreThreshold) {
                    float p_sum = 0;
                    float p_count = 0;
                    ov::Size2f step((candB[j].p.x - candA[i].p.x) / (mid_num - 1),
                                    (candB[j].p.y - candA[i].p.y) / (mid_num - 1));
                    for (int n = 0; n < mid_num; n++) {
                        ov::Point midPoint(ov::cvRound(candA[i].p.x + n * step.width),
                                           ov::cvRound(candA[i].p.y + n * step.height));
                        ov::Point2f pred(scoreMid.first.at(midPoint),
                                         scoreMid.second.at(midPoint));
                        score = vec.x * pred.x + vec.y * pred.y;
                        if (score > midPointsScoreThreshold) {
                            p_sum += score;
                            p_count++;
                        }
                    }
                    suc_ratio = static_cast<float>(p_count / mid_num);
                    float ratio = p_count > 0 ? p_sum / p_count : 0.0f;
                    mid_score = ratio + static_cast<float>(std::min(height_n / norm_vec - 1, 0.0));
                }
                if (mid_score > 0
                    && suc_ratio > foundMidPointsRatioThreshold) {
                    tempJointConnections.push_back(TwoJointsConnection(i, j, mid_score));
                }
            }
        }
        if (!tempJointConnections.empty()) {
            std::sort(tempJointConnections.begin(), tempJointConnections.end(),
                      [](const TwoJointsConnection &a,
                         const TwoJointsConnection &b) {
                          return (a.score > b.score);
                      });
        }
        size_t num_limbs = std::min(nJointsA, nJointsB);
        size_t cnt = 0;
        std::vector<int> occurA(nJointsA, 0);
        std::vector<int> occurB(nJointsB, 0);
        for (size_t row = 0; row < tempJointConnections.size(); row++) {
            if (cnt == num_limbs) {
                break;
            }
            const int &indexA = tempJointConnections[row].firstJointIdx;
            const int &indexB = tempJointConnections[row].secondJointIdx;
            const float &score = tempJointConnections[row].score;
            if (occurA[indexA] == 0
                && occurB[indexB] == 0) {
                connections.push_back(TwoJointsConnection(candA[indexA].id, candB[indexB].id, score));
                cnt++;
                occurA[indexA] = 1;
                occurB[indexB] = 1;
            }
        }
        if (connections.empty()) {
            continue;
        }

        bool extraJointConnections = (k == 17 || k == 18);
        if (k == 0) {
            subset = std::vector<HumanPoseByPeaksIndices>(
                    connections.size(), HumanPoseByPeaksIndices(keypointsNumber));
            for (size_t i = 0; i < connections.size(); i++) {
                const int &indexA = connections[i].firstJointIdx;
                const int &indexB = connections[i].secondJointIdx;
                subset[i].peaksIndices[idxJointA] = indexA;
                subset[i].peaksIndices[idxJointB] = indexB;
                subset[i].nJoints = 2;
                subset[i].score = candidates[indexA].score + candidates[indexB].score + connections[i].score;
            }
        } else if (extraJointConnections) {
            for (size_t i = 0; i < connections.size(); i++) {
                const int &indexA = connections[i].firstJointIdx;
                const int &indexB = connections[i].secondJointIdx;
                for (size_t j = 0; j < subset.size(); j++) {
                    if (subset[j].peaksIndices[idxJointA] == indexA
                        && subset[j].peaksIndices[idxJointB] == -1) {
                        subset[j].peaksIndices[idxJointB] = indexB;
                    } else if (subset[j].peaksIndices[idxJointB] == indexB
                               && subset[j].peaksIndices[idxJointA] == -1) {
                        subset[j].peaksIndices[idxJointA] = indexA;
                    }
                }
            }
            continue;
        } else {
            for (size_t i = 0; i < connections.size(); i++) {
                const int &indexA = connections[i].firstJointIdx;
                const int &indexB = connections[i].secondJointIdx;
                bool num = false;
                for (size_t j = 0; j < subset.size(); j++) {
                    if (subset[j].peaksIndices[idxJointA] == indexA) {
                        subset[j].peaksIndices[idxJointB] = indexB;
                        subset[j].nJoints++;
                        subset[j].score += candidates[indexB].score + connections[i].score;
                        num = true;
                    }
                }
                if (!num) {
                    HumanPoseByPeaksIndices hpWithScore(keypointsNumber);
                    hpWithScore.peaksIndices[idxJointA] = indexA;
                    hpWithScore.peaksIndices[idxJointB] = indexB;
                    hpWithScore.nJoints = 2;
                    hpWithScore.score = candidates[indexA].score + candidates[indexB].score + connections[i].score;
                    subset.push_back(hpWithScore);
                }
            }
        }
    }
    for (const auto &subsetI : subset) {
        if (subsetI.nJoints < minJointsNumber || subsetI.score / subsetI.nJoints < minSubsetScore) {
            continue;
        }
        int position = -1;
        ov::ObjectInfo pose;
        pose.pts = std::vector<ov::Keypoint>(
                keypointsNumber, 
                ov::Keypoint(ov::Point2f(-1.0f, -1.0f))
            );
        pose.score = subsetI.score * std::max(0, subsetI.nJoints - 1);
        for (const auto &peakIdx : subsetI.peaksIndices) {
            position++;
            if (peakIdx >= 0) {
                pose.pts[position] = candidates[peakIdx];
                pose.pts[position].p.x += 0.5;
                pose.pts[position].p.y += 0.5;
            }
        }
        poses.push_back(pose);
    }
}

void OpenPose::postProcess(const ncnn::Mat &pafs, const ncnn::Mat &heatmaps, 
        std::vector<ov::ObjectInfo>& poses, 
        int img_h, int img_w, 
        int net_h, int net_w) {

    float upsample_ratio = 4;
    // heatmaps
    std::vector<ov::Image> cv_heatmaps_upsample(heatmaps.c);
    for (int p = 0; p < heatmaps.c; p++) {
        ncnn::Mat resizedMat;
        int w = heatmaps.channel(p).w * upsample_ratio;
        int h = heatmaps.channel(p).h * upsample_ratio;
        ncnn::resize_bicubic(heatmaps.channel(p), resizedMat, w, h);
        cv_heatmaps_upsample[p] = ov::Image(resizedMat);
    }

    // pafs
    std::vector<ov::Image> cv_pafs_upsample(pafs.c);
    for (int p = 0; p < pafs.c; p++) {
        ncnn::Mat resizedMat;
        int w = pafs.channel(p).w * upsample_ratio;
        int h = pafs.channel(p).h * upsample_ratio;
        ncnn::resize_bicubic(pafs.channel(p), resizedMat, w, h);
        cv_pafs_upsample[p] = ov::Image(resizedMat);
    }

    // postprocess

    std::vector<std::vector<ov::Keypoint> > peaksFromHeatMap(cv_heatmaps_upsample.size());

#if defined(_OPENMP)
#pragma omp parallel for num_threads(num_threads)
#endif
    for (int i = 0; i < cv_heatmaps_upsample.size(); i++) {
        this->findPeaks(cv_heatmaps_upsample, minPeaksDistance, peaksFromHeatMap, i);
    }

    int peaksBefore = 0;
    for (size_t heatmapId = 1; heatmapId < cv_heatmaps_upsample.size(); heatmapId++) {
        peaksBefore += static_cast<int>(peaksFromHeatMap[heatmapId - 1].size());
        for (auto &peak : peaksFromHeatMap[heatmapId]) {
            peak.id += peaksBefore;
        }
    }
    groupPeaksToPoses(
            peaksFromHeatMap, cv_pafs_upsample, poses, keypointsNumber, midPointsScoreThreshold,
            foundMidPointsRatioThreshold, minJointsNumber, minSubsetScore);


    // scale keypoint
    float scale_x = 1.0f * net_w / img_w;
    float scale_y = 1.0f * net_h / img_h;
    float stride = 8.0f;
    float upsample = upsample_ratio;

    for (int i = 0; i < poses.size(); i++) {
        ov::ObjectInfo pose = poses[i];
        for (int j = 0; j < keypointsNumber; j++) {
            if (pose.pts[j].p.x == -1 || pose.pts[j].p.y == -1) {
                continue;
            }
            poses[i].pts[j].p.x = stride / upsample * pose.pts[j].p.x / scale_x;
            poses[i].pts[j].p.y = stride / upsample * pose.pts[j].p.y / scale_y;
        }
    }
}

}
