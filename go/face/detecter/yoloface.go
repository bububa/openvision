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

// YoloFace represents yoloface detecter
type YoloFace struct {
	d C.IFaceDetecter
}

// NewYoloFace returns a new YoloFace
func NewYoloFace() *YoloFace {
	return &YoloFace{
		d: C.new_yoloface(),
	}
}

// Destroy free detecter
func (d *YoloFace) Destroy() {
	Destroy(d)
}

// Handler returns C.IFaceDetecter
func (d *YoloFace) Handler() C.IFaceDetecter {
	return d.d
}

// LoadModel implement Detecter interface
func (d *YoloFace) LoadModel(modelPath string) error {
	return LoadModel(d, modelPath)
}

// DetectFace implement Detecter interface
func (d *YoloFace) DetectFace(img *common.Image) ([]face.FaceInfo, error) {
	return DetectFace(d, img)
}
