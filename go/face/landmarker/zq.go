package landmarker

/*
#include <stdlib.h>
#include <stdbool.h>
#include "openvision/face/landmarker.h"
*/
import "C"
import "github.com/bububa/openvision/go/common"

// Zq represents Zq landmarker
type Zq struct {
	d C.IFaceLandmarker
}

// NewZq returns a new Zq landmarker
func NewZq() *Zq {
	return &Zq{
		d: C.new_zq(),
	}
}

// Handler returns C.ILandmarker
func (d *Zq) Handler() C.IFaceLandmarker {
	return d.d
}

// LoadModel implement Landmarker interface
func (d *Zq) LoadModel(modelPath string) error {
	return LoadModel(d, modelPath)
}

// Destroy implement Landmarker interface
func (d *Zq) Destroy() {
	Destroy(d)
}

// ExtractKeypoints implement Landmarker interface
func (d *Zq) ExtractKeypoints(img *common.Image, faceRect common.Rectangle) ([]common.Point, error) {
	return ExtractKeypoints(d, img, faceRect)
}
