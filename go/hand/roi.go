package hand

/*
#include <stdlib.h>
#include <stdbool.h>
#include "openvision/hand/common.h"
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
	// Label
	Label int
	// Rect roi location
	Rect common.Rectangle
}

// GoROI convert C.HandROI to go type
func GoROI(c *C.HandROI, w float64, h float64) ROI {
	return ROI{
		Label: int(c.label),
		Score: float32(c.prob),
		Rect: common.Rect(
			float64(c.rect.x)/w,
			float64(c.rect.y)/h,
			float64(c.rect.width)/w,
			float64(c.rect.height)/h,
		),
	}
}

// ToCROI returns ROI C type
func (r ROI) ToCROI(w float64, h float64) *C.HandROI {
	ret := (*C.HandROI)(C.malloc(C.sizeof_HandROI))
	ret.label = C.int(r.Label)
	ret.prob = C.float(r.Score)
	ret.rect.x = C.int(r.Rect.X * w)
	ret.rect.y = C.int(r.Rect.Y * h)
	ret.rect.width = C.int(r.Rect.Width * w)
	ret.rect.height = C.int(r.Rect.Height * h)
	return ret
}

// NewROIVector returns *C.HandROIVector
func NewCROIVector() *C.HandROIVector {
	return (*C.HandROIVector)(C.malloc(C.sizeof_HandROIVector))
}

// FreeCROIVector release *C.HandROIVectore memory
func FreeCROIVector(p *C.HandROIVector) {
	C.FreeHandROIVector(p)
	C.free(unsafe.Pointer(p))
}

// GoROIVector convert *C.HandROIVector to ROI slice
func GoROIVector(c *C.HandROIVector, w float64, h float64) []ROI {
	l := int(c.length)
	ret := make([]ROI, 0, l)
	ptr := unsafe.Pointer(c.items)
	for i := 0; i < l; i++ {
		cVal := (*C.HandROI)(unsafe.Pointer(uintptr(ptr) + uintptr(C.sizeof_HandROI*C.int(i))))
		ret = append(ret, GoROI(cVal, w, h))
	}
	return ret
}

// CROIVectiorLength get C.HandROIVector length
func CROIVectiorLength(c *C.HandROIVector) int {
	return int(c.length)
}

// CROIVectorPtr get C.HandROIVector start pointer
func CROIVectorPtr(c *C.HandROIVector) unsafe.Pointer {
	return unsafe.Pointer(c.items)
}
