package pose

/*
#include <stdlib.h>
#include <stdbool.h>
#include "openvision/common/common.h"
#include "openvision/hand/pose.h"
*/
import "C"
import (
	"unsafe"

	openvision "github.com/bububa/openvision/go"
	"github.com/bububa/openvision/go/common"
)

// Estimator represents estimator interface
type Estimator interface {
	common.Estimator
	Detect(img *common.Image, rect common.Rectangle) ([]common.Point, error)
}

// Detect detect hand pose
func Detect(d Estimator, img *common.Image, rect common.Rectangle) ([]common.Point, error) {
	imgWidth := img.WidthF64()
	imgHeight := img.HeightF64()
	data := img.Bytes()
	CPoints := common.NewCPoint2fVector()
	defer common.FreeCPoint2fVector(CPoints)
	CRect := rect.CRect(imgWidth, imgHeight)
	errCode := C.hand_pose(
		(C.IHandPoseEstimator)(d.Pointer()),
		(*C.uchar)(unsafe.Pointer(&data[0])),
		C.int(imgWidth), C.int(imgHeight),
		(*C.Rect)(unsafe.Pointer(CRect)),
		(*C.Point2fVector)(unsafe.Pointer(CPoints)),
	)
	if errCode != 0 {
		return nil, openvision.DetectHandError(int(errCode))
	}
	return common.GoPoint2fVector(CPoints, imgWidth, imgHeight), nil
}
