#ifndef _COMMON_YOLOV5FOCUS_H_
#define _COMMON_YOLOV5FOCUS_H_

#include "layer.h"

class YoloV5Focus : public ncnn::Layer
{
public:
    YoloV5Focus();
    int forward(const ncnn::Mat& bottom_blob, ncnn::Mat& top_blob, const ncnn::Option& opt) const;
};

void register_yolov5focus(ncnn::Net* net);
#endif // !_COMMON_YOLOV5FOCUS_H_
