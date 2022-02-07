#ifndef _SVM_TRAINER_H_
#define _SVM_TRAINER_H_

namespace ovclassifier {
class SVMTrainer {
public:
  virtual ~SVMTrainer(){};
  virtual void Reset() = 0;
  virtual void SetLabels(int labels) = 0;
  virtual void SetFeatures(int feats) = 0;
  virtual void AddData(int label, const float *vec) = 0;
  virtual int Train(const char *modelfile) = 0;
};

} // namespace ovclassifier
#endif // _SVM_TRAINER_H_
