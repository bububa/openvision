package hair

/*
#include <stdlib.h>
#include <stdbool.h>
#include "openvision/face/hair.h"
*/
import "C"
import (
	"unsafe"

	openvision "github.com/bububa/openvision/go"
	"github.com/bububa/openvision/go/common"
)

// Hair represents Hair segmentor
type Hair struct {
	d C.IHair
}

// NewHair returns a new Hair
func NewHair() *Hair {
	return &Hair{
		d: C.new_hair(),
	}
}

// Pointer implement Estimator interface
func (h *Hair) Pointer() unsafe.Pointer {
	return unsafe.Pointer(h.d)
}

// LoadModel load detecter model
func (h *Hair) LoadModel(modelPath string) error {
	return common.EstimatorLoadModel(h, modelPath)
}

// Destroy destroy C.IHair
func (h *Hair) Destroy() {
	common.DestroyEstimator(h)
}

// Matting returns hair matting image
func (h *Hair) Matting(img *common.Image, out *common.Image) error {
	imgWidth := img.WidthF64()
	imgHeight := img.HeightF64()
	data := img.Bytes()
	outImgC := common.NewCImage()
	defer common.FreeCImage(outImgC)
	errCode := C.hair_matting(
		(C.IHair)(h.Pointer()),
		(*C.uchar)(unsafe.Pointer(&data[0])),
		C.int(imgWidth), C.int(imgHeight),
		(*C.Image)(unsafe.Pointer(outImgC)),
	)
	if errCode != 0 {
		return openvision.HairMattingError(int(errCode))
	}
	common.GoImage(outImgC, out)
	return nil
}
