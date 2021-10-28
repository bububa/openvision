#include "yolov5focus.hpp"
#include "net.h"

#ifndef _YOLOV5FOCUS_LAYER_
#define _YOLOV5FOCUS_LAYER_
DEFINE_LAYER_CREATOR(YoloV5Focus)
#endif
// YOLOX use the same focus in yolov5
YoloV5Focus::YoloV5Focus()
{
    one_blob_only = true;
}

int YoloV5Focus::forward(const ncnn::Mat& bottom_blob, ncnn::Mat& top_blob, const ncnn::Option& opt) const
{
    int w = bottom_blob.w;
    int h = bottom_blob.h;
    int channels = bottom_blob.c;

    int outw = w / 2;
    int outh = h / 2;
    int outc = channels * 4;

    top_blob.create(outw, outh, outc, 4u, 1, opt.blob_allocator);
    if (top_blob.empty())
        return -100;

    #pragma omp parallel for num_threads(opt.num_threads)
    for (int p = 0; p < outc; p++)
    {
        const float* ptr = bottom_blob.channel(p % channels).row((p / channels) % 2) + ((p / channels) / 2);
        float* outptr = top_blob.channel(p);

        for (int i = 0; i < outh; i++)
        {
            for (int j = 0; j < outw; j++)
            {
                *outptr = *ptr;

                outptr += 1;
                ptr += 2;
            }

            ptr += w;
        }
    }

    return 0;
}

void register_yolov5focus(ncnn::Net* net) {
    net->register_custom_layer("YoloV5Focus", YoloV5Focus_layer_creator);
}
