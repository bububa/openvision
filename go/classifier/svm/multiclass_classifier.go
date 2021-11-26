package svm

/*
#include <stdlib.h>
#include <stdbool.h>
#include "openvision/classifier/svm_classifier.h"
*/
import "C"

// MultiClassClassifier represents svm classifier
type MultiClassClassifier struct {
	d C.ISVMClassifier
}

// NewMultiClassClassifier returns a new MultiClassClassifier
func NewMultiClassClassifier() *MultiClassClassifier {
	return &MultiClassClassifier{
		d: C.new_svm_multiclass_classifier(),
	}
}

// Destroy destroy C.ISVMClassifier
func (t *MultiClassClassifier) Destroy() {
	DestroyClassifier(t.d)
}

// LoadModel load model
func (t *MultiClassClassifier) LoadModel(modelPath string) {
	LoadClassifierModel(t.d, modelPath)
}

// Predict returns predicted score
func (t *MultiClassClassifier) Predict(vec []float32) float64 {
	return Predict(t.d, vec)
}

// Classify returns classifid scores
func (t *MultiClassClassifier) Classify(vec []float32) ([]float64, error) {
	return Classify(t.d, vec)
}
