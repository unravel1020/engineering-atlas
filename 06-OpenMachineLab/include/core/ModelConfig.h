#pragma once

#include <string>
#include <vector>

namespace oml {
namespace core {

// Preprocess configuration mirrors opencv_onnx_demo::PreprocessConfig so
// existing model.json files remain compatible.
struct PreprocessConfig {
  std::string color_format = "BGR";
  std::string resize_mode = "stretch";
  int target_h = 224;
  int target_w = 224;
  float scale = 1.0f / 255.0f;
  std::vector<float> mean = {0.0f, 0.0f, 0.0f};
  std::vector<float> std = {1.0f, 1.0f, 1.0f};
  std::string layout = "NCHW";
  std::vector<uint8_t> pad_color = {114, 114, 114};
};

// Postprocess configuration mirrors opencv_onnx_demo::PostprocessConfig.
struct PostprocessConfig {
  std::string activation = "softmax";
  int topk = 5;
  std::string labels_file;
  float conf_threshold = 0.25f;
  float nms_threshold = 0.45f;
  int num_classes = 80;
  int num_candidates = 8400;
  std::string box_format = "xywh";
  std::string output_layout = "N_C_K";
};

// Tensor descriptor used in model inputs/outputs.
struct TensorConfig {
  std::string name;
  std::string dtype = "float32";
  PreprocessConfig preprocess;
  PostprocessConfig postprocess;
};

// Per-model configuration loaded from model.json.
struct ModelConfig {
  std::string name;
  std::string task = "unknown";
  std::string model_file = "model.onnx";
  std::vector<TensorConfig> inputs;
  std::vector<TensorConfig> outputs;
};

} // namespace core
} // namespace oml
