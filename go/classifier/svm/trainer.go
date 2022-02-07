package svm

/*
#include <stdlib.h>
#include <stdbool.h>
#include "openvision/classifier/svm_trainer.h"
*/
import "C"
import (
	"unsafe"

	openvision "github.com/bububa/openvision/go"
)

// Trainer represents svm trainer
type Trainer interface {
	Reset()
	Destroy()
	SetLabels(int)
	SetFeatures(int)
	AddData(int, []float32)
	Train(modelPath string) error
}

// DestroyTrainer destroy C.ISVMTrainer
func DestroyTrainer(d C.ISVMTrainer) {
	C.destroy_svm_trainer(d)
}

// ResetTrainer reset C.ISVMTrainer
func ResetTrainer(d C.ISVMTrainer) {
	C.svm_trainer_reset(d)
}

// SetLabels set total labels
func SetLabels(d C.ISVMTrainer, labels int) {
	C.svm_trainer_set_labels(d, C.int(labels))
}

// SetFeatures set total features
func SetFeatures(d C.ISVMTrainer, feats int) {
	C.svm_trainer_set_features(d, C.int(feats))
}

// AddData add data with label
func AddData(d C.ISVMTrainer, labelID int, feats []float32) {
	vec := make([]C.float, 0, len(feats))
	for _, v := range feats {
		vec = append(vec, C.float(v))
	}
	C.svm_trainer_add_data(d, C.int(labelID), &vec[0])
}

// Train train model
func Train(d C.ISVMTrainer, modelPath string) error {
	cPath := C.CString(modelPath)
	defer C.free(unsafe.Pointer(cPath))
	errCode := C.svm_train(d, cPath)
	if errCode != 0 {
		return openvision.TrainingError(int(errCode))
	}
	return nil
}
