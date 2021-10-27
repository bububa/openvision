package detecter

/*
#include <stdlib.h>
#include <stdbool.h>
#include "openvision/pose/common.h"
#include "openvision/pose/detecter.h"
*/
import "C"
import (
	"unsafe"

	openvision "github.com/bububa/openvision/go"
	"github.com/bububa/openvision/go/common"
	"github.com/bububa/openvision/go/pose"
)

// Detecter represents deteter interface
type Detecter interface {
	Handler() C.IDetecter
	LoadModel(modelPath string) error
	ExtractKeypoints(img *common.Image) ([]pose.ROI, error)
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

// ExtractKeypoints detect pose keypoints using detecter
func ExtractKeypoints(d Detecter, img *common.Image) ([]pose.ROI, error) {
	imgWidth := img.WidthF64()
	imgHeight := img.HeightF64()
	data := img.Bytes()
	cROIs := pose.NewCROIVector()
	defer pose.FreeCROIVector(cROIs)
	errCode := C.extract_pose_rois(
		d.Handler(),
		(*C.uchar)(unsafe.Pointer(&data[0])),
		C.int(imgWidth),
		C.int(imgHeight),
		(*C.PoseROIVector)(unsafe.Pointer(cROIs)))
	if errCode != 0 {
		return nil, openvision.DetectPoseError(int(errCode))
	}
	totalROIs := pose.CROIVectiorLength(cROIs)
	rois := make([]pose.ROI, 0, totalROIs)
	ptr := unsafe.Pointer(cROIs)
	for i := 0; i < totalROIs; i++ {
		cKeypoints := pose.NewCKeypointVector()
		defer pose.FreeCKeypointVector(cKeypoints)
		cROI := (*C.PoseROI)(unsafe.Pointer(uintptr(ptr) + uintptr(C.sizeof_PoseROI*C.int(i))))
		errCode := C.extract_pose_keypoints(
			d.Handler(),
			cROI,
			(*C.PoseKeypointVector)(unsafe.Pointer(cKeypoints)))
		if errCode != 0 {
			return nil, openvision.DetectPoseError(int(errCode))
		}
		keypoints := pose.GoKeypointVector(cKeypoints, imgWidth, imgHeight)
		rois = append(rois, pose.ROI{
			Keypoints: keypoints,
			Rect: common.Rect(
				float64(cROI.rect.x)/imgWidth,
				float64(cROI.rect.y)/imgHeight,
				float64(cROI.rect.width)/imgWidth,
				float64(cROI.rect.height)/imgHeight,
			),
			Score: float32(cROI.score),
		})

	}
	return rois, nil
}
