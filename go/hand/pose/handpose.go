package pose

/*
#include <stdlib.h>
#include <stdbool.h>
#include "openvision/hand/pose.h"
*/
import "C"
import (
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
	Destroy(d)
}

// Handler returns C.IHandPoseEstimator
func (d *HandPoseEstimator) Handler() C.IHandPoseEstimator {
	return d.d
}

// LoadModel load model for  estimator
func (d *HandPoseEstimator) LoadModel(modelPath string) error {
	return LoadModel(d, modelPath)
}

// Detect implement Estimator interface
func (d *HandPoseEstimator) Detect(img *common.Image, rect common.Rectangle) ([]common.Point, error) {
	return Detect(d, img, rect)
}
