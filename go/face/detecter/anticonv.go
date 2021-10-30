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

// Anticonv represents anticonv detecter
type Anticonv struct {
	d C.IFaceDetecter
}

// NewAnticonv returns a new Anticonv
func NewAnticonv() *Anticonv {
	return &Anticonv{
		d: C.new_anticonv(),
	}
}

// Destroy free detecter
func (d *Anticonv) Destroy() {
	common.DestroyEstimator(d)
}

// Pointer implement Estimator interface
func (d *Anticonv) Pointer() unsafe.Pointer {
	return unsafe.Pointer(d.d)
}

// LoadModel load model for detecter
func (d *Anticonv) LoadModel(modelPath string) error {
	return common.EstimatorLoadModel(d, modelPath)
}

// Detect implement Detecter interface
func (d *Anticonv) Detect(img *common.Image) ([]face.FaceInfo, error) {
	return Detect(d, img)
}
