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
	Handler() C.IFaceLandmarker
	LoadModel(modelPath string) error
	ExtractKeypoints(img *common.Image, face common.Rectangle) ([]common.Point, error)
	Destroy()
}

// LoadModel load landmarker model
func LoadModel(d Landmarker, modelPath string) error {
	cpath := C.CString(modelPath)
	defer C.free(unsafe.Pointer(cpath))
	retCode := C.load_model((C.IEstimator)(unsafe.Pointer(d.Handler())), cpath)
	if retCode != 0 {
		return openvision.LoadModelError(int(retCode))
	}
	return nil
}

// Destroy a landmarker
func Destroy(d Landmarker) {
	C.destroy_estimator((C.IEstimator)(unsafe.Pointer(d.Handler())))
}

// ExtractKeypoints extract keypoints using landmarker
func ExtractKeypoints(d Landmarker, img *common.Image, faceRect common.Rectangle) ([]common.Point, error) {
	imgWidth := img.WidthF64()
	imgHeight := img.HeightF64()
	faceRect.X *= imgWidth
	faceRect.Y *= imgHeight
	faceRect.Width *= imgWidth
	faceRect.Height *= imgHeight
	data := img.Bytes()
	CPoints := common.NewCPoint2fVector()
	defer common.FreeCPoint2fVector(CPoints)
	CRect := faceRect.CRect()
	errCode := C.extract_face_keypoints(
		d.Handler(),
		(*C.uchar)(unsafe.Pointer(&data[0])),
		C.int(imgWidth), C.int(imgHeight),
		(*C.Rect)(unsafe.Pointer(CRect)),
		(*C.Point2fVector)(unsafe.Pointer(CPoints)),
	)
	C.free(unsafe.Pointer(CRect))
	if errCode != 0 {
		return nil, openvision.FaceLandmarkError(int(errCode))
	}
	points := common.GoPoint2fVector(CPoints, imgWidth, imgHeight)
	return points, nil
}
