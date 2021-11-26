#ifndef _CLASSIFIER_SVM_MULTICLASS_CLASSIFIER_H_
#define _CLASSIFIER_SVM_MULTICLASS_CLASSIFIER_H_

#include "svm_classifier.hpp"
#include "svm_struct_api_types.h"

namespace ovclassifier {
class SVMMultiClassClassifier : public SVMClassifier {
public:
  SVMMultiClassClassifier();
  ~SVMMultiClassClassifier();
  int LoadModel(const char *modelfile);
  double Predict(const float *vec);
  int Classify(const float *vec, std::vector<float> &scores);

private:
  STRUCTMODEL *model_ = NULL;
  STRUCT_LEARN_PARM *sparm_ = NULL;
};
} // namespace ovclassifier
#endif // !_CLASSIFIER_SVM_MULTICLASS_CLASSIFIER_H_
