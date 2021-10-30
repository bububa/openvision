package landmarker

/*
#include <stdlib.h>
#include <stdbool.h>
#include "openvision/common/common.h"
#include "openvision/face/landmarker.h"
*/
import "C"
import (
	"unsafe"

	openvision "github.com/bububa/openvision/go"
	"github.com/bububa/openvision/go/common"
)

// Landmarker represents landmarker interface
type Landmarker interface {
	common.Estimator
	ExtractKeypoints(img *common.Image, face common.Rectangle) ([]common.Point, error)
}

// ExtractKeypoints extract keypoints using landmarker
func ExtractKeypoints(d Landmarker, img *common.Image, faceRect common.Rectangle) ([]common.Point, error) {
	imgWidth := img.WidthF64()
	imgHeight := img.HeightF64()
	data := img.Bytes()
	CPoints := common.NewCPoint2fVector()
	defer common.FreeCPoint2fVector(CPoints)
	CRect := faceRect.CRect(imgWidth, imgHeight)
	errCode := C.extract_face_keypoints(
		(C.IFaceLandmarker)(d.Pointer()),
		(*C.uchar)(unsafe.Pointer(&data[0])),
		C.int(imgWidth), C.int(imgHeight),
		(*C.Rect)(unsafe.Pointer(CRect)),
		(*C.Point2fVector)(unsafe.Pointer(CPoints)),
	)
	C.free(unsafe.Pointer(CRect))
	if errCode != 0 {
		return nil, openvision.FaceLandmarkError(int(errCode))
	}
	return common.GoPoint2fVector(CPoints, imgWidth, imgHeight), nil
}
