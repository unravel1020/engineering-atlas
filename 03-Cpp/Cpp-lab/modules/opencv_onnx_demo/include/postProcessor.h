#pragma once

#include "modelInfo.h"
#include <string>
#include <vector>

namespace postProcessor {

struct ClassificationResult {
  std::vector<std::string> labels;

  std::vector<float> probs;

  std::vector<int> indices;
};

std::vector<int> topk(const std::vector<float> &data, int k);

std::vector<std::string> loadLabels(const std::string &path);

std::vector<float> softmax(const std::vector<float> &logits);

ClassificationResult classify(const std::vector<float> &output,
                              const PostprocessConfig &cfg,
                              const std::string &model_dir);

} // namespace postProcessor
