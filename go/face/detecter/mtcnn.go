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

// Mtcnn represents mtcnn detecter
type Mtcnn struct {
	d C.IFaceDetecter
}

// NewMtcnn returns a new Mtcnn
func NewMtcnn() *Mtcnn {
	return &Mtcnn{
		d: C.new_mtcnn(),
	}
}

// Destroy free detecter
func (d *Mtcnn) Destroy() {
	common.DestroyEstimator(d)
}

// Pointer implement Estimator interface
func (d *Mtcnn) Pointer() unsafe.Pointer {
	return unsafe.Pointer(d.d)
}

// LoadModel implement Detecter interface
func (d *Mtcnn) LoadModel(modelPath string) error {
	return common.EstimatorLoadModel(d, modelPath)
}

// Detect implement Detecter interface
func (d *Mtcnn) Detect(img *common.Image) ([]face.FaceInfo, error) {
	return Detect(d, img)
}
