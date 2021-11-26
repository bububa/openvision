#include "../svm_trainer.h"
#include "svm_binary_trainer.hpp"
#include "svm_multiclass_trainer.hpp"

ISVMTrainer new_svm_binary_trainer() {
  return new ovclassifier::SVMBinaryTrainer();
}
ISVMTrainer new_svm_multiclass_trainer() {
  return new ovclassifier::SVMMultiClassTrainer();
}

void destroy_svm_trainer(ISVMTrainer trainer) {
  delete static_cast<ovclassifier::SVMTrainer *>(trainer);
}

void svm_trainer_reset(ISVMTrainer trainer) {
  static_cast<ovclassifier::SVMTrainer *>(trainer)->Reset();
}

void svm_trainer_set_labels(ISVMTrainer trainer, int labels) {
  static_cast<ovclassifier::SVMTrainer *>(trainer)->SetLabels(labels);
}

void svm_trainer_set_features(ISVMTrainer trainer, int feats) {
  static_cast<ovclassifier::SVMTrainer *>(trainer)->SetFeatures(feats);
}

void svm_trainer_add_data(ISVMTrainer trainer, int label, const float *vec) {
  static_cast<ovclassifier::SVMTrainer *>(trainer)->AddData(label, vec);
}

int svm_train(ISVMTrainer trainer, const char *modelfile) {
  return static_cast<ovclassifier::SVMTrainer *>(trainer)->Train(modelfile);
}
