#include "retinaface.hpp"

#ifdef OV_VULKAN
#include "gpu.h"
#endif // OV_VULKAN

namespace ovface {

int RetinaFace::LoadModel(const char * root_path) {
    int ret = Estimator::LoadModel(root_path);
    if (ret != 0) {
        return ret;
    }
	// generate anchors
	for (int i = 0; i < 3; ++i) {
		ANCHORS anchors;
		if (0 == i) {
			GenerateAnchors(16, { 1.0f }, { 32, 16 }, &anchors, num_threads);
		}
		else if (1 == i) {
			GenerateAnchors(16, { 1.0f }, { 8, 4 }, &anchors, num_threads);
		}
		else {
			GenerateAnchors(16, { 1.0f }, { 2, 1 }, &anchors, num_threads);
		}
		anchors_generated_.push_back(anchors);
	}

	return 0;
}

int RetinaFace::DetectFace(const unsigned char* rgbdata, 
    int img_width, int img_height,
	std::vector<FaceInfo>* faces) {
	faces->clear();
	if (!initialized_) {
		return 10000;
	}
	if (rgbdata == 0) {
		return 10001;
	}

	float factor_x = static_cast<float>(img_width) / inputSize_.width;
	float factor_y = static_cast<float>(img_height) / inputSize_.height;
	ncnn::Mat in = ncnn::Mat::from_pixels_resize(rgbdata,
		ncnn::Mat::PIXEL_RGB, img_width, img_height, inputSize_.width, inputSize_.height);

	ncnn::Extractor ex = net_->create_extractor();
    ex.set_light_mode(light_mode_);
    ex.set_num_threads(num_threads);
	ex.input("data", in);
	
	std::vector<FaceInfo> faces_tmp;
	for (int i = 0; i < 3; ++i) {
		std::string class_layer_name = "face_rpn_cls_prob_reshape_stride" + std::to_string(RPNs_[i]);
		std::string bbox_layer_name = "face_rpn_bbox_pred_stride" + std::to_string(RPNs_[i]);
		std::string landmark_layer_name = "face_rpn_landmark_pred_stride" + std::to_string(RPNs_[i]);

		ncnn::Mat class_mat, bbox_mat, landmark_mat;
		ex.extract(class_layer_name.c_str(), class_mat);
		ex.extract(bbox_layer_name.c_str(), bbox_mat);
		ex.extract(landmark_layer_name.c_str(), landmark_mat);

		ANCHORS anchors = anchors_generated_.at(i);
		int width = class_mat.w;
		int height = class_mat.h;
		int anchor_num = static_cast<int>(anchors.size());
		for (int h = 0; h < height; ++h) {
			for (int w = 0; w < width; ++w) {
				int index = h * width + w;
				for (int a = 0; a < anchor_num; ++a) {
					float score = class_mat.channel(anchor_num + a)[index];
					if (score < scoreThreshold_) {
						continue;
					}
					// 1.??????anchor?????????box
					Rect box = Rect(w * RPNs_[i] + anchors[a].x,
						h * RPNs_[i] + anchors[a].y,
						anchors[a].width,
						anchors[a].height);

					// 2.??????????????????
					float delta_x = bbox_mat.channel(a * 4 + 0)[index];
					float delta_y = bbox_mat.channel(a * 4 + 1)[index];
					float delta_w = bbox_mat.channel(a * 4 + 2)[index];
					float delta_h = bbox_mat.channel(a * 4 + 3)[index];

					// 3.??????anchor box?????????
					Point2f center = Point2f(box.x + box.width * 0.5f,
						box.y + box.height * 0.5f);
					
					// 4.???????????????????????????anchor?????????+????????????
					center.x = center.x + delta_x * box.width;
					center.y = center.y + delta_y * box.height;

					// 5.???????????????????????????
					float curr_width = exp(delta_w) * (box.width + 1);
					float curr_height = exp(delta_h) * (box.height + 1);

					// 6.???????????????????????????
					Rect curr_box = Rect(center.x - curr_width * 0.5f,
						center.y - curr_height * 0.5f, curr_width, 	curr_height);
					curr_box.x = fmaxf(curr_box.x * factor_x, 0);
					curr_box.y = fmaxf(curr_box.y * factor_y, 0);
					curr_box.width = fminf(img_width - curr_box.x, curr_box.width * factor_x);
					curr_box.height = fminf(img_height - curr_box.y, curr_box.height * factor_y);
					
					FaceInfo face_info;
					memset(&face_info, 0, sizeof(face_info));
					
					int offset_index = landmark_mat.c / anchor_num;
					for (int k = 0; k < 5; ++k) {
						float x = landmark_mat.channel(a * offset_index + 2 * k)[index] * box.width + center.x;
						float y = landmark_mat.channel(a * offset_index + 2 * k + 1)[index] * box.height + center.y;
						face_info.keypoints_[k] = fminf(fmaxf(x * factor_x, 0.0f), img_width - 1);
						face_info.keypoints_[k + 5] = fminf(fmaxf(y * factor_y, 0.0f), img_height - 1);
					}

					face_info.score = score;
					face_info.rect = curr_box;
					faces_tmp.push_back(face_info);
				}
			}
		}
	}
	
    ov::NMS(faces_tmp, faces, iouThreshold_);
	return 0;
}

}
