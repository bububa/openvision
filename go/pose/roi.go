package pose

/*
#include <stdlib.h>
#include <stdbool.h>
#include "openvision/pose/common.h"
*/
import "C"
import (
	"unsafe"

	"github.com/bububa/openvision/go/common"
)

// ROI represents detected person roi
type ROI struct {
	// Score detected score
	Score float32
	// Rect roi location
	Rect common.Rectangle
	// Keypoints
	Keypoints []Keypoint
}

// NewROIVector returns *C.PoseROIVector
func NewCROIVector() *C.PoseROIVector {
	return (*C.PoseROIVector)(C.malloc(C.sizeof_PoseROIVector))
}

// FreeCROIVector release *C.PoseROIVectore memory
func FreeCROIVector(p *C.PoseROIVector) {
	C.FreePoseROIVector(p)
	C.free(unsafe.Pointer(p))
}

func CROIVectiorLength(c *C.PoseROIVector) int {
	return int(c.length)
}
