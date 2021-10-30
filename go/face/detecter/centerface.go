package detecter

/*
#include <stdlib.h>
#include <stdbool.h>
#include "openvision/face/detecter.h"
*/
import "C"
import (
	"unsafe"

	"github.com/bububa/openvision/go/common"
	"github.com/bububa/openvision/go/face"
)

// Centerface represents centerface detecter
type Centerface struct {
	d C.IFaceDetecter
}

// NewCenterface returns a new Centerface
func NewCenterface() *Centerface {
	return &Centerface{
		d: C.new_centerface(),
	}
}

// Destroy free detecter
func (d *Centerface) Destroy() {
	common.DestroyEstimator(d)
}

// Pointer implement Estimator interface
func (d *Centerface) Pointer() unsafe.Pointer {
	return unsafe.Pointer(d.d)
}

// LoadModel load model for detecter
func (d *Centerface) LoadModel(modelPath string) error {
	return common.EstimatorLoadModel(d, modelPath)
}

// Detect implement Detecter interface
func (d *Centerface) Detect(img *common.Image) ([]face.FaceInfo, error) {
	return Detect(d, img)
}
