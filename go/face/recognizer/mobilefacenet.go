package recognizer

/*
#include <stdlib.h>
#include <stdbool.h>
#include "openvision/face/recognizer.h"
*/
import "C"
import "github.com/bububa/openvision/go/common"

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

// Handler returns C.IFaceRecognizer
func (d *Mobilefacenet) Handler() C.IFaceRecognizer {
	return d.d
}

// LoadModel implement Recognizer interface
func (d *Mobilefacenet) LoadModel(modelPath string) error {
	return LoadModel(d, modelPath)
}

// Destroy implement Recognizer interface
func (d *Mobilefacenet) Destroy() {
	Destroy(d)
}

// ExtractFeatures implement Recognizer interface
func (d *Mobilefacenet) ExtractFeatures(img *common.Image, faceRect common.Rectangle) ([]float64, error) {
	return ExtractFeatures(d, img, faceRect)
}
