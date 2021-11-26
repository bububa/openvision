#ifndef _SVM_BINARY_TRAINER_H_
#define _SVM_BINARY_TRAINER_H_

#include "svm_common.hpp"
#include "svm_light/svm_common.h"
#include "svm_trainer.hpp"
#include <vector>

namespace ovclassifier {
class SVMBinaryTrainer : public SVMTrainer {
public:
  SVMBinaryTrainer();
  ~SVMBinaryTrainer();
  void Reset();
  void SetLabels(int labels);
  void SetFeatures(int feats);
  void AddData(int label, const float *vec);
  int Train(const char *modelfile);

private:
  KERNEL_PARM *kernel_parm = NULL;
  LEARN_PARM *learn_parm = NULL;
  KERNEL_CACHE *kernel_cache = NULL;
  int feats_;
  std::vector<LabelItem> items_;
};
} // namespace ovclassifier
#endif // _SVM_BINARY_TRAINER_H_
