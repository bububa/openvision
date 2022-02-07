package common

/*
#include <stdlib.h>
#include <stdbool.h>
#include "openvision/common/common.h"
#include "openvision/hand/pose3d.h"
*/
import "C"
import (
	"unsafe"
)

// PalmObject
type PalmObject struct {
	Name       string
	Score      float64
	Rotation   float64
	RectPoints []Point
	Landmarks  []Point
	Skeleton   []Point
	Skeleton3d []Point3d
}

// NewCPalmObjectVector returns *C.PalmObjectVector
func NewCPalmObjectVector() *C.PalmObjectVector {
	return (*C.PalmObjectVector)(C.malloc(C.sizeof_PalmObjectVector))
}

// FreeCPalmObjectVector release *C.PalmObjectVector memory
func FreeCPalmObjectVector(p *C.PalmObjectVector) {
	C.FreePalmObjectVector(p)
	C.free(unsafe.Pointer(p))
}

// GoPalmObject convert C.PalmObject to Go type
func GoPalmObject(cObj *C.PalmObject, w float64, h float64) PalmObject {
	return PalmObject{
		Score:      float64(cObj.score),
		Rotation:   float64(cObj.rotation),
		RectPoints: GoPoint2fVector(cObj.rect, w, h),
		Landmarks:  GoPoint2fVector(cObj.landmarks, w, h),
		Skeleton:   GoPoint2fVector(cObj.skeleton, w, h),
		Skeleton3d: GoPoint3dVector(cObj.skeleton3d),
	}
}

func GoPalmObjectVector(c *C.PalmObjectVector, w float64, h float64) []PalmObject {
	l := int(c.length)
	ret := make([]PalmObject, 0, l)
	ptr := unsafe.Pointer(c.items)
	for i := 0; i < l; i++ {
		cObj := (*C.PalmObject)(unsafe.Pointer(uintptr(ptr) + uintptr(C.sizeof_PalmObject*C.int(i))))
		ret = append(ret, GoPalmObject(cObj, w, h))
	}
	return ret
}
