#ifndef _FACE_ALIGNER_C_H_
#define _FACE_ALIGNER_C_H_

#ifdef __cplusplus
#include "aligner/aligner.hpp"
extern "C" {
#endif
    typedef void* IFaceAligner;
    IFaceAligner new_face_aligner();
    void destroy_face_aligner(IFaceAligner d);
    void set_face_aligner_threads(IFaceAligner d, int n);
    int align_face(IFaceAligner d, const unsigned char* rgbdata, int img_width, int img_height, const Rect* rect, const float keypoints[10], Image* face_aligned); 
#ifdef __cplusplus
}
#endif
#endif // !_FACE_ALIGNER_C_H_
