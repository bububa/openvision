package estimator

/*
#include <stdlib.h>
#include <stdbool.h>
#include "openvision/pose/estimator.h"
*/
import "C"
import (
	"unsafe"

	"github.com/bububa/openvision/go/common"
)

// MoveNetType (lightning/thunder)
type MoveNetType = int

const (
	// MoveNetType_Lightning lightning model
	MoveNetType_Lightning MoveNetType = 0
	// MoveNetType_Thunder thunder model
	MoveNetType_Thunder MoveNetType = 1
)

// MoveNet represents movenet estimator
type MoveNet struct {
	d C.IPoseEstimator
}

// NewMoveNet returns a new MoveNet
func NewMoveNet(modelType MoveNetType) *MoveNet {
	return &MoveNet{
		d: C.new_movenet(C.int(modelType)),
	}
}

// Destroy free detecter
func (d *MoveNet) Destroy() {
	common.DestroyEstimator(d)
}

// Pointer implement Estimator interface
func (d *MoveNet) Pointer() unsafe.Pointer {
	return unsafe.Pointer(d.d)
}

// LoadModel load model for detecter
func (d *MoveNet) LoadModel(modelPath string) error {
	return common.EstimatorLoadModel(d, modelPath)
}

// ExtractKeypoints implement Detecter interface
func (d *MoveNet) ExtractKeypoints(img *common.Image, rect common.Rectangle) ([]common.Keypoint, error) {
	return ExtractKeypoints(d, img, rect)
}
