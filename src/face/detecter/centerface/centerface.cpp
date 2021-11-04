#include "centerface.hpp"

#ifdef OV_VULKAN
#include "gpu.h"
#endif // OV_VULKAN

namespace ovface {

int CenterFace::DetectFace(const unsigned char* rgbdata, 
    int img_width, int img_height,
    std::vector<FaceInfo>* faces) {
    faces->clear();
    if (!initialized_) {
        return 10000;
    }
    if (rgbdata == 0){
        return 10001;
    }

	int img_width_new  = img_width / 32 * 32;
	int img_height_new = img_height / 32 * 32;
    float scale_x = static_cast<float>(img_width)  / img_width_new;
    float scale_y = static_cast<float>(img_height) / img_height_new;

    ncnn::Mat in = ncnn::Mat::from_pixels_resize(rgbdata, ncnn::Mat::PIXEL_RGB,
		img_width, img_height, img_width_new, img_height_new);

	ncnn::Extractor ex = net_->create_extractor();
    ex.set_light_mode(light_mode_);
    ex.set_num_threads(num_threads);
	ex.input("input.1", in);
	ncnn::Mat mat_heatmap, mat_scale, mat_offset, mat_landmark;
	ex.extract("537", mat_heatmap);
	ex.extract("538", mat_scale);
	ex.extract("539", mat_offset);
	ex.extract("540", mat_landmark);

    int height = mat_heatmap.h;
	int width = mat_heatmap.w;
    std::vector<FaceInfo> faces_tmp;
	for (int h = 0; h < height; ++h) {
		for (int w = 0; w < width; ++w) {
			int index = h * width + w;
			float score = mat_heatmap[index];
			if (score < scoreThreshold_) {
				continue;
			}
			float s0 = 4 * exp(mat_scale.channel(0)[index]);
			float s1 = 4 * exp(mat_scale.channel(1)[index]);
			float o0 = mat_offset.channel(0)[index];
			float o1 = mat_offset.channel(1)[index];

			float ymin = fmaxf(0, 4 * (h + o0 + 0.5) - 0.5 * s0);
			float xmin = fmaxf(0, 4 * (w + o1 + 0.5) - 0.5 * s1);
			float ymax = fminf(ymin + s0, img_height_new);
			float xmax = fminf(xmin + s1, img_width_new);

            FaceInfo face_info;
            face_info.score = score;
            face_info.rect.x = scale_x * xmin;
            face_info.rect.y = scale_y * ymin;
            face_info.rect.width = scale_x * (xmax - xmin);
            face_info.rect.height = scale_y * (ymax - ymin);

            for (int num = 0; num < 5; ++num) {
                face_info.keypoints_[num    ] = scale_x * (s1 * mat_landmark.channel(2 * num + 1)[index] + xmin);
                face_info.keypoints_[num + 5] = scale_y * (s0 * mat_landmark.channel(2 * num + 0)[index] + ymin);
            }
            faces_tmp.push_back(face_info);
		}
	}
    ov::NMS(faces_tmp, faces, nmsThreshold_);
    return 0;
}

}
