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

// Anticonv represents anticonv detecter
type Anticonv struct {
	d C.IDetecter
}

// NewAnticonv returns a new Anticonv
func NewAnticonv() *Anticonv {
	return &Anticonv{
		d: C.new_anticonv(),
	}
}

// Destroy free detecter
func (d *Anticonv) Destroy() {
	Destroy(d)
}

// Handler returns C.IDetecter
func (d *Anticonv) Handler() C.IDetecter {
	return d.d
}

// LoadModel load model for detecter
func (d *Anticonv) LoadModel(modelPath string) error {
	return LoadModel(d, modelPath)
}

// DetectFace implement Detecter interface
func (d *Anticonv) DetectFace(img *common.Image) ([]face.FaceInfo, error) {
	return DetectFace(d, img)
}
