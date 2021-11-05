package segmentor

/*
#include <stdlib.h>
#include <stdbool.h>
#include "openvision/common/common.h"
#include "openvision/pose/segmentor.h"
*/
import "C"
import (
	"image"
	"unsafe"

	openvision "github.com/bububa/openvision/go"
	"github.com/bububa/openvision/go/common"
)

// Segmentor represents segmentor interface
type Segmentor interface {
	common.Estimator
	Matting(img *common.Image) (image.Image, error)
	Merge(img *common.Image, bg *common.Image) (image.Image, error)
}

// Matting returns pose segment matting image
func Matting(d Segmentor, img *common.Image) (image.Image, error) {
	imgWidth := img.WidthF64()
	imgHeight := img.HeightF64()
	data := img.Bytes()
	outImgC := common.NewCImage()
	defer common.FreeCImage(outImgC)
	errCode := C.pose_segment_matting(
		(C.IPoseSegmentor)(d.Pointer()),
		(*C.uchar)(unsafe.Pointer(&data[0])),
		C.int(imgWidth),
		C.int(imgHeight),
		(*C.Image)(unsafe.Pointer(outImgC)))
	if errCode != 0 {
		return nil, openvision.DetectPoseError(int(errCode))
	}
	return common.GoImage(outImgC)
}

// Merge merge pose with background
func Merge(d Segmentor, img *common.Image, bg *common.Image) (image.Image, error) {
	imgWidth := img.WidthF64()
	imgHeight := img.HeightF64()
	data := img.Bytes()
	bgWidth := bg.Width()
	bgHeight := bg.Height()
	bgdata := bg.Bytes()
	outImgC := common.NewCImage()
	defer common.FreeCImage(outImgC)
	errCode := C.pose_segment_merge(
		(C.IPoseSegmentor)(d.Pointer()),
		(*C.uchar)(unsafe.Pointer(&data[0])),
		C.int(imgWidth), C.int(imgHeight),
		(*C.uchar)(unsafe.Pointer(&bgdata[0])),
		C.int(bgWidth), C.int(bgHeight),
		(*C.Image)(unsafe.Pointer(outImgC)))
	if errCode != 0 {
		return nil, openvision.DetectPoseError(int(errCode))
	}
	return common.GoImage(outImgC)
}
