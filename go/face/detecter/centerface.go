package detecter

/*
#include <stdlib.h>
#include <stdbool.h>
#include "openvision/face/detecter.h"
*/
import "C"
import (
	"github.com/bububa/openvision/go/common"
	"github.com/bububa/openvision/go/face"
)

// Centerface represents centerface detecter
type Centerface struct {
	d C.IDetecter
}

// NewCenterface returns a new Centerface
func NewCenterface() *Centerface {
	return &Centerface{
		d: C.new_centerface(),
	}
}

// Destroy free detecter
func (d *Centerface) Destroy() {
	Destroy(d)
}

// Handler returns C.IDetecter
func (d *Centerface) Handler() C.IDetecter {
	return d.d
}

// LoadModel load model for detecter
func (d *Centerface) LoadModel(modelPath string) error {
	return LoadModel(d, modelPath)
}

// DetectFace implement Detecter interface
func (d *Centerface) DetectFace(img *common.Image) ([]face.FaceInfo, error) {
	return DetectFace(d, img)
}
