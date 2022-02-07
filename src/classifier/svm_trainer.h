#ifndef _CLASSIFIER_SVM_TRAINER_C_H_
#define _CLASSIFIER_SVM_TRAINER_C_H_

#ifdef __cplusplus
#include "svm/svm_trainer.hpp"
extern "C" {
#endif
typedef void *ISVMTrainer;
ISVMTrainer new_svm_binary_trainer();
ISVMTrainer new_svm_multiclass_trainer();
void destroy_svm_trainer(ISVMTrainer trainer);
void svm_trainer_reset(ISVMTrainer trainer);
void svm_trainer_set_labels(ISVMTrainer trainer, int labels);
void svm_trainer_set_features(ISVMTrainer trainer, int feats);
void svm_trainer_add_data(ISVMTrainer trainer, int label, const float *vec);
int svm_train(ISVMTrainer trainer, const char *modelfile);
#ifdef __cplusplus
}
#endif
#endif // !_CLASSIFER_SVM_TRAINER_C_H_
