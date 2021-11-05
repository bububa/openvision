package segmentor

/*
#include <stdlib.h>
#include <stdbool.h>
#include "openvision/pose/segmentor.h"
*/
import "C"
import (
	"image"
	"unsafe"

	"github.com/bububa/openvision/go/common"
)

// Deeplabv3plus represents deeplabv3plus segmentor
type Deeplabv3plus struct {
	d C.IPoseSegmentor
}

// NewDeeplabv3plus returns a new Deeplabv3plus
func NewDeeplabv3plus() *Deeplabv3plus {
	return &Deeplabv3plus{
		d: C.new_deeplabv3plus_pose_segmentor(),
	}
}

// Destroy free segmentor
func (d *Deeplabv3plus) Destroy() {
	common.DestroyEstimator(d)
}

// Pointer implement Estimator interface
func (d *Deeplabv3plus) Pointer() unsafe.Pointer {
	return unsafe.Pointer(d.d)
}

// LoadModel load model for detecter
func (d *Deeplabv3plus) LoadModel(modelPath string) error {
	return common.EstimatorLoadModel(d, modelPath)
}

// Matting implement Segmentor interface
func (d *Deeplabv3plus) Matting(img *common.Image) (image.Image, error) {
	return Matting(d, img)
}

// Merge implement Segmentor interface
func (d *Deeplabv3plus) Merge(img *common.Image, bg *common.Image) (image.Image, error) {
	return Merge(d, img, bg)
}
