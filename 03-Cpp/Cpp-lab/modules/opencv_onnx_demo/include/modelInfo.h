#pragma once

#include <onnxruntime_cxx_api.h>
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
};

struct PostprocessConfig {
  std::string activation = "softmax";

  int topk = 5;

  std::string labels_file;
};

class ModelInfo {
public:
  ModelInfo() = default;

  ModelInfo(const std::string &model_dir, const std::string &model_file,
            const std::vector<TensorInfo> &inputs,
            const std::vector<TensorInfo> &outputs,
            const PreprocessConfig &preprocess,
            const PostprocessConfig &postprocess);

  const std::string &modelDir() const;

  std::string modelPath() const;

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

  std::vector<TensorInfo> inputs_;

  std::vector<TensorInfo> outputs_;

  PreprocessConfig preprocess_;

  PostprocessConfig postprocess_;
};
