#include "svm_binary_classifier.hpp"
#include "svm_light/svm_common.h"

namespace ovclassifier {
SVMBinaryClassifier::SVMBinaryClassifier() {}

SVMBinaryClassifier::~SVMBinaryClassifier() {
  if (model_ != NULL) {
    free_model(model_, 1);
    model_ = NULL;
  }
}

int SVMBinaryClassifier::LoadModel(const char *modelfile) {
  if (model_ != NULL) {
    free_model(model_, 1);
  }
  model_ = (MODEL *)my_malloc(sizeof(MODEL));
  model_ = read_model((char *)modelfile);
  if (model_->kernel_parm.kernel_type == 0) { /* linear kernel */
    /* compute weight vector */
    add_weight_vector_to_linear_model(model_);
  }
  return 0;
}

double SVMBinaryClassifier::Predict(const float *vec) {
  WORD *words = (WORD *)malloc(sizeof(WORD) * (model_->totwords + 10));
  for (int i = 0; i < (model_->totwords + 10); ++i) {
    if (i >= model_->totwords) {
      words[i].wnum = 0;
      words[i].weight = 0;
    } else {
      words[i].wnum = i + 1;
      words[i].weight = vec[i];
    }
  }
  DOC *doc =
      create_example(-1, 0, 0, 0.0, create_svector(words, (char *)"", 1.0));
  free(words);
  double dist;
  if (model_->kernel_parm.kernel_type == 0) {
    dist = classify_example_linear(model_, doc);
  } else {
    dist = classify_example(model_, doc);
  }
  free_example(doc, 1);
  return dist;
}

int SVMBinaryClassifier::Classify(const float *vec,
                                  std::vector<float> &scores) {
  return -1;
}
} // namespace ovclassifier
