#include "modelInfo.h"
#include <iostream>

ModelInfo::ModelInfo(const std::string &model_dir,
                     const std::string &model_file,
                     const std::string &task,
                     const std::vector<TensorInfo> &inputs,
                     const std::vector<TensorInfo> &outputs,
                     const PreprocessConfig &preprocess,
                     const PostprocessConfig &postprocess)
    : model_dir_(model_dir), model_file_(model_file), task_(task),
      inputs_(inputs), outputs_(outputs), preprocess_(preprocess),
      postprocess_(postprocess) {}

const std::string &ModelInfo::modelDir() const { return model_dir_; }

std::string ModelInfo::modelPath() const { return model_dir_ + "/" + model_file_; }

const std::string &ModelInfo::task() const { return task_; }

const std::vector<TensorInfo> &ModelInfo::inputs() const { return inputs_; }

const std::vector<TensorInfo> &ModelInfo::outputs() const { return outputs_; }

const PreprocessConfig &ModelInfo::preprocess() const { return preprocess_; }

const PostprocessConfig &ModelInfo::postprocess() const { return postprocess_; }

void ModelInfo::setInputs(const std::vector<TensorInfo> &inputs) {
  inputs_ = inputs;
}

void ModelInfo::setOutputs(const std::vector<TensorInfo> &outputs) {
  outputs_ = outputs;
}

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
