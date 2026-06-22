#pragma once

#include <onnxruntime_cxx_api.h>
#include <cstdint>
#include <string>
#include <vector>

struct TensorInfo {
  std::string name;

  std::vector<int64_t> shape;

  ONNXTensorElementDataType type;
};

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

struct PreprocessInfo {
  int orig_h = 0;

  int orig_w = 0;

  int model_h = 0;

  int model_w = 0;

  float scale = 1.0f;

  int pad_top = 0;

  int pad_bottom = 0;

  int pad_left = 0;

  int pad_right = 0;

  std::string resize_mode;
};

struct PostprocessConfig {
  std::string activation = "softmax";

  int topk = 5;

  std::string labels_file;

  float conf_threshold = 0.25f;

  float nms_threshold = 0.45f;

  // Detection-specific metadata
  int num_classes = 80;

  int num_candidates = 8400;

  std::string box_format = "xywh"; // xywh | xyxy

  std::string output_layout = "N_C_K"; // N_C_K: e.g. 1 x 84 x 8400
};

class ModelInfo {
public:
  ModelInfo() = default;

  ModelInfo(const std::string &model_dir, const std::string &model_file,
            const std::string &task,
            const std::vector<TensorInfo> &inputs,
            const std::vector<TensorInfo> &outputs,
            const PreprocessConfig &preprocess,
            const PostprocessConfig &postprocess);

  const std::string &modelDir() const;

  std::string modelPath() const;

  const std::string &task() const;

  const std::vector<TensorInfo> &inputs() const;

  const std::vector<TensorInfo> &outputs() const;

  const PreprocessConfig &preprocess() const;

  const PostprocessConfig &postprocess() const;

  void setInputs(const std::vector<TensorInfo> &inputs);

  void setOutputs(const std::vector<TensorInfo> &outputs);

  void print() const;

private:
  std::string model_dir_;

  std::string model_file_;

  std::string task_;

  std::vector<TensorInfo> inputs_;

  std::vector<TensorInfo> outputs_;

  PreprocessConfig preprocess_;

  PostprocessConfig postprocess_;
};
