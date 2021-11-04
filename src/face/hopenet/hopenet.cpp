#include "../hopenet.h"

#include <iostream>
#include <numeric>

#ifdef OV_VULKAN
#include "gpu.h"
#endif // OV_VULKAN


IHopenet new_hopenet() {
    return new ovface::Hopenet();
}

int hopenet_detect(IHopenet h, const unsigned char* rgbdata, int img_width, int img_height, const Rect* roi, HeadPose* euler_angles) {
    return static_cast<ovface::Hopenet*>(h)->Detect(rgbdata, img_width, img_height, *roi, static_cast<ovface::HeadPose*>(euler_angles));
}

namespace ovface {

#define NEAR_0 1e-10
#define ODIM   66

int Hopenet::LoadModel(const char* root_path) {
    int ret = Estimator::LoadModel(root_path);
    if (ret != 0) {
        return ret;
    }
    for (uint i=1; i<67; i++) idx_tensor[i-1] = i;
    return 0;
}

int Hopenet::Detect(const unsigned char* rgbdata, 
    int img_width, int img_height,
    Rect roi, HeadPose* head_angles) {
    float diff = fabs(roi.height-roi.width);
    if (roi.height > roi.width) {
        roi.x -= diff/2;
        roi.width = roi.height;
    } else if (roi.height < roi.width) {
        roi.y -= diff/2;
        roi.height = roi.width;
    }

    size_t total_size = roi.width * roi.height * 3 * sizeof(unsigned char);
    unsigned char* img_face = (unsigned char*)malloc(total_size);
    const unsigned char *start_ptr = rgbdata;
#if defined(_OPENMP)
#pragma omp parallel for num_threads(num_threads)
#endif
    for(size_t i = 0; i < roi.height; ++i) {
        const unsigned char* srcCursor = start_ptr + ((i + roi.y) * img_width + roi.x) * 3; 
        unsigned char* dstCursor = img_face + i * roi.width * 3;
        memcpy(dstCursor, srcCursor, sizeof(unsigned char) * 3 * roi.width);
    }

    ncnn::Mat in = ncnn::Mat::from_pixels_resize(img_face, ncnn::Mat::PIXEL_RGB2GRAY, roi.width, roi.height, 48, 48);
    ncnn::Extractor ex = net_->create_extractor();
    ex.set_light_mode(light_mode_);
    ex.set_num_threads(num_threads);
    ex.input("data", in);

    ncnn::Mat output;
    ex.extract("hybridsequential0_multitask0_dense0_fwd", output);
    float* pred_pitch = output.range(0, ODIM);
    float* pred_roll  = output.range(ODIM, ODIM*2);
    float* pred_yaw   = output.range(ODIM*2, ODIM*3);

    this->softmax(pred_pitch, ODIM);
    this->softmax(pred_roll,  ODIM);
    this->softmax(pred_yaw,   ODIM);

    // printArray(pred_pitch, ODIM);

    head_angles->pitch = this->getAngle(pred_pitch, ODIM);
    head_angles->roll  = this->getAngle(pred_roll,  ODIM);
    head_angles->yaw   = this->getAngle(pred_yaw,   ODIM);

    free(img_face);

    return 0;
}

void Hopenet::softmax(float* z, size_t el) {
    double zmax = -INFINITY;
    double zsum = 0;
    for (size_t i = 0; i < el; i++) if (z[i] > zmax) zmax=z[i];
    for (size_t i=0; i<el; i++) z[i] = std::exp(z[i]-zmax);
    zsum = std::accumulate(z, z+el, 0.0);
    for (size_t i=0; i<el; i++) z[i] = (z[i]/zsum)+NEAR_0;
}

double Hopenet::getAngle(float* prediction, size_t len) {
    double expectation[len];
    for (uint i=0; i<len; i++) expectation[i]=idx_tensor[i]*prediction[i];
    double angle = std::accumulate(expectation, expectation+len, 0.0) * 3 - 99;
    return angle;
}

}
