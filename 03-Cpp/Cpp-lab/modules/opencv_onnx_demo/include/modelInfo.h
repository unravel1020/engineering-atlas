#pragma once

#include <onnxruntime_cxx_api.h>
#include <string>
#include <vector>

struct TensorInfo {
  std::string name;

  std::vector<int64_t> shape;

  ONNXTensorElementDataType type;
};

class ModelInfo {
public:
  ModelInfo() = default;

  ModelInfo(const std::string &model_path,
            const std::vector<TensorInfo> &inputs,
            const std::vector<TensorInfo> &outputs);

  const std::string &modelPath() const;

  const std::vector<TensorInfo> &inputs() const;

  const std::vector<TensorInfo> &outputs() const;

  void print() const;

private:
  std::string model_path_;

  std::vector<TensorInfo> inputs_;

  std::vector<TensorInfo> outputs_;
};
