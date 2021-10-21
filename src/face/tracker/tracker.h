#ifndef _FACE_TRACKER_C_H_
#define _FACE_TRACKER_C_H_
#include "../common/common.h"

#ifdef __cplusplus
#include "tracker.hpp"
extern "C" {
#endif
    typedef void* ITracker;
    ITracker new_tracker();
    void destroy_tracker(ITracker t);
    int track(ITracker t, const FaceInfoVector* curr_faces, TrackedFaceInfoVector* faces); 
#ifdef __cplusplus
}
#endif
#endif // !_FACE_TRACKER_C_H_
