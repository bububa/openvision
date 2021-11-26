package svm

/*
#include <stdlib.h>
#include <stdbool.h>
#include "openvision/classifier/svm_classifier.h"
*/
import "C"
import (
	"unsafe"

	openvision "github.com/bububa/openvision/go"
	"github.com/bububa/openvision/go/common"
)

// Classifier represents svm classifier
type Classifier interface {
	LoadModel(string)
	Destroy()
	Predict(vec []float32) float64
	Classify(vec []float32) (scores []float64, err error)
}

// Destroy destroy C.ISVMClassifier
func DestroyClassifier(d C.ISVMClassifier) {
	C.destroy_svm_classifier(d)
}

// LoadModel load model
func LoadClassifierModel(d C.ISVMClassifier, modelPath string) {
	cPath := C.CString(modelPath)
	defer C.free(unsafe.Pointer(cPath))
	C.svm_classifier_load_model(d, cPath)
}

func Predict(d C.ISVMClassifier, vec []float32) float64 {
	cvals := make([]C.float, 0, len(vec))
	for _, v := range vec {
		cvals = append(cvals, C.float(v))
	}
	score := C.svm_predict(d, &cvals[0])
	return float64(score)
}

// Classify returns class scores
func Classify(d C.ISVMClassifier, vec []float32) ([]float64, error) {
	cvals := make([]C.float, 0, len(vec))
	for _, v := range vec {
		cvals = append(cvals, C.float(v))
	}
	cScores := common.NewCFloatVector()
	defer common.FreeCFloatVector(cScores)
	errCode := C.svm_classify(d, &cvals[0], (*C.FloatVector)(unsafe.Pointer(cScores)))
	if errCode != 0 {
		return nil, openvision.ClassifyError(int(errCode))
	}
	return common.GoFloatVector(cScores), nil
}
