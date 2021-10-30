#include "ultralight.hpp"
#include <string>

#ifdef OV_VULKAN
#include "gpu.h"
#endif // OV_VULKAN

namespace ovpose {

int Ultralight::Detect(const unsigned char* rgbdata,
    int img_width, int img_height,
    std::vector<ov::ObjectInfo>* rois) {
	if (!initialized_) {
		return 10000;
	}
	if (rgbdata == 0){
		return 10001;
	}
	ncnn::Mat in = ncnn::Mat::from_pixels_resize(rgbdata,
		ncnn::Mat::PIXEL_RGB, img_width, img_height, 320, 320);
    //数据预处理
    in.substract_mean_normalize(mean_vals, norm_vals);

    ncnn::Extractor ex = net_->create_extractor();
    ex.input("data", in);
    ncnn::Mat out;
    ex.extract("output", out);

    for (int i = 0; i < out.h; i++)
    {
        float x1, y1, x2, y2, score, label;
        float pw,ph,cx,cy;
        const float* values = out.row(i);
        
        x1 = values[2] * img_width;
        y1 = values[3] * img_height;
        x2 = values[4] * img_width;
        y2 = values[5] * img_height;

        pw = x2-x1;
        ph = y2-y1;
        cx = x1+0.5*pw;
        cy = y1+0.5*ph;

        x1 = cx - 0.7*pw;
        y1 = cy - 0.6*ph;
        x2 = cx + 0.7*pw;
        y2 = cy + 0.6*ph;

        score = values[1];
        label = values[0];

        //处理坐标越界问题
        if(x1<0) x1=0;
        if(y1<0) y1=0;
        if(x2<0) x2=0;
        if(y2<0) y2=0;

        if(x1>img_width) x1=img_width;
        if(y1>img_height) y1=img_height;
        if(x2>img_width) x2=img_width;
        if(y2>img_height) y2=img_height;
        //截取人体ROI
        //printf("x1:%f y1:%f x2:%f y2:%f\n",x1,y1,x2,y2);
        ov::Rect rect = ov::Rect(x1, y1, x2-x1, y2-y1);
        ov::ObjectInfo roi;
        roi.rect = rect;
        roi.score = score;
        rois->push_back(roi);
    }
    return 0;
}

}
