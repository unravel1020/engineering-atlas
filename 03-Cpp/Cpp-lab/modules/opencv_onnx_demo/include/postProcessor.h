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

std::vector<float> softmax(const std::vector<float> &logits);

ClassificationResult classify(const std::vector<float> &output,
                              const PostprocessConfig &cfg,
                              const std::string &model_dir);

// detect() expects the model output in N_C_K layout (batch x channels x
// candidates), which is the default YOLOv8 ONNX export format. The actual
// number of classes and candidates are read from PostprocessConfig so that
// the same function can adapt to different detection models.
DetectionResult detect(const std::vector<float> &output,
                       const PostprocessConfig &cfg,
                       const std::string &model_dir,
                       const PreprocessInfo &preprocess_info);

} // namespace postProcessor
