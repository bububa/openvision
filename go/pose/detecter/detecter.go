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
	ExtractKeypoints(img *common.Image) ([]common.ObjectInfo, error)
}

// ExtractKeypoints detect pose keypoints using detecter
func ExtractKeypoints(d Detecter, img *common.Image) ([]common.ObjectInfo, error) {
	imgWidth := img.WidthF64()
	imgHeight := img.HeightF64()
	data := img.Bytes()
	cObjs := common.NewCObjectInfoVector()
	defer common.FreeCObjectInfoVector(cObjs)
	errCode := C.extract_pose_rois(
		(C.IPoseDetecter)(d.Pointer()),
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
			(C.IPoseDetecter)(d.Pointer()),
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
			Score: float32(cROI.score),
		})

	}
	return rois, nil
}
