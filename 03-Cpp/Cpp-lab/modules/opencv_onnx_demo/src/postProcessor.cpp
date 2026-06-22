#include "postProcessor.h"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <fstream>
#include <numeric>
#include <vector>

namespace postProcessor {

std::vector<int> topk(const std::vector<float> &data, int k) {
  if (k > static_cast<int>(data.size())) {
    k = static_cast<int>(data.size());
  }

  std::vector<int> idx(data.size());
  std::iota(idx.begin(), idx.end(), 0);

  std::partial_sort(idx.begin(), idx.begin() + k, idx.end(),
                    [&](int a, int b) { return data[a] > data[b]; });

  idx.resize(k);
  return idx;
}

std::vector<std::string> loadLabels(const std::string &path) {
  std::ifstream ifs(path);

  std::vector<std::string> labels;

  std::string line;

  while (std::getline(ifs, line)) {
    labels.push_back(line);
  }

  return labels;
}

std::vector<float> softmax(const std::vector<float> &logits) {
  std::vector<float> probs(logits.size());

  float maxValue = *std::max_element(logits.begin(), logits.end());

  float sum = 0.0f;

  for (size_t i = 0; i < logits.size(); i++) {
    probs[i] = std::exp(logits[i] - maxValue);
    sum += probs[i];
  }

  for (auto &x : probs) {
    x /= sum;
  }
  return probs;
}

ClassificationResult classify(const std::vector<float> &output,
                              const PostprocessConfig &cfg,
                              const std::string &model_dir) {
  ClassificationResult result;

  result.probs = softmax(output);
  result.indices = topk(result.probs, cfg.topk);
  result.labels = loadLabels(model_dir + "/" + cfg.labels_file);

  // 防御 labels 文件行数不足导致的越界访问
  while (result.labels.size() < output.size()) {
    result.labels.emplace_back();
  }

  return result;
}

} // namespace postProcessor
