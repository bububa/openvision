package tracker

/*
#include <stdlib.h>
#include <stdbool.h>
#include "openvision/tracker/tracker.h"
*/
import "C"
import (
	"unsafe"

	"github.com/bububa/openvision/go/common"
)

// LightTrack represents light track tracker
type LightTrack struct {
	d C.ITracker
}

// NewLightTrack returns a new LightTrack
func NewLightTrack() *LightTrack {
	return &LightTrack{
		d: C.new_light_track(),
	}
}

// Destroy free tracker
func (d *LightTrack) Destroy() {
	common.DestroyEstimator(d)
}

// Pointer implement Estimator interface
func (d *LightTrack) Pointer() unsafe.Pointer {
	return unsafe.Pointer(d.d)
}

// LoadModel load model for detecter
func (d *LightTrack) LoadModel(modelPath string) error {
	return common.EstimatorLoadModel(d, modelPath)
}

// Track implement Object Tracker interface
func (d *LightTrack) Track(img *common.Image, rect common.Rectangle) (common.Rectangle, error) {
	return Track(d, img, rect)
}
