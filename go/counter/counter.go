package counter

/*
#include <stdlib.h>
#include <stdbool.h>
#include "openvision/common/common.h"
#include "openvision/counter/counter.h"
*/
import "C"
import (
	"unsafe"

	openvision "github.com/bububa/openvision/go"
	"github.com/bububa/openvision/go/common"
)

// Counter represents Object Counter interface
type Counter interface {
	common.Estimator
	CrowdCount(img *common.Image) ([]common.Keypoint, error)
}

// CrowdCount returns object counter
func CrowdCount(d Counter, img *common.Image) ([]common.Keypoint, error) {
	imgWidth := img.WidthF64()
	imgHeight := img.HeightF64()
	data := img.Bytes()
	ptsC := common.NewCKeypointVector()
	defer common.FreeCKeypointVector(ptsC)
	errCode := C.crowd_count(
		(C.ICounter)(d.Pointer()),
		(*C.uchar)(unsafe.Pointer(&data[0])),
		C.int(imgWidth),
		C.int(imgHeight),
		(*C.KeypointVector)(unsafe.Pointer(ptsC)))
	if errCode != 0 {
		return nil, openvision.CounterError(int(errCode))
	}
	return common.GoKeypointVector(ptsC, imgWidth, imgHeight), nil
}
