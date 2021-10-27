package hopenet

/*
#include <stdlib.h>
#include <stdbool.h>
#include "openvision/face/hopenet.h"
*/
import "C"
import (
	"unsafe"

	openvision "github.com/bububa/openvision/go"
	"github.com/bububa/openvision/go/common"
	"github.com/bububa/openvision/go/face"
)

// Hopenet represents Hopenet detecter
type Hopenet struct {
	d C.IHopenet
}

// NewHopenet returns a new Hopenet
func NewHopenet() *Hopenet {
	return &Hopenet{
		d: C.new_hopenet(),
	}
}

// LoadModel load detecter model
func (h *Hopenet) LoadModel(modelPath string) error {
	cpath := C.CString(modelPath)
	defer C.free(unsafe.Pointer(cpath))
	retCode := C.load_model((C.IEstimator)(unsafe.Pointer(h.d)), cpath)
	if retCode != 0 {
		return openvision.LoadModelError(int(retCode))
	}
	return nil
}

// Destroy destroy C.IHopeNet
func (h *Hopenet) Destroy() {
	C.destroy_estimator((C.IEstimator)(unsafe.Pointer(h.d)))
}

// Detect head pose
func (h *Hopenet) Detect(img *common.Image, faceRect common.Rectangle) (face.HeadPose, error) {
	imgWidth := img.WidthF64()
	imgHeight := img.HeightF64()
	faceRect.X *= imgWidth
	faceRect.Y *= imgHeight
	faceRect.Width *= imgWidth
	faceRect.Height *= imgHeight
	data := img.Bytes()
	CRect := faceRect.CRect()
	CHeadPose := face.NewCHeadPose()
	defer C.free(unsafe.Pointer(CHeadPose))
	errCode := C.hopenet_detect(
		h.d,
		(*C.uchar)(unsafe.Pointer(&data[0])),
		C.int(imgWidth), C.int(imgHeight),
		(*C.Rect)(unsafe.Pointer(CRect)),
		(*C.HeadPose)(unsafe.Pointer(CHeadPose)),
	)
	C.free(unsafe.Pointer(CRect))
	if errCode != 0 {
		return face.HeadPose{}, openvision.DetectHeadPoseError(int(errCode))
	}
	return face.GoHeadPose(CHeadPose), nil
}
