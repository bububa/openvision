package detecter

/*
#include <stdlib.h>
#include <stdbool.h>
#include "openvision/face/common.h"
#include "openvision/face/detecter.h"
*/
import "C"
import (
	"unsafe"

	openvision "github.com/bububa/openvision/go"
	"github.com/bububa/openvision/go/common"
	"github.com/bububa/openvision/go/face"
)

// Detecter represents deteter interface
type Detecter interface {
	common.Estimator
	Detect(img *common.Image) ([]face.FaceInfo, error)
}

// Detect detect face useing detecter
func Detect(d Detecter, img *common.Image) ([]face.FaceInfo, error) {
	imgWidth := img.WidthF64()
	imgHeight := img.HeightF64()
	data := img.Bytes()
	CFaces := face.NewCFaceInfoVector()
	defer face.FreeCFaceInfoVector(CFaces)
	errCode := C.detect_face((C.IFaceDetecter)(d.Pointer()), (*C.uchar)(unsafe.Pointer(&data[0])), C.int(imgWidth), C.int(imgHeight), (*C.FaceInfoVector)(unsafe.Pointer(CFaces)))
	if errCode != 0 {
		return nil, openvision.DetectFaceError(int(errCode))
	}
	faces := face.GoFaceInfoVector(CFaces, imgWidth, imgHeight)
	return faces, nil
}
