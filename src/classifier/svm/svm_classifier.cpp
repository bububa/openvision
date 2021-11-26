#include "../svm_classifier.h"
#include "svm_binary_classifier.hpp"
#include "svm_multiclass_classifier.hpp"

ISVMClassifier new_svm_binary_classifier() {
  return new ovclassifier::SVMBinaryClassifier();
}
ISVMClassifier new_svm_multiclass_classifier() {
  return new ovclassifier::SVMMultiClassClassifier();
}
void destroy_svm_classifier(ISVMClassifier e) {
  delete static_cast<ovclassifier::SVMClassifier *>(e);
}
int svm_classifier_load_model(ISVMClassifier e, const char *modelfile) {
  return static_cast<ovclassifier::SVMClassifier *>(e)->LoadModel(modelfile);
}
double svm_predict(ISVMClassifier e, const float *vec) {
  return static_cast<ovclassifier::SVMClassifier *>(e)->Predict(vec);
}
int svm_classify(ISVMClassifier e, const float *vec, FloatVector *scores) {
  std::vector<float> scores_;
  int ret =
      static_cast<ovclassifier::SVMClassifier *>(e)->Classify(vec, scores_);
  if (ret != 0) {
    return ret;
  }
  scores->length = scores_.size();
  scores->values = (float *)malloc(sizeof(float) * scores->length);
  for (int i = 0; i < scores->length; ++i) {
    scores->values[i] = scores_[i];
  }
  return 0;
}
