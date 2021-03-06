package landmarker

/*
#include <stdlib.h>
#include <stdbool.h>
#include "openvision/face/landmarker.h"
*/
import "C"
import (
	"unsafe"

	"github.com/bububa/openvision/go/common"
)

// Insightface represents Insightface landmarker
type Insightface struct {
	d C.IFaceLandmarker
}

// NewInsightface returns a new Insightface landmarker
func NewInsightface() *Insightface {
	return &Insightface{
		d: C.new_insightface(),
	}
}

// Pointer implement Estimator interface
func (d *Insightface) Pointer() unsafe.Pointer {
	return unsafe.Pointer(d.d)
}

// LoadModel implement Landmarker interface
func (d *Insightface) LoadModel(modelPath string) error {
	return common.EstimatorLoadModel(d, modelPath)
}

// Destroy implement Landmarker interface
func (d *Insightface) Destroy() {
	common.DestroyEstimator(d)
}

// ExtractKeypoints implement Landmarker interface
func (d *Insightface) ExtractKeypoints(img *common.Image, faceRect common.Rectangle) ([]common.Point, error) {
	return ExtractKeypoints(d, img, faceRect)
}
