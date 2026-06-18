#pragma once
#include <string>
#include <vector>

namespace postProcessor {
std::vector<int> topk(const std::vector<float> &data, int k);

std::vector<std::string> loadLabels(const std::string &path);

std::vector<float> softmax(const std::vector<float> &logits);
} // namespace postProcessor