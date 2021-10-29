package detecter

/*
#include <stdlib.h>
#include <stdbool.h>
#include "openvision/hand/detecter.h"
*/
import "C"
import (
	"github.com/bububa/openvision/go/common"
)

// Nanodet represents nanodet detecter
type Nanodet struct {
	d C.IHandDetecter
}

// NewNanodet returns a new Nanodet
func NewNanodet() *Nanodet {
	return &Nanodet{
		d: C.new_nanodet(),
	}
}

// Destroy free detecter
func (d *Nanodet) Destroy() {
	Destroy(d)
}

// Handler returns C.IHandDetecter
func (d *Nanodet) Handler() C.IHandDetecter {
	return d.d
}

// LoadModel load model for detecter
func (d *Nanodet) LoadModel(modelPath string) error {
	return LoadModel(d, modelPath)
}

// Detect implement Detecter interface
func (d *Nanodet) Detect(img *common.Image) ([]common.ObjectInfo, error) {
	return Detect(d, img)
}
