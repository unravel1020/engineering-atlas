#pragma once

#include "modelInfo.h"
#include <onnxruntime_cxx_api.h>
#include <opencv2/opencv.hpp>
#include <string>
#include <unordered_map>
#include <vector>

// InferenceResult couples raw ONNX outputs with the geometric transform used
// during preprocessing. This allows post-processing to map detection boxes
// back to the original image without recomputing the transform.
struct InferenceResult {
  std::unordered_map<std::string, std::vector<float>> outputs;
  PreprocessInfo preprocess;
};

// Inference owns a single ONNX Runtime session. It is deliberately decoupled
// from task-specific preprocessing/postprocessing so that the same engine can
// serve classification, detection and future tasks.
class Inference {
public:
  explicit Inference(const ModelInfo &model_info);

  InferenceResult
  runWithInfo(const std::unordered_map<std::string, cv::Mat> &input_images);

  InferenceResult runWithInfo(const cv::Mat &img);

  std::unordered_map<std::string, std::vector<float>>
  run(const std::unordered_map<std::string, cv::Mat> &input_images);

  std::vector<float> run(const cv::Mat &img);

  void printModelInfo() const;

  const ModelInfo &modelInfo() const;

private:
  // Overwrite JSON-provided tensor shapes/types with the real ONNX session
  // metadata. This is necessary because model.json may declare symbolic or
  // placeholder dimensions such as -1 for batch size.
  void syncTensorShape();

  // Replace symbolic dimensions (<= 0) in the model input shape with the
  // concrete dimensions produced by the preprocessor.
  std::vector<int64_t> fixInputShape(const std::vector<int64_t> &shape, int c,
                                     int h, int w) const;

private:
  Ort::Env env_;

  Ort::Session session_;

  Ort::SessionOptions options_;

  ModelInfo model_info_;
};
