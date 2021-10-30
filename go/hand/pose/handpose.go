package pose

/*
#include <stdlib.h>
#include <stdbool.h>
#include "openvision/hand/pose.h"
*/
import "C"
import (
	"unsafe"

	"github.com/bububa/openvision/go/common"
)

// HandPoseEstimator represents handpose estimator
type HandPoseEstimator struct {
	d C.IHandPoseEstimator
}

// NewHandPoseEstimator returns a new HandPoseEstimator
func NewHandPoseEstimator() *HandPoseEstimator {
	return &HandPoseEstimator{
		d: C.new_handpose(),
	}
}

// Destroy free Estimator
func (d *HandPoseEstimator) Destroy() {
	common.DestroyEstimator(d)
}

// Pointer implement Estimator interface
func (d *HandPoseEstimator) Pointer() unsafe.Pointer {
	return unsafe.Pointer(d.d)
}

// LoadModel load model for  estimator
func (d *HandPoseEstimator) LoadModel(modelPath string) error {
	return common.EstimatorLoadModel(d, modelPath)
}

// Detect implement Estimator interface
func (d *HandPoseEstimator) Detect(img *common.Image, rect common.Rectangle) ([]common.Point, error) {
	return Detect(d, img, rect)
}
