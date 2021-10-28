#include "../detecter.h"
#include "centerface/centerface.hpp"
#include "mtcnn/mtcnn.hpp"
#include "retinaface/retinaface.hpp"
#include "anticonv/anticonv.hpp"

IFaceDetecter new_retinaface() {
    return new ovface::RetinaFace();
}

IFaceDetecter new_centerface() {
    return new ovface::CenterFace();
}

IFaceDetecter new_mtcnn() {
    return new ovface::Mtcnn();
}

IFaceDetecter new_anticonv() {
    return new ovface::AntiConv();
}

int detect_face(IFaceDetecter d, const unsigned char* rgbdata, int img_width, int img_height, FaceInfoVector* faces) {
	std::vector<ovface::FaceInfo> detected;
    int ret = static_cast<ovface::Detecter*>(d)->DetectFace(rgbdata, img_width, img_height, &detected);
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

namespace ovface {
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
