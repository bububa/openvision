#include "tracker.h"
#include <queue>

ITracker new_tracker() {
    return new mirror::Tracker();
}

void destroy_tracker(ITracker t) {
    delete static_cast<mirror::Tracker*>(t);
}

int track(ITracker t, const FaceInfoVector* curr_faces, TrackedFaceInfoVector* faces) {
    std::vector<FaceInfo> cfaces;    
    for (int i = 0; i < curr_faces->length; ++i) {
        cfaces.push_back(static_cast<FaceInfo>(curr_faces->faces[i]));
    }
    std::vector<TrackedFaceInfo> tfaces;
    int ret = static_cast<mirror::Tracker*>(t)->Track(cfaces, &tfaces);
    if (ret != 0) {
        return ret;
    }
    faces->length = tfaces.size();
    faces->faces = (TrackedFaceInfo*)malloc(faces->length * sizeof(TrackedFaceInfo));
    for (size_t i = 0; i < faces->length; ++i) {
        faces->faces[i] = tfaces.at(i);
    }
    return 0;
}

namespace mirror {
Tracker::Tracker() {

}

Tracker::~Tracker() {

}

int Tracker::Track(const std::vector<FaceInfo>& curr_faces, std::vector<TrackedFaceInfo>* faces) {
    faces->clear();
    int num_faces = static_cast<int>(curr_faces.size());

    std::deque<TrackedFaceInfo>scored_tracked_faces(pre_tracked_faces_.begin(), pre_tracked_faces_.end());
    std::vector<TrackedFaceInfo> curr_tracked_faces;
    for (int i = 0; i < num_faces; ++i) {
        auto& face = curr_faces.at(i);
        for (auto scored_tracked_face : scored_tracked_faces) {
            ComputeIOU(scored_tracked_face.face_info_.location_,
                face.location_, &scored_tracked_face.iou_score_);
        }
        if (scored_tracked_faces.size() > 0) {
            std::partial_sort(scored_tracked_faces.begin(),
                scored_tracked_faces.begin() + 1,
				scored_tracked_faces.end(),
				[](const TrackedFaceInfo &a, const TrackedFaceInfo &b) {
				return a.iou_score_ > b.iou_score_;
			});
        }
        if (!scored_tracked_faces.empty() && scored_tracked_faces.front().iou_score_ > minScore_) {
			TrackedFaceInfo matched_face = scored_tracked_faces.front();
			scored_tracked_faces.pop_front();
			TrackedFaceInfo &tracked_face = matched_face;
			if (matched_face.iou_score_ < maxScore_) {
				tracked_face.face_info_.location_.x = (tracked_face.face_info_.location_.x + face.location_.x) / 2;
				tracked_face.face_info_.location_.y = (tracked_face.face_info_.location_.y + face.location_.y) / 2;
				tracked_face.face_info_.location_.width = (tracked_face.face_info_.location_.width + face.location_.width) / 2;
				tracked_face.face_info_.location_.height = (tracked_face.face_info_.location_.height + face.location_.height) / 2;
			} else {
				tracked_face.face_info_ = face;
			}
			curr_tracked_faces.push_back(tracked_face);
		} else {
			TrackedFaceInfo tracked_face;
			tracked_face.face_info_ = face;
			curr_tracked_faces.push_back(tracked_face);
		}
    }

    pre_tracked_faces_ = curr_tracked_faces;
    *faces = curr_tracked_faces;
    return 0;
}

}
