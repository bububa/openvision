package detecter

/*
#include <stdlib.h>
#include <stdbool.h>
#include "openvision/common/common.h"
#include "openvision/pose/detecter.h"
*/
import "C"
import (
	"unsafe"

	openvision "github.com/bububa/openvision/go"
	"github.com/bububa/openvision/go/common"
)

// Detecter represents deteter interface
type Detecter interface {
	common.Estimator
	HasKeypoints() bool
	Detect(img *common.Image) ([]common.ObjectInfo, error)
}

// Detect detect pose rect using detecter
func Detect(d Detecter, img *common.Image) ([]common.ObjectInfo, error) {
	imgWidth := img.WidthF64()
	imgHeight := img.HeightF64()
	data := img.Bytes()
	cObjs := common.NewCObjectInfoVector()
	defer common.FreeCObjectInfoVector(cObjs)
	errCode := C.detect_pose(
		(C.IPoseDetecter)(d.Pointer()),
		(*C.uchar)(unsafe.Pointer(&data[0])),
		C.int(imgWidth),
		C.int(imgHeight),
		(*C.ObjectInfoVector)(unsafe.Pointer(cObjs)))
	if errCode != 0 {
		return nil, openvision.DetectPoseError(int(errCode))
	}
	return common.GoObjectInfoVector(cObjs, imgWidth, imgHeight), nil
}
