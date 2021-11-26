#ifndef _SVM_MULTCLASS_TRAINER_H_
#define _SVM_MULTCLASS_TRAINER_H_

#include "svm_common.hpp"
#include "svm_light/svm_common.h"
#include "svm_struct_api_types.h"
#include "svm_trainer.hpp"
#include <vector>

namespace ovclassifier {
class SVMMultiClassTrainer : public SVMTrainer {
public:
  SVMMultiClassTrainer();
  ~SVMMultiClassTrainer();
  void Reset();
  void SetLabels(int labels);
  void SetFeatures(int feats);
  void AddData(int label, const float *vec);
  int Train(const char *modelfile);

private:
  KERNEL_PARM *kernel_parm = NULL;
  LEARN_PARM *learn_parm = NULL;
  STRUCT_LEARN_PARM *struct_parm = NULL;
  int alg_type;
  int feats_;
  int labels_;
  std::vector<LabelItem> items_;
};
} // namespace ovclassifier
#endif // _SVM_MULTICLASS_TRAINER_H_
