package face

/*
#include <stdlib.h>
#include <stdbool.h>
#include "openvision/common/common.h"
*/
import "C"
import (
	"unsafe"
)

// TrackedFaceInfo represents detected tracked face info
type TrackedFaceInfo struct {
	Face     FaceInfo
	IOUScore float64
}

// GoTrackedFaceInfo convert c TrackedFaceInfo to go type
func GoTrackedFaceInfo(cInfo *C.TrackedFaceInfo, w float64, h float64) TrackedFaceInfo {
	return TrackedFaceInfo{
		Face:     GoFaceInfo(&cInfo.face_info_, w, h),
		IOUScore: float64(cInfo.iou_score_),
	}
}

// NewCTrackedFaceInfoVector returns C.TrackedFaceInfoVector pointer
func NewCTrackedFaceInfoVector() *C.TrackedFaceInfoVector {
	return (*C.TrackedFaceInfoVector)(C.malloc(C.sizeof_TrackedFaceInfo))
}

// GoTrackedFaceInfoVector conver c TrackedFaceInfoVector to go TrackedFaceInfo slice
func GoTrackedFaceInfoVector(cVector *C.TrackedFaceInfoVector, w float64, h float64) []TrackedFaceInfo {
	l := int(cVector.length)
	ret := make([]TrackedFaceInfo, 0, l)
	ptr := unsafe.Pointer(cVector.faces)
	for i := 0; i < l; i++ {
		cFace := (*C.TrackedFaceInfo)(unsafe.Pointer(uintptr(ptr) + uintptr(C.sizeof_TrackedFaceInfo*C.int(i))))
		ret = append(ret, GoTrackedFaceInfo(cFace, w, h))
	}
	return ret
}

// FreeCTrackedFaceInfoVector release CTrackedFaceInfoVector memory
func FreeCTrackedFaceInfoVector(faces *C.TrackedFaceInfoVector) {
	C.FreeTrackedFaceInfoVector(faces)
	C.free(unsafe.Pointer(faces))
}
