#include "../hopenet.h"

#include <iostream>
#include <numeric>

#ifdef OV_VULKAN
#include "gpu.h"
#endif // OV_VULKAN

#define NEAR_0 1e-10
#define ODIM   66

IHopeNet new_hopenet() {
    return new ov::HopeNet();
}

void destroy_hopenet(IHopeNet h) {
    delete static_cast<ov::HopeNet*>(h);
}

int hopenet_load_model(IHopeNet h, const char* root_path) {
    return static_cast<ov::HopeNet*>(h)->LoadModel(root_path);
}

int hopenet_detect(IHopeNet h, const unsigned char* rgbdata, int img_width, int img_height, const Rect* roi, HeadPose* euler_angles) {
    return static_cast<ov::HopeNet*>(h)->Detect(rgbdata, img_width, img_height, *roi, static_cast<ov::HeadPose*>(euler_angles));
}

namespace ov {
HopeNet::HopeNet():
    net_(new ncnn::Net()),
	initialized_(false) {
#ifdef OV_VULKAN
    net_->opt.use_vulkan_compute = true;
#endif // OV_VULKAN
}

HopeNet::~HopeNet() {
	if (net_) {
		net_->clear();
	}
}

int HopeNet::LoadModel(const char * root_path) {
	std::string param_file = std::string(root_path) + "/param";
	std::string bin_file = std::string(root_path) + "/bin";
	if (net_->load_param(param_file.c_str()) == -1 ||
		net_->load_model(bin_file.c_str()) == -1) {
		return 10000;
	}
    for (uint i=1; i<67; i++) idx_tensor[i-1] = i;
	initialized_ = true;

	return 0;
}


int HopeNet::Detect(const unsigned char* rgbdata, 
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
    for(size_t i = 0; i < roi.height; ++i) {
        const unsigned char* srcCursor = start_ptr + ((i + roi.y) * img_width + roi.x) * 3; 
        unsigned char* dstCursor = img_face + i * roi.width * 3;
        memcpy(dstCursor, srcCursor, sizeof(unsigned char) * 3 * roi.width);
    }

    ncnn::Mat in = ncnn::Mat::from_pixels_resize(img_face, ncnn::Mat::PIXEL_RGB2GRAY, roi.width, roi.height, 48, 48);
    ncnn::Extractor ex = net_->create_extractor();
    ex.input("data", in);

    ncnn::Mat output;
    ex.extract("hybridsequential0_multitask0_dense0_fwd", output);
    float* pred_pitch = output.range(0, ODIM);
    float* pred_roll  = output.range(ODIM, ODIM*2);
    float* pred_yaw   = output.range(ODIM*2, ODIM*3);

    softmax(pred_pitch, ODIM);
    softmax(pred_roll,  ODIM);
    softmax(pred_yaw,   ODIM);

    // printArray(pred_pitch, ODIM);

    head_angles->pitch = getAngle(pred_pitch, ODIM);
    head_angles->roll  = getAngle(pred_roll,  ODIM);
    head_angles->yaw   = getAngle(pred_yaw,   ODIM);

    free(img_face);

    return 0;
}

void HopeNet::softmax(float* z, size_t el) {
    double zmax = -INFINITY;
    double zsum = 0;
    for (size_t i = 0; i < el; i++) if (z[i] > zmax) zmax=z[i];
    for (size_t i=0; i<el; i++) z[i] = std::exp(z[i]-zmax);
    zsum = std::accumulate(z, z+el, 0.0);
    for (size_t i=0; i<el; i++) z[i] = (z[i]/zsum)+NEAR_0;
}

double HopeNet::getAngle(float* prediction, size_t len) {
    double expectation[len];
    for (uint i=0; i<len; i++) expectation[i]=idx_tensor[i]*prediction[i];
    double angle = std::accumulate(expectation, expectation+len, 0.0) * 3 - 99;
    return angle;
}

}
