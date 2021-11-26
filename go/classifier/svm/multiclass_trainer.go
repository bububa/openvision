package svm

/*
#include <stdlib.h>
#include <stdbool.h>
#include "openvision/classifier/svm_trainer.h"
*/
import "C"

// MultiClassTrainer represents svm trainer
type MultiClassTrainer struct {
	d C.ISVMTrainer
}

// NewMultiClassTrainer returns a new MultiClassTrainer
func NewMultiClassTrainer() *MultiClassTrainer {
	return &MultiClassTrainer{
		d: C.new_svm_multiclass_trainer(),
	}
}

// Destroy destroy C.ISVMTrainer
func (t *MultiClassTrainer) Destroy() {
	DestroyTrainer(t.d)
}

// Reset reset C.ISVMTrainer
func (t *MultiClassTrainer) Reset() {
	ResetTrainer(t.d)
}

// SetLabels set total labels
func (t *MultiClassTrainer) SetLabels(labels int) {
	SetLabels(t.d, labels)
}

// SetFeatures set total features
func (t *MultiClassTrainer) SetFeatures(feats int) {
	SetFeatures(t.d, feats)
}

// AddData add data with label
func (t *MultiClassTrainer) AddData(labelID int, feats []float32) {
	AddData(t.d, labelID, feats)
}

// Train train model
func (t *MultiClassTrainer) Train(modelPath string) error {
	return Train(t.d, modelPath)
}
