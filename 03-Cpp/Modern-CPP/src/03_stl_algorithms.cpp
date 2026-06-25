// 优先使用 STL 算法和容器，减少手写循环，提高代码可读性和正确性。
// 本示例展示 vector、sort、find、transform、accumulate 的常见用法。

#include <algorithm>
#include <cmath>
#include <iostream>
#include <numeric>
#include <vector>

int main() {
  std::vector<float> scores = {0.85f, 0.12f, 0.93f, 0.45f, 0.67f};

  // sort + 自定义比较器：获取 top-k 分数的下标。
  std::vector<std::size_t> indices(scores.size());
  std::iota(indices.begin(), indices.end(), 0); // 0, 1, 2, ...
  std::sort(indices.begin(), indices.end(), [&scores](std::size_t a,
                                                       std::size_t b) {
    return scores[a] > scores[b]; // 按分数降序
  });

  std::cout << "top-3 indices: ";
  for (std::size_t i = 0; i < 3 && i < indices.size(); ++i) {
    std::cout << indices[i] << "(" << scores[indices[i]] << ") ";
  }
  std::cout << "\n";

  // transform：对容器中每个元素应用 softmax 的指数部分。
  std::vector<float> exp_values;
  exp_values.reserve(scores.size());
  std::transform(scores.begin(), scores.end(), std::back_inserter(exp_values),
                 [](float x) { return std::exp(x); });

  float sum = std::accumulate(exp_values.begin(), exp_values.end(), 0.0f);
  std::vector<float> probs;
  probs.reserve(exp_values.size());
  std::transform(exp_values.begin(), exp_values.end(),
                 std::back_inserter(probs),
                 [sum](float x) { return x / sum; });

  float prob_sum = std::accumulate(probs.begin(), probs.end(), 0.0f);
  std::cout << "softmax sum = " << prob_sum << " (should be ~1)\n";

  // find_if：查找第一个满足条件的元素。
  auto it = std::find_if(scores.begin(), scores.end(),
                         [](float x) { return x > 0.9f; });
  if (it != scores.end()) {
    std::cout << "first score > 0.9: " << *it << "\n";
  }

  std::cout << "[PASS] STL algorithms demo finished\n";
  return 0;
}
