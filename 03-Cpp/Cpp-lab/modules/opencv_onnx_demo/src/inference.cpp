#include "inference.h"
#include "preProcessor.h"
#include "utils.h"
#include <iostream>
#include <stdexcept>

Inference::Inference(const ModelInfo &model_info)
    : env_(ORT_LOGGING_LEVEL_WARNING, "demo"), session_(nullptr),
      options_(), model_info_(model_info) {
  options_.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_ALL);

  session_ = Ort::Session(env_, model_info_.modelPath().c_str(), options_);

  syncTensorShape();
}

void Inference::syncTensorShape() {
  Ort::AllocatorWithDefaultOptions allocator;

  std::vector<TensorInfo> inputs = model_info_.inputs();
  for (size_t i = 0; i < inputs.size(); ++i) {
    auto typeInfo = session_.GetInputTypeInfo(i);
    auto tensorInfo = typeInfo.GetTensorTypeAndShapeInfo();
    inputs[i].shape = tensorInfo.GetShape();
    inputs[i].type = tensorInfo.GetElementType();
  }
  model_info_.setInputs(inputs);

  std::vector<TensorInfo> outputs = model_info_.outputs();
  for (size_t i = 0; i < outputs.size(); ++i) {
    auto typeInfo = session_.GetOutputTypeInfo(i);
    auto tensorInfo = typeInfo.GetTensorTypeAndShapeInfo();
    outputs[i].shape = tensorInfo.GetShape();
    outputs[i].type = tensorInfo.GetElementType();
  }
  model_info_.setOutputs(outputs);
}

std::vector<int64_t> Inference::fixInputShape(const std::vector<int64_t> &shape,
                                              int c, int h, int w) const {
  std::vector<int64_t> result = shape;

  if (result.size() == 4) {
    if (result[0] <= 0) {
      result[0] = 1;
    }
    if (result[1] <= 0) {
      result[1] = c;
    }
    if (result[2] <= 0) {
      result[2] = h;
    }
    if (result[3] <= 0) {
      result[3] = w;
    }
  }

  return result;
}

std::unordered_map<std::string, std::vector<float>>
Inference::run(const std::unordered_map<std::string, cv::Mat> &input_images) {
  struct InputData {
    std::vector<float> values;
    std::vector<int64_t> shape;
  };

  std::vector<InputData> input_datas;
  std::vector<const char *> input_names;

  for (const auto &info : model_info_.inputs()) {
    auto it = input_images.find(info.name);
    if (it == input_images.end()) {
      throw std::runtime_error("Missing input tensor: " + info.name);
    }

    int c, h, w;
    InputData data;
    data.values = preProcessor::mat_to_tensor(it->second, info,
                                              model_info_.preprocess(), c, h, w);
    data.shape = fixInputShape(info.shape, c, h, w);

    input_datas.push_back(std::move(data));
    input_names.push_back(info.name.c_str());
  }

  Ort::MemoryInfo memory_info =
      Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);

  std::vector<Ort::Value> input_tensors;
  for (auto &data : input_datas) {
    input_tensors.push_back(Ort::Value::CreateTensor<float>(
        memory_info, data.values.data(), data.values.size(),
        data.shape.data(), data.shape.size()));
  }

  std::vector<const char *> output_names;
  for (const auto &info : model_info_.outputs()) {
    output_names.push_back(info.name.c_str());
  }

  auto output_values = session_.Run(
      Ort::RunOptions{nullptr}, input_names.data(), input_tensors.data(),
      input_tensors.size(), output_names.data(), output_names.size());

  std::unordered_map<std::string, std::vector<float>> outputs;
  for (size_t i = 0; i < output_values.size(); ++i) {
    float *out = output_values[i].GetTensorMutableData<float>();
    size_t out_size =
        output_values[i].GetTensorTypeAndShapeInfo().GetElementCount();
    outputs[model_info_.outputs()[i].name] =
        std::vector<float>(out, out + out_size);
  }

  return outputs;
}

std::vector<float> Inference::run(const cv::Mat &img) {
  if (model_info_.inputs().empty() || model_info_.outputs().empty()) {
    throw std::runtime_error("Model has no inputs or outputs");
  }

  auto outputs = run({{model_info_.inputs()[0].name, img}});
  return outputs[model_info_.outputs()[0].name];
}

void Inference::printModelInfo() const { model_info_.print(); }

const ModelInfo &Inference::modelInfo() const { return model_info_; }
