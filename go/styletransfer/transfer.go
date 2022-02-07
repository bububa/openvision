package styletransfer

/*
#include <stdlib.h>
#include <stdbool.h>
#include "openvision/common/common.h"
#include "openvision/styletransfer/styletransfer.h"
*/
import "C"
import (
	"unsafe"

	openvision "github.com/bububa/openvision/go"
	"github.com/bububa/openvision/go/common"
)

// StyleTransfer represents Style Transfer interface
type StyleTransfer interface {
	common.Estimator
	Transform(img *common.Image, out *common.Image) error
}

// Transform returns style transform image
func Transform(d StyleTransfer, img *common.Image, out *common.Image) error {
	imgWidth := img.WidthF64()
	imgHeight := img.HeightF64()
	data := img.Bytes()
	outImgC := common.NewCImage()
	defer common.FreeCImage(outImgC)
	errCode := C.style_transform(
		(C.IStyleTransfer)(d.Pointer()),
		(*C.uchar)(unsafe.Pointer(&data[0])),
		C.int(imgWidth),
		C.int(imgHeight),
		(*C.Image)(unsafe.Pointer(outImgC)))
	if errCode != 0 {
		return openvision.DetectPoseError(int(errCode))
	}
	common.GoImage(outImgC, out)
	return nil
}
