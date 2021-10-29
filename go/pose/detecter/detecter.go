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
	Handler() C.IPoseDetecter
	LoadModel(modelPath string) error
	ExtractKeypoints(img *common.Image) ([]common.ObjectInfo, error)
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
func ExtractKeypoints(d Detecter, img *common.Image) ([]common.ObjectInfo, error) {
	imgWidth := img.WidthF64()
	imgHeight := img.HeightF64()
	data := img.Bytes()
	cObjs := common.NewCObjectInfoVector()
	defer common.FreeCObjectInfoVector(cObjs)
	errCode := C.extract_pose_rois(
		d.Handler(),
		(*C.uchar)(unsafe.Pointer(&data[0])),
		C.int(imgWidth),
		C.int(imgHeight),
		(*C.ObjectInfoVector)(unsafe.Pointer(cObjs)))
	if errCode != 0 {
		return nil, openvision.DetectPoseError(int(errCode))
	}
	totalROIs := common.CObjectInfoVectiorLength(cObjs)
	rois := make([]common.ObjectInfo, 0, totalROIs)
	ptr := common.CObjectInfoVectorPtr(cObjs)
	for i := 0; i < totalROIs; i++ {
		cKeypoints := common.NewCKeypointVector()
		defer common.FreeCKeypointVector(cKeypoints)
		cROI := (*C.ObjectInfo)(unsafe.Pointer(uintptr(ptr) + uintptr(C.sizeof_ObjectInfo*C.int(i))))
		errCode := C.extract_pose_keypoints(
			d.Handler(),
			(*C.uchar)(unsafe.Pointer(&data[0])),
			C.int(imgWidth),
			C.int(imgHeight),
			(*C.Rect)(unsafe.Pointer(&cROI.rect)),
			(*C.KeypointVector)(unsafe.Pointer(cKeypoints)))
		if errCode != 0 {
			return nil, openvision.DetectPoseError(int(errCode))
		}
		keypoints := common.GoKeypointVector(cKeypoints, imgWidth, imgHeight)
		rois = append(rois, common.ObjectInfo{
			Keypoints: keypoints,
			Rect: common.Rect(
				float64(cROI.rect.x)/imgWidth,
				float64(cROI.rect.y)/imgHeight,
				float64(cROI.rect.width)/imgWidth,
				float64(cROI.rect.height)/imgHeight,
			),
			Score: float32(cROI.prob),
		})

	}
	return rois, nil
}
