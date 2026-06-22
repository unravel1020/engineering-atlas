#pragma once

#include "modelInfo.h"
#include "result.h"
#include <string>
#include <vector>

namespace postProcessor {

using ClassificationResult = result::ClassificationResult;

using Detection = result::Detection;

using DetectionResult = result::DetectionResult;

std::vector<int> topk(const std::vector<float> &data, int k);

std::vector<std::string> loadLabels(const std::string &path);

std::vector<float> softmax(const std::vector<float> &logits);

ClassificationResult classify(const std::vector<float> &output,
                              const PostprocessConfig &cfg,
                              const std::string &model_dir);

DetectionResult detect(const std::vector<float> &output,
                       const PostprocessConfig &cfg,
                       const std::string &model_dir,
                       const PreprocessInfo &preprocess_info);

} // namespace postProcessor
