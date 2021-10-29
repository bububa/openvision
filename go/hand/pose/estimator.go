package pose

/*
#include <stdlib.h>
#include <stdbool.h>
#include "openvision/hand/common.h"
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
	Handler() C.IHandPoseEstimator
	LoadModel(modelPath string) error
	Detect(img *common.Image, rect common.Rectangle) ([]common.Point, error)
	Destroy()
}

// LoadModel load detecter model
func LoadModel(d Estimator, modelPath string) error {
	cpath := C.CString(modelPath)
	defer C.free(unsafe.Pointer(cpath))
	retCode := C.load_model((C.IEstimator)(unsafe.Pointer(d.Handler())), cpath)
	if retCode != 0 {
		return openvision.LoadModelError(int(retCode))
	}
	return nil
}

// Destroy a estimator
func Destroy(d Estimator) {
	C.destroy_estimator((C.IEstimator)(unsafe.Pointer(d.Handler())))
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
		d.Handler(),
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
