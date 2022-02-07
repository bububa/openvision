package aligner

/*
#include <stdlib.h>
#include <stdbool.h>
#include "openvision/face/common.h"
#include "openvision/face/aligner.h"
*/
import "C"
import (
	"unsafe"

	openvision "github.com/bububa/openvision/go"
	"github.com/bububa/openvision/go/common"
	face "github.com/bububa/openvision/go/face"
)

// Aligner represents Aligner
type Aligner struct {
	d C.IFaceAligner
}

// NewAligner returns a new Aligner
func NewAligner() *Aligner {
	return &Aligner{
		d: C.new_face_aligner(),
	}
}

// Destroy destroy C.IFaceAligner
func (a *Aligner) Destroy() {
	C.destroy_face_aligner(a.d)
}

// SetThreads set num_threads
func (a *Aligner) SetThreads(n int) {
	C.set_face_aligner_threads(a.d, C.int(n))
}

// Align face
func (a *Aligner) Align(img *common.Image, faceInfo face.FaceInfo, out *common.Image) error {
	imgWidth := img.WidthF64()
	imgHeight := img.HeightF64()
	data := img.Bytes()
	outImgC := common.NewCImage()
	defer common.FreeCImage(outImgC)
	var cPts [10]C.float
	for idx, pt := range faceInfo.Keypoints {
		cPts[idx] = C.float(pt.X * imgWidth)
		cPts[idx+5] = C.float(pt.Y * imgHeight)
	}
	cRect := faceInfo.Rect.CRect(imgWidth, imgHeight)
	defer C.free(unsafe.Pointer(cRect))
	errCode := C.align_face(
		a.d,
		(*C.uchar)(unsafe.Pointer(&data[0])),
		C.int(imgWidth), C.int(imgHeight),
		(*C.Rect)(unsafe.Pointer(cRect)),
		(*C.float)(unsafe.Pointer(&cPts)),
		(*C.Image)(unsafe.Pointer(outImgC)),
	)
	if errCode != 0 {
		return openvision.AlignFaceError(int(errCode))
	}
	common.GoImage(outImgC, out)
	return nil
}
