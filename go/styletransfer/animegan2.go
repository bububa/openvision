package styletransfer

/*
#include <stdlib.h>
#include <stdbool.h>
#include "openvision/styletransfer/styletransfer.h"
*/
import "C"
import (
	"image"
	"unsafe"

	"github.com/bububa/openvision/go/common"
)

// AnimeGan2 represents animegan2 style transfer
type AnimeGan2 struct {
	d C.IStyleTransfer
}

// NewAnimeGan2 returns a new AnimeGan2
func NewAnimeGan2() *AnimeGan2 {
	return &AnimeGan2{
		d: C.new_animegan2_style_transfer(),
	}
}

// Destroy free style transfer
func (d *AnimeGan2) Destroy() {
	common.DestroyEstimator(d)
}

// Pointer implement Estimator interface
func (d *AnimeGan2) Pointer() unsafe.Pointer {
	return unsafe.Pointer(d.d)
}

// LoadModel load model for detecter
func (d *AnimeGan2) LoadModel(modelPath string) error {
	return common.EstimatorLoadModel(d, modelPath)
}

// Transform implement StyleTransfer interface
func (d *AnimeGan2) Transform(img *common.Image) (image.Image, error) {
	return Transform(d, img)
}
