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

// RetinaFace represents retinaface detecter
type RetinaFace struct {
	d C.IFaceDetecter
}

// NewRetinaFace returns a new RetinaFace
func NewRetinaFace() *RetinaFace {
	return &RetinaFace{
		d: C.new_retinaface(),
	}
}

// Destroy free detecter
func (d *RetinaFace) Destroy() {
	common.DestroyEstimator(d)
}

// Pointer implement Estimator interface
func (d *RetinaFace) Pointer() unsafe.Pointer {
	return unsafe.Pointer(d.d)
}

// LoadModel implement Detecter interface
func (d *RetinaFace) LoadModel(modelPath string) error {
	return common.EstimatorLoadModel(d, modelPath)
}

// Detect implement Detecter interface
func (d *RetinaFace) Detect(img *common.Image) ([]face.FaceInfo, error) {
	return Detect(d, img)
}
