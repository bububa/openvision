package recognizer

/*
#include <stdlib.h>
#include <stdbool.h>
#include "openvision/common/common.h"
#include "openvision/face/recognizer.h"
*/
import "C"
import (
	"unsafe"

	openvision "github.com/bububa/openvision/go"
	"github.com/bububa/openvision/go/common"
)

// Recognizer represents Recognizer interface
type Recognizer interface {
	common.Estimator
	ExtractFeatures(img *common.Image, face common.Rectangle) ([]float64, error)
}

// ExtractFeatures extract features using recognizer
func ExtractFeatures(r Recognizer, img *common.Image, faceRect common.Rectangle) ([]float64, error) {
	imgWidth := img.WidthF64()
	imgHeight := img.HeightF64()
	data := img.Bytes()
	CFeatures := common.NewCFloatVector()
	defer common.FreeCFloatVector(CFeatures)
	CRect := faceRect.CRect(imgWidth, imgHeight)
	errCode := C.extract_feature(
		(C.IFaceRecognizer)(r.Pointer()),
		(*C.uchar)(unsafe.Pointer(&data[0])),
		C.int(imgWidth), C.int(imgHeight),
		(*C.Rect)(unsafe.Pointer(CRect)),
		(*C.FloatVector)(unsafe.Pointer(CFeatures)),
	)
	C.free(unsafe.Pointer(CRect))
	if errCode != 0 {
		return nil, openvision.RecognizeFaceError(int(errCode))
	}
	features := common.GoFloatVector(CFeatures)
	return features, nil
}
