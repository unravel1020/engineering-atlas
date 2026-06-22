#include "inference.h"
#include "postProcessor.h"
#include "preProcessor.h"
#include "utils.h"
#include <iostream>

Inference::Inference(const std::string &model_path)
    : env(ORT_LOGGING_LEVEL_WARNING, "demo"), session(nullptr) {
  options.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_ALL);

  session = Ort::Session(env, model_path.c_str(), options);

  model_info_ = buildModelInfo(model_path);
}

ModelInfo Inference::buildModelInfo(const std::string &model_path) {
  Ort::AllocatorWithDefaultOptions allocator;

  std::vector<TensorInfo> inputs;

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

  std::vector<TensorInfo> outputs;

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

  return ModelInfo(model_path, inputs, outputs);
}

std::vector<float> Inference::run(const cv::Mat &img) {

  int c, h, w;

  int target_h = static_cast<int>(model_info_.inputs()[0].shape[2]);

  int target_w = static_cast<int>(model_info_.inputs()[0].shape[3]);

  auto input_tensor_values =
      preProcessor::mat_to_tensor_nchw(img, c, h, w, target_h, target_w);

  std::cout << "Model Input Shape: " << target_h << " x " << target_w
            << std::endl;

  std::vector<int64_t> input_shape = {1, c, h, w};

  Ort::MemoryInfo memory_info =
      Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);

  auto input_tensor = Ort::Value::CreateTensor<float>(
      memory_info, input_tensor_values.data(), input_tensor_values.size(),
      input_shape.data(), input_shape.size());

  const char *input_names[] = {model_info_.inputs()[0].name.c_str()};
  const char *output_names[] = {model_info_.outputs()[0].name.c_str()};

  auto output = session.Run(Ort::RunOptions{nullptr}, input_names,
                            &input_tensor, 1, output_names, 1);

  float *out = output[0].GetTensorMutableData<float>();

  size_t out_size = output[0].GetTensorTypeAndShapeInfo().GetElementCount();

  return std::vector<float>(out, out + out_size);
}

void Inference::printModelInfo() const {
  model_info_.print();
}
