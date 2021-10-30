package estimator

/*
#include <stdlib.h>
#include <stdbool.h>
#include "openvision/common/common.h"
#include "openvision/pose/estimator.h"
*/
import "C"
import (
	"unsafe"

	openvision "github.com/bububa/openvision/go"
	"github.com/bububa/openvision/go/common"
)

// Estimator represents deteter interface
type Estimator interface {
	common.Estimator
	ExtractKeypoints(img *common.Image, rect common.Rectangle) ([]common.Keypoint, error)
}

// ExtractKeypoints detect pose keypoints using estimator
func ExtractKeypoints(d Estimator, img *common.Image, rect common.Rectangle) ([]common.Keypoint, error) {
	imgWidth := img.WidthF64()
	imgHeight := img.HeightF64()
	data := img.Bytes()
	cKeypoints := common.NewCKeypointVector()
	defer common.FreeCKeypointVector(cKeypoints)
	cRect := rect.CRect(imgWidth, imgHeight)
	defer C.free(unsafe.Pointer(cRect))
	errCode := C.extract_pose_keypoints(
		(C.IPoseEstimator)(d.Pointer()),
		(*C.uchar)(unsafe.Pointer(&data[0])),
		C.int(imgWidth),
		C.int(imgHeight),
		(*C.Rect)(unsafe.Pointer(cRect)),
		(*C.KeypointVector)(unsafe.Pointer(cKeypoints)))
	if errCode != 0 {
		return nil, openvision.DetectPoseError(int(errCode))
	}
	return common.GoKeypointVector(cKeypoints, imgWidth, imgHeight), nil
}
