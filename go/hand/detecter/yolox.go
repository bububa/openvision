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

// Yolox represents yolox detecter
type Yolox struct {
	d C.IHandDetecter
}

// NewYolox returns a new Yolox
func NewYolox() *Yolox {
	return &Yolox{
		d: C.new_yolox(),
	}
}

// Destroy free detecter
func (d *Yolox) Destroy() {
	common.DestroyEstimator(d)
}

// Pointer implenment Estimator interface
func (d *Yolox) Pointer() unsafe.Pointer {
	return unsafe.Pointer(d.d)
}

// LoadModel load model for detecter
func (d *Yolox) LoadModel(modelPath string) error {
	return common.EstimatorLoadModel(d, modelPath)
}

// Detect implement Detecter interface
func (d *Yolox) Detect(img *common.Image) ([]common.ObjectInfo, error) {
	return Detect(d, img)
}
