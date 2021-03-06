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
	IsClosed(img *common.Image, face common.Rectangle) (bool, error)
}

// IsClosed check whether eyes are closed
func IsClosed(r Detecter, img *common.Image, faceRect common.Rectangle) (bool, error) {
	imgWidth := img.WidthF64()
	imgHeight := img.HeightF64()
	data := img.Bytes()
	scoresC := common.NewCFloatVector()
	defer common.FreeCFloatVector(scoresC)
	CRect := faceRect.CRect(imgWidth, imgHeight)
	errCode := C.eye_status(
		(C.IEye)(r.Pointer()),
		(*C.uchar)(unsafe.Pointer(&data[0])),
		C.int(imgWidth), C.int(imgHeight),
		(*C.Rect)(unsafe.Pointer(CRect)),
		(*C.FloatVector)(unsafe.Pointer(scoresC)),
	)
	C.free(unsafe.Pointer(CRect))
	if errCode != 0 {
		return false, openvision.RecognizeFaceError(int(errCode))
	}
	scores := common.GoFloatVector(scoresC)
	return len(scores) > 0 && scores[0] == 1, nil
}
