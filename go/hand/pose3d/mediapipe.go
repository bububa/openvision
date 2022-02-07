package pose3d

/*
#include <stdlib.h>
#include <stdbool.h>
#include "openvision/common/common.h"
#include "openvision/hand/pose3d.h"
*/
import "C"
import (
	"unsafe"

	openvision "github.com/bububa/openvision/go"
	"github.com/bububa/openvision/go/common"
)

// Mediapipe represents mediapipe estimator interface
type Mediapipe struct {
	d C.IHandPose3DEstimator
}

func NewMediapipe() *Mediapipe {
	return &Mediapipe{
		d: C.new_mediapipe_hand(),
	}
}

func (m *Mediapipe) Destroy() {
	C.destroy_mediapipe_hand(m.d)
}

func (m *Mediapipe) LoadModel(palmPath string, handPath string) error {
	cPalm := C.CString(palmPath)
	defer C.free(unsafe.Pointer(cPalm))
	cHand := C.CString(handPath)
	defer C.free(unsafe.Pointer(cHand))
	retCode := C.mediapipe_hand_load_model(m.d, cPalm, cHand)
	if retCode != 0 {
		return openvision.LoadModelError(int(retCode))
	}
	return nil

}

// Detect detect hand 3d pose
func (m *Mediapipe) Detect(img *common.Image) ([]common.PalmObject, error) {
	imgWidth := img.WidthF64()
	imgHeight := img.HeightF64()
	data := img.Bytes()
	cObjs := common.NewCPalmObjectVector()
	defer common.FreeCPalmObjectVector(cObjs)
	errCode := C.mediapipe_hand_detect(
		m.d,
		(*C.uchar)(unsafe.Pointer(&data[0])),
		C.int(imgWidth), C.int(imgHeight),
		(*C.PalmObjectVector)(unsafe.Pointer(cObjs)),
	)
	if errCode != 0 {
		return nil, openvision.DetectHandError(int(errCode))
	}
	return common.GoPalmObjectVector(cObjs, imgWidth, imgHeight), nil
}
