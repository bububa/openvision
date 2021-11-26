package svm

/*
#include <stdlib.h>
#include <stdbool.h>
#include "openvision/classifier/svm_trainer.h"
*/
import "C"

// BinaryTrainer represents svm trainer
type BinaryTrainer struct {
	d C.ISVMTrainer
}

// NewBinaryTrainer returns a new BinaryTrainer
func NewBinaryTrainer() *BinaryTrainer {
	return &BinaryTrainer{
		d: C.new_svm_binary_trainer(),
	}
}

// Destroy destroy C.ISVMTrainer
func (t *BinaryTrainer) Destroy() {
	DestroyTrainer(t.d)
}

// Reset reset C.ISVMTrainer
func (t *BinaryTrainer) Reset() {
	ResetTrainer(t.d)
}

// SetLabels set total labels
func (t *BinaryTrainer) Labels(labels int) {
	SetLabels(t.d, labels)
}

// SetFeatures set total features
func (t *BinaryTrainer) SetFeatures(feats int) {
	SetFeatures(t.d, feats)
}

// AddData add data with label
func (t *BinaryTrainer) AddData(labelID int, feats []float32) {
	AddData(t.d, labelID, feats)
}

// Train train model
func (t *BinaryTrainer) Train(modelPath string) error {
	return Train(t.d, modelPath)
}
