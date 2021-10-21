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

// Mtcnn represents mtcnn detecter
type Mtcnn struct {
	d C.IDetecter
}

// NewMtcnn returns a new Mtcnn
func NewMtcnn() *Mtcnn {
	return &Mtcnn{
		d: C.new_mtcnn(),
	}
}

// Destroy free detecter
func (d *Mtcnn) Destroy() {
	Destroy(d)
}

// Handler returns C.IDetecter
func (d *Mtcnn) Handler() C.IDetecter {
	return d.d
}

// LoadModel implement Detecter interface
func (d *Mtcnn) LoadModel(modelPath string) error {
	return LoadModel(d, modelPath)
}

// DetectFace implement Detecter interface
func (d *Mtcnn) DetectFace(img *common.Image) ([]face.FaceInfo, error) {
	return DetectFace(d, img)
}
