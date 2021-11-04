#ifndef _FACE_TRACKER_C_H_
#define _FACE_TRACKER_C_H_
#include "common.h"

#ifdef __cplusplus
#include "tracker/tracker.hpp"
extern "C" {
#endif
    typedef void* IFaceTracker;
    IFaceTracker new_face_tracker();
    void destroy_face_tracker(IFaceTracker t);
    int track_face(IFaceTracker t, const FaceInfoVector* curr_faces, TrackedFaceInfoVector* faces); 
#ifdef __cplusplus
}
#endif
#endif // !_FACE_TRACKER_C_H_
