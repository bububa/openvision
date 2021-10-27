#include "../detecter.h"
#include "centerface/centerface.hpp"
#include "mtcnn/mtcnn.hpp"
#include "retinaface/retinaface.hpp"
#include "anticonv/anticonv.hpp"

IDetecter new_retinaface() {
    return new ov::RetinaFace();
}

IDetecter new_centerface() {
    return new ov::CenterFace();
}

IDetecter new_mtcnn() {
    return new ov::Mtcnn();
}

IDetecter new_anticonv() {
    return new ov::AntiConv();
}

int detect_face(IDetecter d, const unsigned char* rgbdata, int img_width, int img_height, FaceInfoVector* faces) {
	std::vector<FaceInfo> detected;
    int ret = static_cast<ov::Detecter*>(d)->DetectFace(rgbdata, img_width, img_height, &detected);
    if (ret != 0) {
        return ret;
    }

    FaceInfo* fps = new FaceInfo[detected.size()]; 
    for (size_t i = 0; i < detected.size(); ++i) {
        fps[i] = detected[i];
    }
    faces->length = detected.size();
    faces->faces = fps;
    return 0;
}

namespace ov{
Detecter* CenterfaceFactory::CreateDetecter() {
	return new CenterFace();
}

Detecter* MtcnnFactory::CreateDetecter() {
	return new Mtcnn();
}

Detecter* RetinafaceFactory::CreateDetecter() {
	return new RetinaFace();
}

Detecter* AnticonvFactory::CreateDetecter() {
	return new AntiConv();
}

}
