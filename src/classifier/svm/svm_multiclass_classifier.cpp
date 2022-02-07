#include "svm_multiclass_classifier.hpp"
#include "svm_struct/svm_struct_common.h"
#include "svm_struct_api.h"

namespace ovclassifier {
SVMMultiClassClassifier::SVMMultiClassClassifier() {}
SVMMultiClassClassifier::~SVMMultiClassClassifier() {
  if (model_ != NULL) {
    free_struct_model(*model_);
    free(model_);
    model_ = NULL;
  }
  if (sparm_ != NULL) {
    free(sparm_);
    sparm_ = NULL;
  }
}
int SVMMultiClassClassifier::LoadModel(const char *modelfile) {
  if (model_ != NULL) {
    free_struct_model(*model_);
  }
  if (sparm_ != NULL) {
    free(sparm_);
  }
  model_ = (STRUCTMODEL *)my_malloc(sizeof(STRUCTMODEL));
  sparm_ = (STRUCT_LEARN_PARM *)my_malloc(sizeof(STRUCT_LEARN_PARM));
  (*model_) = read_struct_model((char *)modelfile, sparm_);
  if (model_->svm_model->kernel_parm.kernel_type ==
      LINEAR) { /* linear kernel */
    /* compute weight vector */
    add_weight_vector_to_linear_model(model_->svm_model);
    model_->w = model_->svm_model->lin_weights;
  }
  return 0;
}

double SVMMultiClassClassifier::Predict(const float *vec) { return 0; }

int SVMMultiClassClassifier::Classify(const float *vec,
                                      std::vector<float> &scores) {
  if (model_ == NULL || sparm_ == NULL) {
    return -1;
  }
  struct_verbosity = 5;
  int feats = sparm_->num_features;
  WORD *words = (WORD *)malloc(sizeof(WORD) * (feats + 10));
  for (int i = 0; i < (feats + 10); ++i) {
    if (i >= feats) {
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
  PATTERN pattern;
  pattern.doc = doc;
  LABEL y = classify_struct_example(pattern, model_, sparm_);
  free_pattern(pattern);
  scores.clear();
  for (int i = 1; i <= y.num_classes_; ++i) {
    scores.push_back(y.scores[i]);
  }
  free_label(y);
  return 0;
}
} // namespace ovclassifier
