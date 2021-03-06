package recognizer

/*
#include <stdlib.h>
#include <stdbool.h>
#include "openvision/face/recognizer.h"
*/
import "C"
import (
	"unsafe"

	"github.com/bububa/openvision/go/common"
)

// Mobilefacenet represents Mobilefacenet recognizer
type Mobilefacenet struct {
	d C.IFaceRecognizer
}

// NewMobilefacenet returns a new Mobilefacenet recognizer
func NewMobilefacenet() *Mobilefacenet {
	return &Mobilefacenet{
		d: C.new_mobilefacenet(),
	}
}

// Pointer implement Estimator interface
func (d *Mobilefacenet) Pointer() unsafe.Pointer {
	return unsafe.Pointer(d.d)
}

// LoadModel implement Recognizer interface
func (d *Mobilefacenet) LoadModel(modelPath string) error {
	return common.EstimatorLoadModel(d, modelPath)
}

// Destroy implement Recognizer interface
func (d *Mobilefacenet) Destroy() {
	common.DestroyEstimator(d)
}

// ExtractFeatures implement Recognizer interface
func (d *Mobilefacenet) ExtractFeatures(img *common.Image, faceRect common.Rectangle) ([]float64, error) {
	return ExtractFeatures(d, img, faceRect)
}
