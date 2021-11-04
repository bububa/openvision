package common

/*
#include <stdlib.h>
#include <stdbool.h>
#include "openvision/common/common.h"
*/
import "C"
import (
	"unsafe"

	openvision "github.com/bububa/openvision/go"
)

// Estimator represents Estimator interface
type Estimator interface {
	LoadModel(modelPath string) error
	Destroy()
	Pointer() unsafe.Pointer
}

// SetEstimatorThreads set ncnn net opt.num_threads
func SetEstimatorThreads(e Estimator, n int) {
	C.set_num_threads((C.IEstimator)(e.Pointer()), C.int(n))
}

// SetEstimatorLightMode set ncnn net opt.lightmode
func SetEstimatorLightMode(e Estimator, mode bool) {
	C.set_light_mode((C.IEstimator)(e.Pointer()), C.bool(mode))
}

// DestroyEstimator destory an Estimator
func DestroyEstimator(e Estimator) {
	C.destroy_estimator((C.IEstimator)(e.Pointer()))
}

// EstimatorLoadModel load detecter model
func EstimatorLoadModel(e Estimator, modelPath string) error {
	cpath := C.CString(modelPath)
	defer C.free(unsafe.Pointer(cpath))
	retCode := C.load_model((C.IEstimator)(e.Pointer()), cpath)
	if retCode != 0 {
		return openvision.LoadModelError(int(retCode))
	}
	return nil
}
