#include "svm_binary_trainer.hpp"
#include "svm_light/svm_common.h"
#include "svm_light/svm_learn.h"

namespace ovclassifier {

SVMBinaryTrainer::SVMBinaryTrainer() {
  learn_parm = (LEARN_PARM *)malloc(sizeof(LEARN_PARM));
  strcpy(learn_parm->predfile, "trans_predictions");
  strcpy(learn_parm->alphafile, "");
  learn_parm->biased_hyperplane = 1;
  learn_parm->sharedslack = 0;
  learn_parm->remove_inconsistent = 0;
  learn_parm->skip_final_opt_check = 0;
  learn_parm->svm_maxqpsize = 10;
  learn_parm->svm_newvarsinqp = 0;
  learn_parm->svm_iter_to_shrink = -9999;
  learn_parm->maxiter = 100000;
  learn_parm->kernel_cache_size = 40;
  learn_parm->svm_c = 0.0;
  learn_parm->eps = 0.1;
  learn_parm->transduction_posratio = -1.0;
  learn_parm->svm_costratio = 1.0;
  learn_parm->svm_costratio_unlab = 1.0;
  learn_parm->svm_unlabbound = 1E-5;
  learn_parm->epsilon_crit = 0.001;
  learn_parm->epsilon_a = 1E-15;
  learn_parm->compute_loo = 0;
  learn_parm->rho = 1.0;
  learn_parm->xa_depth = 0;
  kernel_parm = (KERNEL_PARM *)malloc(sizeof(KERNEL_PARM));
  kernel_parm->kernel_type = 0;
  kernel_parm->poly_degree = 3;
  kernel_parm->rbf_gamma = 1.0;
  kernel_parm->coef_lin = 1;
  kernel_parm->coef_const = 1;
  strcpy(kernel_parm->custom, "empty");
}

SVMBinaryTrainer::~SVMBinaryTrainer() {
  if (learn_parm != NULL) {
    free(learn_parm);
    learn_parm = NULL;
  }
  if (kernel_parm != NULL) {
    free(kernel_parm);
    kernel_parm = NULL;
  }
  if (kernel_cache != NULL) {
    kernel_cache_cleanup(kernel_cache);
    kernel_cache = NULL;
  }
}

void SVMBinaryTrainer::Reset() {
  feats_ = 0;
  items_.clear();
  if (kernel_cache != NULL) {
    kernel_cache_cleanup(kernel_cache);
  }
}

void SVMBinaryTrainer::SetLabels(int labels) {}

void SVMBinaryTrainer::SetFeatures(int feats) { feats_ = feats; }

void SVMBinaryTrainer::AddData(int label, const float *vec) {
  if (label != 1 && label != -1) {
    return;
  }
  LabelItem itm;
  itm.label = label;
  for (int i = 0; i < feats_; ++i) {
    itm.vec.push_back(vec[i]);
  }
  items_.push_back(itm);
}

int SVMBinaryTrainer::Train(const char *modelfile) {
  int totdoc = items_.size();
  if (totdoc == 0 || feats_ == 0) {
    return -1;
  }
  kernel_cache = kernel_cache_init(totdoc, learn_parm->kernel_cache_size);
  double *labels = (double *)malloc(sizeof(double) * totdoc);
  double *alphas = (double *)malloc(sizeof(double) * totdoc);
  DOC **docs = (DOC **)malloc(sizeof(DOC *) * totdoc);
  WORD *words = (WORD *)malloc(sizeof(WORD) * (feats_ + 10));
  for (int dnum = 0; dnum < totdoc; ++dnum) {
    const int docFeats = items_[dnum].vec.size();
    for (int i = 0; i < (feats_ + 10); ++i) {
      if (i >= feats_) {
        words[i].wnum = 0;
      } else {
        words[i].wnum = i + 1;
      }
      if (i >= docFeats) {
        words[i].weight = 0;
      } else {
        words[i].weight = items_[dnum].vec[i];
      }
    }
    labels[dnum] = items_[dnum].label;
    docs[dnum] =
        create_example(dnum, 0, 0, 0, create_svector(words, (char *)"", 1.0));
  }
  free(words);

  MODEL *model_ = (MODEL *)malloc(sizeof(MODEL));
  svm_learn_classification(docs, labels, (long int)totdoc, (long int)feats_,
                           learn_parm, kernel_parm, kernel_cache, model_,
                           alphas);
  write_model((char *)modelfile, model_);
  free(labels);
  labels = NULL;
  free(alphas);
  alphas = NULL;
  for (int i = 0; i < totdoc; i++) {
    free_example(docs[i], 1);
  }
  free_model(model_, 0);
  model_ = NULL;
  return 0;
}

} // namespace ovclassifier
