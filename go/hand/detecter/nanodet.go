package detecter

/*
#include <stdlib.h>
#include <stdbool.h>
#include "openvision/hand/detecter.h"
*/
import "C"
import (
	"unsafe"

	"github.com/bububa/openvision/go/common"
)

// Nanodet represents nanodet detecter
type Nanodet struct {
	d C.IHandDetecter
}

// NewNanodet returns a new Nanodet
func NewNanodet() *Nanodet {
	return &Nanodet{
		d: C.new_nanodet(),
	}
}

// Destroy free detecter
func (d *Nanodet) Destroy() {
	common.DestroyEstimator(d)
}

// Pointer implement Estimator interface
func (d *Nanodet) Pointer() unsafe.Pointer {
	return unsafe.Pointer(d.d)
}

// LoadModel load model for detecter
func (d *Nanodet) LoadModel(modelPath string) error {
	return common.EstimatorLoadModel(d, modelPath)
}

// Detect implement Detecter interface
func (d *Nanodet) Detect(img *common.Image) ([]common.ObjectInfo, error) {
	return Detect(d, img)
}
