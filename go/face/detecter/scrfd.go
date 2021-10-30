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

// Scrfd represents scrfd detecter
type Scrfd struct {
	d C.IFaceDetecter
}

// NewScrfd returns a new Scrfd
func NewScrfd() *Scrfd {
	return &Scrfd{
		d: C.new_scrfd(),
	}
}

// Destroy free detecter
func (d *Scrfd) Destroy() {
	common.DestroyEstimator(d)
}

// LoadModel implement Detecter interface
func (d *Scrfd) LoadModel(modelPath string) error {
	return common.EstimatorLoadModel(d, modelPath)
}

// Pointer implement Estimator interface
func (d *Scrfd) Pointer() unsafe.Pointer {
	return unsafe.Pointer(d.d)
}

// Detect implement Detecter interface
func (d *Scrfd) Detect(img *common.Image) ([]face.FaceInfo, error) {
	return Detect(d, img)
}
