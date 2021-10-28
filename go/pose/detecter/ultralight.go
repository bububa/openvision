package detecter

/*
#include <stdlib.h>
#include <stdbool.h>
#include "openvision/pose/detecter.h"
*/
import "C"
import (
	"github.com/bububa/openvision/go/common"
	"github.com/bububa/openvision/go/pose"
)

// Ultralight represents utralight detecter
type Ultralight struct {
	d C.IPoseDetecter
}

// NewUltralight returns a new Utralight
func NewUltralight() *Ultralight {
	return &Ultralight{
		d: C.new_ultralight(),
	}
}

// Destroy free detecter
func (d *Ultralight) Destroy() {
	Destroy(d)
}

// Handler returns C.IPoseDetecter
func (d *Ultralight) Handler() C.IPoseDetecter {
	return d.d
}

// LoadModel load model for detecter
func (d *Ultralight) LoadModel(modelPath string) error {
	return LoadModel(d, modelPath)
}

// ExtractKeypoints implement Detecter interface
func (d *Ultralight) ExtractKeypoints(img *common.Image) ([]pose.ROI, error) {
	return ExtractKeypoints(d, img)
}
