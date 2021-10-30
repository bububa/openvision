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

// ObjectInfo represents detected roi object info
type ObjectInfo struct {
	// Score detected score
	Score float32
	// Label
	Label int
	// Rect roi location
	Rect Rectangle
	// Points keypoints
	Keypoints []Keypoint
}

// GoObjectInfo convert C.ObjectInfo to go type
func GoObjectInfo(c *C.ObjectInfo, w float64, h float64) ObjectInfo {
	ret := ObjectInfo{
		Label: int(c.label),
		Score: float32(c.score),
		Rect: Rect(
			float64(c.rect.x)/w,
			float64(c.rect.y)/h,
			float64(c.rect.width)/w,
			float64(c.rect.height)/h,
		),
	}
	if c.pts != nil {
		ret.Keypoints = GoKeypointVector(c.pts, w, h)
	}
	return ret
}

// ToCObjectInfo returns ObjectInfo C type
func (o ObjectInfo) ToCObjectInfo(w float64, h float64) *C.ObjectInfo {
	ret := (*C.ObjectInfo)(C.malloc(C.sizeof_ObjectInfo))
	ret.label = C.int(o.Label)
	ret.score = C.float(o.Score)
	ret.rect.x = C.int(o.Rect.X * w)
	ret.rect.y = C.int(o.Rect.Y * h)
	ret.rect.width = C.int(o.Rect.Width * w)
	ret.rect.height = C.int(o.Rect.Height * h)
	if len(o.Keypoints) > 0 {
		ret.pts = (*C.KeypointVector)(C.malloc(C.sizeof_KeypointVector))
		ret.pts.length = C.int(len(o.Keypoints))
		ret.pts.points = (*C.Keypoint)(C.malloc(C.sizeof_Keypoint))
		for idx, p := range o.Keypoints {
			pt := C.Keypoint{
				C.Point2f{C.float(p.Point.X * w), C.float(p.Point.Y * h)},
				C.float(p.Score),
			}
			C.KeypointVectorSetValue(ret.pts, C.int(idx), &pt)
		}
	}
	return ret
}

// NewCObjectInfoector returns *C.ObjectInfoVector
func NewCObjectInfoVector() *C.ObjectInfoVector {
	return (*C.ObjectInfoVector)(C.malloc(C.sizeof_ObjectInfoVector))
}

// FreeCObjectInfoVector release *C.ObjectInfoVector memory
func FreeCObjectInfoVector(p *C.ObjectInfoVector) {
	C.FreeObjectInfoVector(p)
	C.free(unsafe.Pointer(p))
}

// GoObjectInfoVector convert *C.ObjectInfoVector to ROI slice
func GoObjectInfoVector(c *C.ObjectInfoVector, w float64, h float64) []ObjectInfo {
	l := int(c.length)
	ret := make([]ObjectInfo, 0, l)
	ptr := unsafe.Pointer(c.items)
	for i := 0; i < l; i++ {
		cVal := (*C.ObjectInfo)(unsafe.Pointer(uintptr(ptr) + uintptr(C.sizeof_ObjectInfo*C.int(i))))
		ret = append(ret, GoObjectInfo(cVal, w, h))
	}
	return ret
}

// CObjectInfoVectiorLength get C.ObjectInfoVector length
func CObjectInfoVectiorLength(c *C.ObjectInfoVector) int {
	return int(c.length)
}

// CObjectInfoVectorPtr get C.ObjectInfoVector start pointer
func CObjectInfoVectorPtr(c *C.ObjectInfoVector) unsafe.Pointer {
	return unsafe.Pointer(c.items)
}
