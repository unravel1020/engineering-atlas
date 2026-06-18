#include "inference.h"
#include "postProcessor.h"
#include "preProcessor.h"
#include "utils.h"
#include <iostream>

Inference::Inference(const std::string &model_path)
    : env(ORT_LOGGING_LEVEL_WARNING, "demo"), session(nullptr) {
  options.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_ALL);

  session = Ort::Session(env, model_path.c_str(), options);

  init_io_names();

  parseModelInfo();
}

void Inference::parseModelInfo() {
  Ort::AllocatorWithDefaultOptions allocator;

  size_t inputCount = session.GetInputCount();

  for (size_t i = 0; i < inputCount; ++i) {
    TensorInfo info;

    auto name = session.GetInputNameAllocated(i, allocator);

    info.name = name.get();

    auto typeInfo = session.GetInputTypeInfo(i);

    auto tensorInfo = typeInfo.GetTensorTypeAndShapeInfo();

    info.shape = tensorInfo.GetShape();

    info.type = tensorInfo.GetElementType();

    inputs.push_back(info);
  }

  size_t outputCount = session.GetOutputCount();

  for (size_t i = 0; i < outputCount; ++i) {
    TensorInfo info;

    auto name = session.GetOutputNameAllocated(i, allocator);

    info.name = name.get();

    auto typeInfo = session.GetOutputTypeInfo(i);

    auto tensorInfo = typeInfo.GetTensorTypeAndShapeInfo();

    info.shape = tensorInfo.GetShape();

    info.type = tensorInfo.GetElementType();

    outputs.push_back(info);
  }
}

void Inference::init_io_names() {

  Ort::AllocatorWithDefaultOptions allocator;

  input_name = session.GetInputNameAllocated(0, allocator).get();
  output_name = session.GetOutputNameAllocated(0, allocator).get();
}

std::vector<float> Inference::run(const cv::Mat &img) {

  int c, h, w;
  auto input_tensor_values = preProcessor::mat_to_tensor_nchw(img, c, h, w);

  std::vector<int64_t> input_shape = {1, c, h, w};

  Ort::MemoryInfo memory_info =
      Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);

  auto input_tensor = Ort::Value::CreateTensor<float>(
      memory_info, input_tensor_values.data(), input_tensor_values.size(),
      input_shape.data(), input_shape.size());

  const char *input_names[] = {input_name.c_str()};
  const char *output_names[] = {output_name.c_str()};

  auto output = session.Run(Ort::RunOptions{nullptr}, input_names,
                            &input_tensor, 1, output_names, 1);

  float *out = output[0].GetTensorMutableData<float>();

  size_t out_size = output[0].GetTensorTypeAndShapeInfo().GetElementCount();

  return std::vector<float>(out, out + out_size);
} // namespace inference

void Inference::printModelInfo() const {
  std::cout << "\n=== Inputs ===\n";

  for (const auto &input : inputs) {
    std::cout << input.name << " : [";

    for (auto dim : input.shape) {
      std::cout << dim << " ";
    }

    std::cout << "]\n";
  }

  std::cout << "\n=== Outputs ===\n";

  for (const auto &output : outputs) {
    std::cout << output.name << " : [";

    for (auto dim : output.shape) {
      std::cout << dim << " ";
    }

    std::cout << "]\n";
  }
}