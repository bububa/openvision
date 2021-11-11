package segmentor

/*
#include <stdlib.h>
#include <stdbool.h>
#include "openvision/pose/segmentor.h"
*/
import "C"
import (
	"unsafe"

	"github.com/bububa/openvision/go/common"
)

// RVM represents rvm segmentor
type RVM struct {
	d C.IPoseSegmentor
}

// NewRVM returns a new RVM
func NewRVM(width int, height int) *RVM {
	return &RVM{
		d: C.new_rvm_pose_segmentor(
			C.int(width),
			C.int(height),
		),
	}
}

// Destroy free segmentor
func (d *RVM) Destroy() {
	common.DestroyEstimator(d)
}

// Pointer implement Estimator interface
func (d *RVM) Pointer() unsafe.Pointer {
	return unsafe.Pointer(d.d)
}

// LoadModel load model for detecter
func (d *RVM) LoadModel(modelPath string) error {
	return common.EstimatorLoadModel(d, modelPath)
}

// Matting implement Segmentor interface
func (d *RVM) Matting(img *common.Image, out *common.Image) error {
	return Matting(d, img, out)
}

// Merge implement Segmentor interface
func (d *RVM) Merge(img *common.Image, bg *common.Image, out *common.Image) error {
	return Merge(d, img, bg, out)
}
