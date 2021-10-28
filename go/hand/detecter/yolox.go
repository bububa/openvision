package detecter

/*
#include <stdlib.h>
#include <stdbool.h>
#include "openvision/hand/detecter.h"
*/
import "C"
import (
	"github.com/bububa/openvision/go/common"
	"github.com/bububa/openvision/go/hand"
)

// Yolox represents yolox detecter
type Yolox struct {
	d C.IHandDetecter
}

// NewYolox returns a new Yolox
func NewYolox() *Yolox {
	return &Yolox{
		d: C.new_yolox(),
	}
}

// Destroy free detecter
func (d *Yolox) Destroy() {
	Destroy(d)
}

// Handler returns C.IHandDetecter
func (d *Yolox) Handler() C.IHandDetecter {
	return d.d
}

// LoadModel load model for detecter
func (d *Yolox) LoadModel(modelPath string) error {
	return LoadModel(d, modelPath)
}

// Detect implement Detecter interface
func (d *Yolox) Detect(img *common.Image) ([]hand.ROI, error) {
	return Detect(d, img)
}
