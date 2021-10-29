package common

/*
#include <stdlib.h>
#include <stdbool.h>
#include "openvision/common/common.h"
*/
import "C"
import (
	"unsafe"
)

// Keypoint represents detected body keypoint
type Keypoint struct {
	// Point keypoint location
	Point Point
	// Score keypoint prob
	Score float32
}

// GoKeypoint convert C.Keypoint to go type
func GoKeypoint(c *C.Keypoint, w float64, h float64) Keypoint {
	return Keypoint{
		Point: Pt(float64(c.p.x)/w, float64(c.p.y)/h),
		Score: float32(c.prob),
	}
}

// Convert Keypoint to C.Keypoint pointer
func (k Keypoint) CKeypoint(w float64, h float64) *C.Keypoint {
	ret := (*C.Keypoint)(C.malloc(C.sizeof_Keypoint))
	ret.prob = C.float(k.Score)
	ret.p = C.Point2f{
		C.float(k.Point.X * w),
		C.float(k.Point.Y * h),
	}
	return ret
}

// NewCKeypointVector returns *C.KeypointVector
func NewCKeypointVector() *C.KeypointVector {
	return (*C.KeypointVector)(C.malloc(C.sizeof_KeypointVector))
}

// FreeCKeypointVector release *C.KeypointVector memory
func FreeCKeypointVector(points *C.KeypointVector) {
	C.FreeKeypointVector(points)
	C.free(unsafe.Pointer(points))
}

// GoKeypointVector convert *C.KeypointVector to Keypoint slice
func GoKeypointVector(c *C.KeypointVector, w float64, h float64) []Keypoint {
	l := int(c.length)
	ret := make([]Keypoint, 0, l)
	ptr := unsafe.Pointer(c.points)
	for i := 0; i < l; i++ {
		cKeypoint := (*C.Keypoint)(unsafe.Pointer(uintptr(ptr) + uintptr(C.sizeof_Keypoint*C.int(i))))
		ret = append(ret, GoKeypoint(cKeypoint, w, h))
	}
	return ret
}
