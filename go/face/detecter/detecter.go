package detecter

/*
#include <stdlib.h>
#include <stdbool.h>
#include "openvision/common/common.h"
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
	Handler() C.IDetecter
	LoadModel(modelPath string) error
	DetectFace(img *common.Image) ([]face.FaceInfo, error)
	Destroy()
}

// LoadModel load detecter model
func LoadModel(d Detecter, modelPath string) error {
	cpath := C.CString(modelPath)
	defer C.free(unsafe.Pointer(cpath))
	retCode := C.detecter_load_model(d.Handler(), cpath)
	if retCode != 0 {
		return openvision.LoadModelError(int(retCode))
	}
	return nil
}

// Destroy a detecter
func Destroy(d Detecter) {
	C.destroy_detecter(d.Handler())
}

// DetectFace detect face useing detecter
func DetectFace(d Detecter, img *common.Image) ([]face.FaceInfo, error) {
	imgWidth := img.WidthF64()
	imgHeight := img.HeightF64()
	data := img.Bytes()
	CFaces := face.NewCFaceInfoVector()
	defer face.FreeCFaceInfoVector(CFaces)
	errCode := C.detect_face(d.Handler(), (*C.uchar)(unsafe.Pointer(&data[0])), C.int(imgWidth), C.int(imgHeight), (*C.FaceInfoVector)(unsafe.Pointer(CFaces)))
	if errCode != 0 {
		return nil, openvision.DetectFaceError(int(errCode))
	}
	faces := face.GoFaceInfoVector(CFaces, imgWidth, imgHeight)
	return faces, nil
}
