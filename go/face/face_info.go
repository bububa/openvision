package face

/*
#include <stdlib.h>
#include <stdbool.h>
#include "openvision/face/common.h"
*/
import "C"
import (
	"unsafe"

	"github.com/bububa/openvision/go/common"
)

// FaceInfo represents detected face info
type FaceInfo struct {
	// Rect face location
	Rect common.Rectangle
	// Score detected score
	Score float32
	// Keypoints .
	Keypoints [5]common.Point
	// Mask has mask or not
	Mask bool
}

// GoFaceInfo convert c FaceInfo to go type
func GoFaceInfo(cInfo *C.FaceInfo, w float64, h float64) FaceInfo {
	info := FaceInfo{
		Score: float32(cInfo.score_),
		Mask:  bool(cInfo.mask_),
		Rect: common.Rect(
			float64(cInfo.location_.x)/w,
			float64(cInfo.location_.y)/h,
			float64(cInfo.location_.width)/w,
			float64(cInfo.location_.height)/h,
		),
	}
	for i := 0; i < 5; i++ {
		info.Keypoints[i] = common.Pt(
			float64(cInfo.keypoints_[i])/w,
			float64(cInfo.keypoints_[i+5])/h,
		)
	}
	return info
}

// CFaceInfo convert FaceInfo to C.FaceInfo
func (f FaceInfo) CFaceInfo() *C.FaceInfo {
	ret := (*C.FaceInfo)(C.malloc(C.sizeof_FaceInfo))
	ret.score_ = C.float(f.Score)
	ret.mask_ = C.bool(f.Mask)
	ret.location_ = C.Rect{
		C.int(f.Rect.X),
		C.int(f.Rect.Y),
		C.int(f.Rect.Width),
		C.int(f.Rect.Height),
	}
	for i := 0; i < 5; i++ {
		ret.keypoints_[i] = C.float(f.Keypoints[i].X)
		ret.keypoints_[i+5] = C.float(f.Keypoints[i].Y)
	}
	return ret
}

// NewCFaceInfoVector returns C.FaceInfoVector pointer
func NewCFaceInfoVector() *C.FaceInfoVector {
	return (*C.FaceInfoVector)(C.malloc(C.sizeof_FaceInfo))
}

// GoFaceInfoVector conver c FaceInfoVector to go FaceInfo slice
func GoFaceInfoVector(cVector *C.FaceInfoVector, w float64, h float64) []FaceInfo {
	l := int(cVector.length)
	ret := make([]FaceInfo, 0, l)
	ptr := unsafe.Pointer(cVector.faces)
	for i := 0; i < l; i++ {
		cFace := (*C.FaceInfo)(unsafe.Pointer(uintptr(ptr) + uintptr(C.sizeof_FaceInfo*C.int(i))))
		ret = append(ret, GoFaceInfo(cFace, w, h))
	}
	return ret
}

// FreeCFaceInfoVector release CFaceInfoVector memory
func FreeCFaceInfoVector(faces *C.FaceInfoVector) {
	C.FreeFaceInfoVector(faces)
	C.free(unsafe.Pointer(faces))
}

// NewCFaceInfoVectorFromFaces returns C.FaceInfoVector pointer
func NewCFaceInfoVectorFromFaces(faces []FaceInfo) *C.FaceInfoVector {
	l := len(faces)
	vec := (*C.FaceInfoVector)(C.malloc(C.sizeof_FaceInfoVector))
	C.NewFaceInfoVector(vec, C.int(l))
	p := (*[1 << 30]C.FaceInfo)(unsafe.Pointer(vec.faces))[:l:l]
	for i := 0; i < l; i++ {
		face := faces[i].CFaceInfo()
		defer C.free(unsafe.Pointer(face))
		p[i] = *face
	}
	return vec
}
