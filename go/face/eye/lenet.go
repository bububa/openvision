package eye

/*
#include <stdlib.h>
#include <stdbool.h>
#include "openvision/face/eye.h"
*/
import "C"
import (
	"unsafe"

	"github.com/bububa/openvision/go/common"
)

// Lenet represents Lenet detecter
type Lenet struct {
	d C.IEye
}

// NetLenet returns a new Lenet detecter
func NewLenet() *Lenet {
	return &Lenet{
		d: C.new_lenet_eye(),
	}
}

// Pointer implement Estimator interface
func (d *Lenet) Pointer() unsafe.Pointer {
	return unsafe.Pointer(d.d)
}

// LoadModel implement Recognizer interface
func (d *Lenet) LoadModel(modelPath string) error {
	return common.EstimatorLoadModel(d, modelPath)
}

// Destroy implement Recognizer interface
func (d *Lenet) Destroy() {
	common.DestroyEstimator(d)
}

// IsClosed implement Eye Detecter interface
func (d *Lenet) IsClosed(img *common.Image, faceRect common.Rectangle) (bool, error) {
	return IsClosed(d, img, faceRect)
}
