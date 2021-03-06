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

// YoloFace represents yoloface detecter
type YoloFace struct {
	d C.IFaceDetecter
}

// NewYoloFace returns a new YoloFace
func NewYoloFace() *YoloFace {
	return &YoloFace{
		d: C.new_yoloface(),
	}
}

// Destroy free detecter
func (d *YoloFace) Destroy() {
	common.DestroyEstimator(d)
}

// Pointer implement Estimator interface
func (d *YoloFace) Pointer() unsafe.Pointer {
	return unsafe.Pointer(d.d)
}

// LoadModel implement Detecter interface
func (d *YoloFace) LoadModel(modelPath string) error {
	return common.EstimatorLoadModel(d, modelPath)
}

// Detect implement Detecter interface
func (d *YoloFace) Detect(img *common.Image) ([]face.FaceInfo, error) {
	return Detect(d, img)
}
