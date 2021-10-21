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

// RetinaFace represents retinaface detecter
type RetinaFace struct {
	d C.IDetecter
}

// NewRetinaFace returns a new RetinaFace
func NewRetinaFace() *RetinaFace {
	return &RetinaFace{
		d: C.new_retinaface(),
	}
}

// Destroy free detecter
func (d *RetinaFace) Destroy() {
	C.destroy_detecter(d.d)
}

func (d *RetinaFace) Handler() C.IDetecter {
	return d.d
}

func (d *RetinaFace) LoadModel(modelPath string) error {
	return LoadModel(d, modelPath)
}

func (d *RetinaFace) DetectFace(img *common.Image) ([]face.FaceInfo, error) {
	return DetectFace(d, img)
}
