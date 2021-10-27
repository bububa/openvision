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
	Handler() C.IRecognizer
	LoadModel(modelPath string) error
	ExtractFeatures(img *common.Image, face common.Rectangle) ([]float64, error)
	Destroy()
}

// LoadModel load recognizer model
func LoadModel(r Recognizer, modelPath string) error {
	cpath := C.CString(modelPath)
	defer C.free(unsafe.Pointer(cpath))
	retCode := C.load_model((C.IEstimator)(unsafe.Pointer(r.Handler())), cpath)
	if retCode != 0 {
		return openvision.LoadModelError(int(retCode))
	}
	return nil
}

// Destroy a recognizer
func Destroy(r Recognizer) {
	C.destroy_estimator((C.IEstimator)(unsafe.Pointer(r.Handler())))
}

// ExtractFeatures extract features using recognizer
func ExtractFeatures(r Recognizer, img *common.Image, faceRect common.Rectangle) ([]float64, error) {
	imgWidth := img.WidthF64()
	imgHeight := img.HeightF64()
	faceRect.X *= imgWidth
	faceRect.Y *= imgHeight
	faceRect.Width *= imgWidth
	faceRect.Height *= imgHeight
	data := img.Bytes()
	CFeatures := common.NewCFloatVector()
	defer common.FreeCFloatVector(CFeatures)
	CRect := faceRect.CRect()
	errCode := C.extract_feature(
		r.Handler(),
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
