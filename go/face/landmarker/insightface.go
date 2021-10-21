package landmarker

/*
#include <stdlib.h>
#include <stdbool.h>
#include "openvision/face/landmarker.h"
*/
import "C"
import "github.com/bububa/openvision/go/common"

// Insightface represents Insightface landmarker
type Insightface struct {
	d C.ILandmarker
}

// NewInsightface returns a new Insightface landmarker
func NewInsightface() *Insightface {
	return &Insightface{
		d: C.new_insightface(),
	}
}

// Handler returns C.ILandmarker
func (d *Insightface) Handler() C.ILandmarker {
	return d.d
}

// LoadModel implement Landmarker interface
func (d *Insightface) LoadModel(modelPath string) error {
	return LoadModel(d, modelPath)
}

// Destroy implement Landmarker interface
func (d *Insightface) Destroy() {
	Destroy(d)
}

// ExtractKeypoints implement Landmarker interface
func (d *Insightface) ExtractKeypoints(img *common.Image, faceRect common.Rectangle) ([]common.Point, error) {
	return ExtractKeypoints(d, img, faceRect)
}
