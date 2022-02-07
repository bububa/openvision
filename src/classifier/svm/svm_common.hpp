#ifndef _SVM_COMMON_HPP_
#define _SVM_COMMON_HPP_

#include <vector>
namespace ovclassifier {
struct LabelItem {
  int label;
  std::vector<float> vec;
};
} // namespace ovclassifier
#endif // !_SVM_COMMON_HPP_
