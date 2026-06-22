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

struct Detection {
  int class_id = 0;

  float confidence = 0.0f;

  float x1 = 0.0f;

  float y1 = 0.0f;

  float x2 = 0.0f;

  float y2 = 0.0f;
};

std::vector<int> topk(const std::vector<float> &data, int k);

std::vector<std::string> loadLabels(const std::string &path);

std::vector<float> softmax(const std::vector<float> &logits);

ClassificationResult classify(const std::vector<float> &output,
                              const PostprocessConfig &cfg,
                              const std::string &model_dir);

std::vector<Detection> detect(const std::vector<float> &output,
                              const PostprocessConfig &cfg,
                              const std::string &model_dir,
                              const PreprocessInfo &preprocess_info);

} // namespace postProcessor
