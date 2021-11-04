#include "aligner.h"

IFaceAligner new_face_aligner() {
    return new ovface::Aligner();
}

void destroy_face_aligner(IFaceAligner t) {
    delete static_cast<ovface::Aligner*>(t);
}
void set_face_aligner_threads(IFaceAligner t, int n) {
    static_cast<ovface::Aligner*>(t)->set_num_threads(n);
}

int align_face(IFaceAligner d, const unsigned char* rgbdata, int img_width, int img_height, const Rect* rect, const float keypoints[10], Image* face_aligned) {
    int ret = static_cast<ovface::Aligner*>(d)->Align(rgbdata, img_width, img_height, *rect, keypoints, face_aligned);
    return ret;
}

namespace ovface {

int Aligner::Align(const unsigned char* rgbdata,
    int img_width, int img_height,
    const Rect& rect,
    const float keypoints[10], 
    Image* face_aligned) {
	if (rgbdata == 0) {
		return 10001;
	}
    float points_src[10] = {
		keypoints[0] - rect.x, keypoints[5] - rect.y,
		keypoints[1] - rect.x, keypoints[6] - rect.y,
		keypoints[2] - rect.x, keypoints[7] - rect.y,
		keypoints[3] - rect.x,  keypoints[8] - rect.y,
		keypoints[4] - rect.x, keypoints[9] - rect.y 
	};
    size_t total_size = rect.width * rect.height * 3 * sizeof(unsigned char);
    unsigned char* img_face = (unsigned char*)malloc(total_size);
    const unsigned char *start_ptr = rgbdata;
#if defined(_OPENMP)
#pragma omp parallel for num_threads(num_threads)
#endif
    for(size_t i = 0; i < rect.height; ++i) {
        const unsigned char* srcCursor = start_ptr + ((i + rect.y) * img_width + rect.x) * 3; 
        unsigned char* dstCursor = img_face + i * rect.width * 3;
        memcpy(dstCursor, srcCursor, sizeof(unsigned char) * 3 * rect.width);
    }
    float tm_inv[6]; 
    face_aligned->width = crop_width;
    face_aligned->height = crop_height;
    face_aligned->channels = 3;
    face_aligned->data = (unsigned char*)malloc(crop_width * crop_height * 3 * sizeof(unsigned char));
    ncnn::get_affine_transform(points_dst, points_src, 5, tm_inv);
    ncnn::warpaffine_bilinear_c3(img_face, rect.width, rect.height, face_aligned->data, crop_width, crop_height, tm_inv);
    free(img_face);
    return 0;
}
}
