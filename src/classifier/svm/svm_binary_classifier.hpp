#ifndef _CLASSIFIER_SVM_BINARY_CLASSIFIER_H_
#define _CLASSIFIER_SVM_BINARY_CLASSIFIER_H_

#include "svm_classifier.hpp"

namespace ovclassifier {
class SVMBinaryClassifier : public SVMClassifier {
public:
  SVMBinaryClassifier();
  ~SVMBinaryClassifier();
  int LoadModel(const char *modelfile);
  double Predict(const float *vec);
  int Classify(const float *vec, std::vector<float> &scores);

private:
  MODEL *model_ = NULL;
};
} // namespace ovclassifier
#endif // !_CLASSIFIER_SVM_BINARY_CLASSIFIER_H_
