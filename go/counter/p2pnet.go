package counter

/*
#include <stdlib.h>
#include <stdbool.h>
#include "openvision/counter/counter.h"
*/
import "C"
import (
	"unsafe"

	"github.com/bububa/openvision/go/common"
)

// P2PNet represents p2pnet counter
type P2PNet struct {
	d C.ICounter
}

// NewP2PNet returns a new P2PNet
func NewP2PNet() *P2PNet {
	return &P2PNet{
		d: C.new_p2pnet_crowd_counter(),
	}
}

// Destroy free tracker
func (d *P2PNet) Destroy() {
	common.DestroyEstimator(d)
}

// Pointer implement Estimator interface
func (d *P2PNet) Pointer() unsafe.Pointer {
	return unsafe.Pointer(d.d)
}

// LoadModel load model for detecter
func (d *P2PNet) LoadModel(modelPath string) error {
	return common.EstimatorLoadModel(d, modelPath)
}

// CrowdCount implement Object Counter interface
func (d *P2PNet) CrowdCount(img *common.Image) ([]common.Keypoint, error) {
	return CrowdCount(d, img)
}
