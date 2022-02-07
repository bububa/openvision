package svm

/*
#include <stdlib.h>
#include <stdbool.h>
#include "openvision/classifier/svm_classifier.h"
*/
import "C"

// BinaryClassifier represents svm classifier
type BinaryClassifier struct {
	d C.ISVMClassifier
}

// NewBinaryClassifier returns a new BinaryClassifier
func NewBinaryClassifier() *BinaryClassifier {
	return &BinaryClassifier{
		d: C.new_svm_binary_classifier(),
	}
}

// Destroy destroy C.ISVMClassifier
func (t *BinaryClassifier) Destroy() {
	DestroyClassifier(t.d)
}

// LoadModel load model
func (t *BinaryClassifier) LoadModel(modelPath string) {
	LoadClassifierModel(t.d, modelPath)
}

// Predict returns predicted score
func (t *BinaryClassifier) Predict(vec []float32) float64 {
	return Predict(t.d, vec)
}

// Classify returns classifid scores
func (t *BinaryClassifier) Classify(vec []float32) ([]float64, error) {
	return Classify(t.d, vec)
}
