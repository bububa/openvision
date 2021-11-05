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

// ERDNet represents erdnet segmentor
type ERDNet struct {
	d C.IPoseSegmentor
}

// NewERDNet returns a new ERDNet
func NewERDNet() *ERDNet {
	return &ERDNet{
		d: C.new_erdnet_pose_segmentor(),
	}
}

// Destroy free segmentor
func (d *ERDNet) Destroy() {
	common.DestroyEstimator(d)
}

// Pointer implement Estimator interface
func (d *ERDNet) Pointer() unsafe.Pointer {
	return unsafe.Pointer(d.d)
}

// LoadModel load model for detecter
func (d *ERDNet) LoadModel(modelPath string) error {
	return common.EstimatorLoadModel(d, modelPath)
}

// Matting implement Segmentor interface
func (d *ERDNet) Matting(img *common.Image) (image.Image, error) {
	return Matting(d, img)
}

// Merge implement Segmentor interface
func (d *ERDNet) Merge(img *common.Image, bg *common.Image) (image.Image, error) {
	return Merge(d, img, bg)
}
