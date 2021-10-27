#ifndef _FACE_RECOGNIZER_C_H_
#define _FACE_RECOGNIZER_C_H_

#include "common.h"

#ifdef __cplusplus
#include "recognizer/recognizer.hpp"
extern "C" {
#endif
    typedef void* IRecognizer;
    IRecognizer new_mobilefacenet();
    int extract_feature(IRecognizer r, const unsigned char* rgbdata, int img_width, int img_height, const Rect* face, FloatVector* feature); 
#ifdef __cplusplus
}
#endif
#endif // !_FACE_RECOGNIZER_C_H_
