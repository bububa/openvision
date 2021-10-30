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

// Scrfd represents Scrfd landmarker
type Scrfd struct {
	d C.IFaceLandmarker
}

// NewScrfd returns a new Scrfd landmarker
func NewScrfd() *Scrfd {
	return &Scrfd{
		d: C.new_scrfd_landmarker(),
	}
}

// Pointer implement Estimator interface
func (d *Scrfd) Pointer() unsafe.Pointer {
	return unsafe.Pointer(d.d)
}

// LoadModel implement Landmarker interface
func (d *Scrfd) LoadModel(modelPath string) error {
	return common.EstimatorLoadModel(d, modelPath)
}

// Destroy implement Landmarker interface
func (d *Scrfd) Destroy() {
	common.DestroyEstimator(d)
}

// ExtractKeypoints implement Landmarker interface
func (d *Scrfd) ExtractKeypoints(img *common.Image, faceRect common.Rectangle) ([]common.Point, error) {
	return ExtractKeypoints(d, img, faceRect)
}
