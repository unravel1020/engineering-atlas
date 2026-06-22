#include "modelInfo.h"
#include <iostream>

ModelInfo::ModelInfo(const std::string &model_path,
                     const std::vector<TensorInfo> &inputs,
                     const std::vector<TensorInfo> &outputs)
    : model_path_(model_path), inputs_(inputs), outputs_(outputs) {}

const std::string &ModelInfo::modelPath() const { return model_path_; }

const std::vector<TensorInfo> &ModelInfo::inputs() const { return inputs_; }

const std::vector<TensorInfo> &ModelInfo::outputs() const { return outputs_; }

void ModelInfo::print() const {
  std::cout << "\n=== Inputs ===\n";

  for (const auto &input : inputs_) {
    std::cout << input.name << " : [";

    for (auto dim : input.shape) {
      std::cout << dim << " ";
    }

    std::cout << "]\n";
  }

  std::cout << "\n=== Outputs ===\n";

  for (const auto &output : outputs_) {
    std::cout << output.name << " : [";

    for (auto dim : output.shape) {
      std::cout << dim << " ";
    }

    std::cout << "]\n";
  }
}
