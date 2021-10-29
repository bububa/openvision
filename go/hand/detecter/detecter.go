package detecter

/*
#include <stdlib.h>
#include <stdbool.h>
#include "openvision/common/common.h"
#include "openvision/hand/detecter.h"
*/
import "C"
import (
	"unsafe"

	openvision "github.com/bububa/openvision/go"
	"github.com/bububa/openvision/go/common"
)

// Detecter represents deteter interface
type Detecter interface {
	Handler() C.IHandDetecter
	LoadModel(modelPath string) error
	Detect(img *common.Image) ([]common.ObjectInfo, error)
	Destroy()
}

// LoadModel load detecter model
func LoadModel(d Detecter, modelPath string) error {
	cpath := C.CString(modelPath)
	defer C.free(unsafe.Pointer(cpath))
	retCode := C.load_model((C.IEstimator)(unsafe.Pointer(d.Handler())), cpath)
	if retCode != 0 {
		return openvision.LoadModelError(int(retCode))
	}
	return nil
}

// Destroy a detecter
func Destroy(d Detecter) {
	C.destroy_estimator((C.IEstimator)(unsafe.Pointer(d.Handler())))
}

// Detect detect hand ROI
func Detect(d Detecter, img *common.Image) ([]common.ObjectInfo, error) {
	imgWidth := img.WidthF64()
	imgHeight := img.HeightF64()
	data := img.Bytes()
	cObjs := common.NewCObjectInfoVector()
	defer common.FreeCObjectInfoVector(cObjs)
	errCode := C.extract_hand_rois(
		d.Handler(),
		(*C.uchar)(unsafe.Pointer(&data[0])),
		C.int(imgWidth),
		C.int(imgHeight),
		(*C.ObjectInfoVector)(unsafe.Pointer(cObjs)))
	if errCode != 0 {
		return nil, openvision.DetectHandError(int(errCode))
	}
	return common.GoObjectInfoVector(cObjs, imgWidth, imgHeight), nil
}
