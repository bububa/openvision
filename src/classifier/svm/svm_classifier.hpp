#ifndef _CLASSIFIER_SVM_CLASSIFIER_H_
#define _CLASSIFIER_SVM_CLASSIFIER_H_

#include "svm_light/svm_common.h"
#include <vector>

namespace ovclassifier {
class SVMClassifier {
public:
  virtual ~SVMClassifier(){};
  virtual int LoadModel(const char *modelfile) = 0;
  virtual double Predict(const float *vec) = 0;
  virtual int Classify(const float *vec, std::vector<float> &scores) = 0;
};
} // namespace ovclassifier
#endif // !_CLASSIFIER_SVM_CLASSIFIER_H_
