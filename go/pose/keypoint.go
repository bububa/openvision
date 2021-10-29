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

// Keypoint represents detected body keypoint
type Keypoint struct {
	// Point keypoint location
	Point common.Point
	// Score keypoint prob
	Score float32
}

// GoKeypoint convert C.PoseKeypoint to go type
func GoKeypoint(c *C.PoseKeypoint, w float64, h float64) Keypoint {
	return Keypoint{
		Point: common.Pt(float64(c.p.x)/w, float64(c.p.y)/h),
		Score: float32(c.prob),
	}
}

// Convert Keypoint to C.Keypoint pointer
func (k Keypoint) CKeypoint(w float64, h float64) *C.PoseKeypoint {
	ret := (*C.PoseKeypoint)(C.malloc(C.sizeof_PoseKeypoint))
	ret.prob = C.float(k.Score)
	ret.p = C.Point2f{
		C.float(k.Point.X * w),
		C.float(k.Point.Y * h),
	}
	return ret
}

// NewCKeypointVector returns *C.PoseKeypointVector
func NewCKeypointVector() *C.PoseKeypointVector {
	return (*C.PoseKeypointVector)(C.malloc(C.sizeof_PoseKeypointVector))
}

// FreeCKeypointVector release *C.PoseKeypointVector memory
func FreeCKeypointVector(points *C.PoseKeypointVector) {
	C.FreePoseKeypointVector(points)
	C.free(unsafe.Pointer(points))
}

// GoKeypointVector convert *C.PoseKeypointVector to Keypoint slice
func GoKeypointVector(c *C.PoseKeypointVector, w float64, h float64) []Keypoint {
	l := int(c.length)
	ret := make([]Keypoint, 0, l)
	ptr := unsafe.Pointer(c.points)
	for i := 0; i < l; i++ {
		cKeypoint := (*C.PoseKeypoint)(unsafe.Pointer(uintptr(ptr) + uintptr(C.sizeof_PoseKeypoint*C.int(i))))
		ret = append(ret, GoKeypoint(cKeypoint, w, h))
	}
	return ret
}
