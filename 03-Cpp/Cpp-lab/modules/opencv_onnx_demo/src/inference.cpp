#include "inference.h"
#include "logger.h"
#include "preProcessor.h"
#include "utils.h"
#include <filesystem>
#include <stdexcept>

Inference::Inference(const ModelInfo &model_info)
    : env_(ORT_LOGGING_LEVEL_WARNING, "demo"), session_(nullptr),
      options_(), model_info_(model_info) {
  options_.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_ALL);

  if (!std::filesystem::exists(model_info_.modelPath())) {
    throw std::runtime_error("ONNX model file not found: " +
                             model_info_.modelPath());
  }

  LOG_INFO("Loading ONNX model: " + model_info_.modelPath());
  session_ = Ort::Session(env_, model_info_.modelPath().c_str(), options_);
  LOG_INFO("ONNX model loaded successfully: " + model_info_.modelPath());

  // ONNX session metadata is authoritative: model.json may contain symbolic
  // shapes such as -1 for batch size, so we synchronize after creating the
  // session to obtain concrete types and dimensions.
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

  // Symbolic or unknown dimensions in the ONNX graph are represented as <= 0.
  // We replace them with the concrete shape produced by the preprocessor.
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

InferenceResult
Inference::runWithInfo(const std::unordered_map<std::string, cv::Mat> &input_images) {
  struct InputData {
    std::vector<float> values_f32;
    std::vector<Ort::Float16_t> values_f16;
    std::vector<int64_t> shape;
    PreprocessInfo preprocess;
    ONNXTensorElementDataType type = ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT;
  };

  std::vector<InputData> input_datas;
  std::vector<const char *> input_names;
  PreprocessInfo first_preprocess;

  for (const auto &info : model_info_.inputs()) {
    auto it = input_images.find(info.name);
    if (it == input_images.end()) {
      throw std::runtime_error("Missing input tensor: " + info.name);
    }
    if (it->second.empty()) {
      throw std::runtime_error("Empty input image for tensor: " + info.name);
    }

    int c, h, w;
    InputData data;
    PreprocessInfo preprocess;
    data.values_f32 = preProcessor::mat_to_tensor(it->second, info,
                                                  model_info_.preprocess(), c, h, w,
                                                  &preprocess);
    data.shape = fixInputShape(info.shape, c, h, w);
    data.preprocess = preprocess;
    data.type = info.type;

    if (data.type != ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT &&
        data.type != ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT16) {
      throw std::runtime_error("Unsupported input data type for tensor '" +
                               info.name + "'");
    }

    if (input_datas.empty()) {
      first_preprocess = preprocess;
    }

    input_datas.push_back(std::move(data));
    input_names.push_back(info.name.c_str());
  }

  Ort::MemoryInfo memory_info =
      Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);

  std::vector<Ort::Value> input_tensors;
  for (auto &data : input_datas) {
    // The downloaded YOLOv8 model is exported with FP16 I/O. ONNX Runtime on
    // CPU still accepts FP16 tensors, so we convert the FP32 preprocessing
    // output to FP16 only when the model explicitly declares FP16 inputs.
    if (data.type == ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT16) {
      data.values_f16.reserve(data.values_f32.size());
      for (float v : data.values_f32) {
        data.values_f16.emplace_back(v);
      }
      input_tensors.push_back(Ort::Value::CreateTensor<Ort::Float16_t>(
          memory_info, data.values_f16.data(), data.values_f16.size(),
          data.shape.data(), data.shape.size()));
    } else {
      input_tensors.push_back(Ort::Value::CreateTensor<float>(
          memory_info, data.values_f32.data(), data.values_f32.size(),
          data.shape.data(), data.shape.size()));
    }
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
    const std::string &name = model_info_.outputs()[i].name;
    auto type_info = output_values[i].GetTensorTypeAndShapeInfo();
    size_t out_size = type_info.GetElementCount();
    auto element_type = type_info.GetElementType();

    // Internal pipeline code works in FP32, so FP16 outputs are converted back
    // immediately after inference. This keeps post-processing independent of
    // the model's I/O precision.
    if (element_type == ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT16) {
      Ort::Float16_t *out = output_values[i].GetTensorMutableData<Ort::Float16_t>();
      std::vector<float> out_f32(out_size);
      for (size_t j = 0; j < out_size; ++j) {
        out_f32[j] = static_cast<float>(out[j]);
      }
      outputs[name] = std::move(out_f32);
    } else if (element_type == ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT) {
      float *out = output_values[i].GetTensorMutableData<float>();
      outputs[name] = std::vector<float>(out, out + out_size);
    } else {
      throw std::runtime_error("Unsupported output element type for tensor '" +
                               name + "'");
    }
  }

  return InferenceResult{outputs, first_preprocess};
}

InferenceResult Inference::runWithInfo(const cv::Mat &img) {
  if (model_info_.inputs().empty() || model_info_.outputs().empty()) {
    throw std::runtime_error("Model has no inputs or outputs");
  }

  return runWithInfo({{model_info_.inputs()[0].name, img}});
}

std::unordered_map<std::string, std::vector<float>>
Inference::run(const std::unordered_map<std::string, cv::Mat> &input_images) {
  return runWithInfo(input_images).outputs;
}

std::vector<float> Inference::run(const cv::Mat &img) {
  auto result = runWithInfo(img);
  const std::string &name = model_info_.outputs()[0].name;
  auto it = result.outputs.find(name);
  if (it == result.outputs.end()) {
    throw std::runtime_error("Missing output tensor: " + name);
  }
  return it->second;
}

void Inference::printModelInfo() const { model_info_.print(); }

const ModelInfo &Inference::modelInfo() const { return model_info_; }
