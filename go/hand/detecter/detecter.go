package detecter

/*
#include <stdlib.h>
#include <stdbool.h>
#include "openvision/hand/common.h"
#include "openvision/hand/detecter.h"
*/
import "C"
import (
	"unsafe"

	openvision "github.com/bububa/openvision/go"
	"github.com/bububa/openvision/go/common"
	"github.com/bububa/openvision/go/hand"
)

// Detecter represents deteter interface
type Detecter interface {
	Handler() C.IHandDetecter
	LoadModel(modelPath string) error
	Detect(img *common.Image) ([]hand.ROI, error)
	Destroy()
}

// LoadModel load detecter model
func LoadModel(d Detecter, modelPath string) error {
	cpath := C.CString(modelPath)
	defer C.free(unsafe.Pointer(cpath))
	retCode := C.load_model((C.IEstimator)(unsafe.Pointer(d.Handler())), cpath)
	if retCode != 0 {
		return openvision.LoadModelError(int(retCode))
	}
	return nil
}

// Destroy a detecter
func Destroy(d Detecter) {
	C.destroy_estimator((C.IEstimator)(unsafe.Pointer(d.Handler())))
}

// Detect detect hand ROI
func Detect(d Detecter, img *common.Image) ([]hand.ROI, error) {
	imgWidth := img.WidthF64()
	imgHeight := img.HeightF64()
	data := img.Bytes()
	cROIs := hand.NewCROIVector()
	defer hand.FreeCROIVector(cROIs)
	errCode := C.extract_hand_rois(
		d.Handler(),
		(*C.uchar)(unsafe.Pointer(&data[0])),
		C.int(imgWidth),
		C.int(imgHeight),
		(*C.HandROIVector)(unsafe.Pointer(cROIs)))
	if errCode != 0 {
		return nil, openvision.DetectHandError(int(errCode))
	}
	return hand.GoROIVector(cROIs, imgWidth, imgHeight), nil
}
