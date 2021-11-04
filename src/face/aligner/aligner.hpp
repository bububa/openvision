#ifndef _FACE_ALIGNER_H_
#define _FACE_ALIGNER_H_

#include "../../common/common.h"
#include <vector>

namespace ovface {
class Aligner : public ov::EstimatorBase {
public:
	int Align(const unsigned char* rgbdata,
        int img_width, int img_height,
        const ov::Rect& rect,
		const float keypoints[10], 
        Image* face_aligned);

private:
	float points_dst[10] = {
		30.2946f + 8.0f, 51.6963f, // left eye
        65.5318f + 8.0f, 51.5014f, // right eye
        48.0252f + 8.0f, 71.7366f, // nose tip
        33.5493f + 8.0f, 92.3655f, // mouth left
        62.7299f + 8.0f, 92.2041f // mouth right
	};
    int crop_width = 112;
    int crop_height = 112;
};

}

#endif // !_FACE_ALIGNER_H_
