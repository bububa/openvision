package eye

/*
#include <stdlib.h>
#include <stdbool.h>
#include "openvision/common/common.h"
#include "openvision/face/eye.h"
*/
import "C"
import (
	"unsafe"

	openvision "github.com/bububa/openvision/go"
	"github.com/bububa/openvision/go/common"
)

// Detecter represents Eye Detector interface
type Detecter interface {
	common.Estimator
	Status(img *common.Image, face common.Rectangle) ([]float64, error)
}

// Status extract scores using recognizer
func Status(r Detecter, img *common.Image, faceRect common.Rectangle) ([]float64, error) {
	imgWidth := img.WidthF64()
	imgHeight := img.HeightF64()
	data := img.Bytes()
	CFeatures := common.NewCFloatVector()
	defer common.FreeCFloatVector(CFeatures)
	CRect := faceRect.CRect(imgWidth, imgHeight)
	errCode := C.eye_status(
		(C.IEye)(r.Pointer()),
		(*C.uchar)(unsafe.Pointer(&data[0])),
		C.int(imgWidth), C.int(imgHeight),
		(*C.Rect)(unsafe.Pointer(CRect)),
		(*C.FloatVector)(unsafe.Pointer(CFeatures)),
	)
	C.free(unsafe.Pointer(CRect))
	if errCode != 0 {
		return nil, openvision.RecognizeFaceError(int(errCode))
	}
	return common.GoFloatVector(CFeatures), nil
}
