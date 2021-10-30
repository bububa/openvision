package detecter

/*
#include <stdlib.h>
#include <stdbool.h>
#include "openvision/pose/detecter.h"
*/
import "C"
import (
	"unsafe"

	"github.com/bububa/openvision/go/common"
)

// Ultralight represents utralight detecter
type Ultralight struct {
	d C.IPoseDetecter
}

// NewUltralight returns a new Utralight
func NewUltralight() *Ultralight {
	return &Ultralight{
		d: C.new_ultralight(),
	}
}

// Destroy free detecter
func (d *Ultralight) Destroy() {
	common.DestroyEstimator(d)
}

// Pointer implement Estimator interface
func (d *Ultralight) Pointer() unsafe.Pointer {
	return unsafe.Pointer(d.d)
}

// LoadModel load model for detecter
func (d *Ultralight) LoadModel(modelPath string) error {
	return common.EstimatorLoadModel(d, modelPath)
}

// ExtractKeypoints implement Detecter interface
func (d *Ultralight) ExtractKeypoints(img *common.Image) ([]common.ObjectInfo, error) {
	return ExtractKeypoints(d, img)
}
