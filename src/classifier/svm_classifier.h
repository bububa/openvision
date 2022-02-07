#ifndef _CLASSIFIER_SVM_CLASSIFIER_C_H_
#define _CLASSIFIER_SVM_CLASSIFIER_C_H_

#include "../common/common.h"
#ifdef __cplusplus
#include "svm/svm_classifier.hpp"
extern "C" {
#endif
typedef void *ISVMClassifier;
ISVMClassifier new_svm_binary_classifier();
ISVMClassifier new_svm_multiclass_classifier();
void destroy_svm_classifier(ISVMClassifier e);
int svm_classifier_load_model(ISVMClassifier e, const char *modelfile);
double svm_predict(ISVMClassifier e, const float *vec);
int svm_classify(ISVMClassifier e, const float *vec, FloatVector *scores);
#ifdef __cplusplus
}
#endif
#endif // !_CLASSIFER_SVM_CLASSIFIER_C_H_
