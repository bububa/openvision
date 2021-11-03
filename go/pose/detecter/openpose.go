package detecter

/*
#include <stdlib.h>
#include <stdbool.h>
#include "openvision/common/common.h"
#include "openvision/pose/detecter.h"
*/
import "C"
import (
	"unsafe"

	"github.com/bububa/openvision/go/common"
)

// OpenPose represents utralight detecter
type OpenPose struct {
	d C.IPoseDetecter
}

// NewOpenPose returns a new OpenPose
func NewOpenPose() *OpenPose {
	return &OpenPose{
		d: C.new_openpose(),
	}
}

// Destroy free detecter
func (d *OpenPose) Destroy() {
	common.DestroyEstimator(d)
}

// Pointer implement Estimator interface
func (d *OpenPose) Pointer() unsafe.Pointer {
	return unsafe.Pointer(d.d)
}

// HasKeypoints implement Detecter interface
func (d *OpenPose) HasKeypoints() bool {
	return true
}

// LoadModel load model for detecter
func (d *OpenPose) LoadModel(modelPath string) error {
	return common.EstimatorLoadModel(d, modelPath)
}

// Detect implement Detecter interface
func (d *OpenPose) Detect(img *common.Image) ([]common.ObjectInfo, error) {
	return Detect(d, img)
}
