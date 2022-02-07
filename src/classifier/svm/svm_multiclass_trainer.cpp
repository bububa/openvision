#include "svm_multiclass_trainer.hpp"
#include "svm_light/svm_learn.h"
#include "svm_struct/svm_struct_learn.h"
#include "svm_struct_api.h"

namespace ovclassifier {

SVMMultiClassTrainer::SVMMultiClassTrainer() {
  alg_type = DEFAULT_ALG_TYPE;
  struct_parm = (STRUCT_LEARN_PARM *)malloc(sizeof(STRUCT_LEARN_PARM));
  struct_parm->C = 10000;
  struct_parm->slack_norm = 1;
  struct_parm->epsilon = DEFAULT_EPS;
  struct_parm->custom_argc = 0;
  struct_parm->loss_function = DEFAULT_LOSS_FCT;
  struct_parm->loss_type = DEFAULT_RESCALING;
  struct_parm->newconstretrain = 100;
  struct_parm->ccache_size = 5;
  struct_parm->batch_size = 100;

  learn_parm = (LEARN_PARM *)malloc(sizeof(LEARN_PARM));
  strcpy(learn_parm->predfile, "trans_predictions");
  strcpy(learn_parm->alphafile, "");
  learn_parm->biased_hyperplane = 1;
  learn_parm->remove_inconsistent = 0;
  learn_parm->skip_final_opt_check = 0;
  learn_parm->svm_maxqpsize = 10;
  learn_parm->svm_newvarsinqp = 0;
  // learn_parm->svm_iter_to_shrink = -9999;
  learn_parm->svm_iter_to_shrink = 100;
  learn_parm->maxiter = 100000;
  learn_parm->kernel_cache_size = 40;
  learn_parm->svm_c = 99999999; /* overridden by struct_parm->C */
  learn_parm->eps = 0.001;      /* overridden by struct_parm->epsilon */
  learn_parm->transduction_posratio = -1.0;
  learn_parm->svm_costratio = 1.0;
  learn_parm->svm_costratio_unlab = 1.0;
  learn_parm->svm_unlabbound = 1E-5;
  learn_parm->epsilon_crit = 0.001;
  learn_parm->epsilon_a = 1E-10; /* changed from 1e-15 */
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

  parse_struct_parameters(struct_parm);
}

SVMMultiClassTrainer::~SVMMultiClassTrainer() {
  if (learn_parm != NULL) {
    free(learn_parm);
    learn_parm = NULL;
  }
  if (kernel_parm != NULL) {
    free(kernel_parm);
    kernel_parm = NULL;
  }
  if (learn_parm != NULL) {
    free(learn_parm);
    learn_parm = NULL;
  }
}

void SVMMultiClassTrainer::Reset() {
  labels_ = 0;
  feats_ = 0;
  items_.clear();
}
void SVMMultiClassTrainer::SetLabels(int labels) { labels_ = labels; }
void SVMMultiClassTrainer::SetFeatures(int feats) { feats_ = feats; }
void SVMMultiClassTrainer::AddData(int label, const float *vec) {
  LabelItem itm;
  itm.label = label;
  for (int i = 0; i < feats_; ++i) {
    itm.vec.push_back(vec[i]);
  }
  items_.push_back(itm);
}

int SVMMultiClassTrainer::Train(const char *modelfile) {
  struct_verbosity = 2;
  int totdoc = items_.size();
  if (totdoc == 0 || feats_ == 0 || labels_ == 0) {
    return -1;
  }
  EXAMPLE *examples = (EXAMPLE *)my_malloc(sizeof(EXAMPLE) * totdoc);
  WORD *words = (WORD *)my_malloc(sizeof(WORD) * (feats_ * 10));
  for (int dnum = 0; dnum < totdoc; ++dnum) {
    const int docFeats = items_[dnum].vec.size();
    for (int i = 0; i < (feats_ + 10); ++i) {
      if (i >= feats_) {
        words[i].wnum = 0;
      } else {
        (words[i]).wnum = i + 1;
      }
      if (i >= docFeats) {
        (words[i]).weight = 0;
      } else {
        (words[i]).weight = (FVAL)items_[dnum].vec[i];
      }
    }
    DOC *doc =
        create_example(dnum, 0, 0, 0, create_svector(words, (char *)"", 1.0));
    examples[dnum].x.doc = doc;
    examples[dnum].y.class_ = (double)items_[dnum].label + 0.1;
    examples[dnum].y.scores = NULL;
    examples[dnum].y.num_classes_ = (double)labels_ + 0.1;
  }
  free(words);

  SAMPLE sample;
  sample.n = totdoc;
  sample.examples = examples;
  STRUCTMODEL structmodel;
  /* Do the learning and return structmodel. */
  if (alg_type == 0)
    svm_learn_struct(sample, struct_parm, learn_parm, kernel_parm, &structmodel,
                     NSLACK_ALG);
  else if (alg_type == 1)
    svm_learn_struct(sample, struct_parm, learn_parm, kernel_parm, &structmodel,
                     NSLACK_SHRINK_ALG);
  else if (alg_type == 2)
    svm_learn_struct_joint(sample, struct_parm, learn_parm, kernel_parm,
                           &structmodel, ONESLACK_PRIMAL_ALG);
  else if (alg_type == 3)
    svm_learn_struct_joint(sample, struct_parm, learn_parm, kernel_parm,
                           &structmodel, ONESLACK_DUAL_ALG);
  else if (alg_type == 4)
    svm_learn_struct_joint(sample, struct_parm, learn_parm, kernel_parm,
                           &structmodel, ONESLACK_DUAL_CACHE_ALG);
  else if (alg_type == 9)
    svm_learn_struct_joint_custom(sample, struct_parm, learn_parm, kernel_parm,
                                  &structmodel);
  else
    return -1;

  write_struct_model((char *)modelfile, &structmodel, struct_parm);

  free_struct_sample(sample);
  free_struct_model(structmodel);

  svm_struct_learn_api_exit();
  return 0;
}
} // namespace ovclassifier
