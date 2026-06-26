#include "backend/ONNXBackend.h"
#include "backend/BackendRegistry.h"
#include <algorithm>
#include <filesystem>
#include <functional>
#include <numeric>
#include <stdexcept>

namespace oml {
namespace backend {

namespace {

// Map ONNX element type to a backend-agnostic dtype string.
std::string onnxTypeToString(ONNXTensorElementDataType type) {
  switch (type) {
  case ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT:
    return "float32";
  case ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT16:
    return "float16";
  case ONNX_TENSOR_ELEMENT_DATA_TYPE_INT32:
    return "int32";
  case ONNX_TENSOR_ELEMENT_DATA_TYPE_INT64:
    return "int64";
  default:
    return "unknown";
  }
}

std::vector<TensorInfo> fetchTensorInfos(const Ort::Session &session,
                                         bool inputs) {
  Ort::AllocatorWithDefaultOptions allocator;
  std::vector<TensorInfo> infos;

  const size_t count =
      inputs ? session.GetInputCount() : session.GetOutputCount();
  infos.reserve(count);

  for (size_t i = 0; i < count; ++i) {
    auto type_info =
        inputs ? session.GetInputTypeInfo(i) : session.GetOutputTypeInfo(i);
    auto tensor_info = type_info.GetTensorTypeAndShapeInfo();

    TensorInfo info;
    info.name = inputs ? session.GetInputNameAllocated(i, allocator).get()
                       : session.GetOutputNameAllocated(i, allocator).get();
    info.shape = tensor_info.GetShape();
    info.dtype = onnxTypeToString(tensor_info.GetElementType());
    infos.push_back(std::move(info));
  }

  return infos;
}

} // namespace

// ------------------------------------------------------------------
// ONNXBackend
// ------------------------------------------------------------------

ONNXBackend::ONNXBackend() = default;
ONNXBackend::~ONNXBackend() { shutdown(); }

bool ONNXBackend::initialize(const BackendConfig &config) {
  if (initialized_) {
    return true;
  }

  env_ = std::make_shared<Ort::Env>(ORT_LOGGING_LEVEL_WARNING, "OpenMachineLab");
  options_.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_ALL);

  // Honor thread hint if provided; ONNX Runtime uses 0 as default.
  if (config.threads > 0) {
    options_.SetIntraOpNumThreads(config.threads);
  }

  initialized_ = true;
  return true;
}

ModelSessionPtr ONNXBackend::loadModel(const std::string &model_path,
                                       const std::string &model_config_path) {
  if (!initialized_) {
    throw std::runtime_error("ONNXBackend is not initialized");
  }
  if (!std::filesystem::exists(model_path)) {
    throw std::runtime_error("ONNX model file not found: " + model_path);
  }

  return std::make_shared<ONNXModelSession>(env_, options_, model_path);
}

std::string ONNXBackend::getName() const { return "onnx"; }

std::vector<std::string> ONNXBackend::getSupportedDevices() const {
  return {"cpu"};
}

void ONNXBackend::shutdown() {
  env_.reset();
  initialized_ = false;
}

// ------------------------------------------------------------------
// ONNXModelSession
// ------------------------------------------------------------------

ONNXModelSession::ONNXModelSession(std::shared_ptr<Ort::Env> env,
                                   const Ort::SessionOptions &options,
                                   const std::string &model_path)
    : env_(std::move(env)) {
  session_ = std::make_unique<Ort::Session>(*env_, model_path.c_str(), options);
  input_infos_ = fetchTensorInfos(*session_, true);
  output_infos_ = fetchTensorInfos(*session_, false);
}

std::vector<TensorInfo> ONNXModelSession::getInputInfos() const {
  return input_infos_;
}

std::vector<TensorInfo> ONNXModelSession::getOutputInfos() const {
  return output_infos_;
}

std::unordered_map<std::string, std::vector<float>>
ONNXModelSession::run(const std::unordered_map<std::string, std::vector<float>> &inputs) {
  Ort::MemoryInfo memory_info =
      Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
  Ort::AllocatorWithDefaultOptions allocator;

  std::vector<std::vector<float>> mutable_inputs;
  mutable_inputs.reserve(input_infos_.size());
  std::vector<Ort::Value> input_tensors;
  std::vector<const char *> input_names;
  input_tensors.reserve(input_infos_.size());
  input_names.reserve(input_infos_.size());

  for (const auto &info : input_infos_) {
    auto it = inputs.find(info.name);
    if (it == inputs.end()) {
      throw std::runtime_error("Missing input tensor: " + info.name);
    }

    // Build concrete shape: replace symbolic or non-positive dimensions with 1.
    std::vector<int64_t> shape = info.shape;
    if (shape.size() == 4) {
      for (auto &dim : shape) {
        if (dim <= 0) {
          dim = 1;
        }
      }
    }

    const size_t expected = std::accumulate(
        shape.begin(), shape.end(), size_t{1}, std::multiplies<size_t>());
    if (it->second.size() != expected) {
      throw std::runtime_error("Input tensor '" + info.name + "' size mismatch");
    }

    // CreateTensor borrows the pointer, so we keep the mutable buffer alive
    // until Run() returns.
    mutable_inputs.push_back(it->second);
    input_tensors.push_back(Ort::Value::CreateTensor<float>(
        memory_info, mutable_inputs.back().data(), mutable_inputs.back().size(),
        shape.data(), shape.size()));
    input_names.push_back(info.name.c_str());
  }

  std::vector<const char *> output_names;
  output_names.reserve(output_infos_.size());
  for (const auto &info : output_infos_) {
    output_names.push_back(info.name.c_str());
  }

  auto output_values = session_->Run(
      Ort::RunOptions{nullptr}, input_names.data(), input_tensors.data(),
      input_tensors.size(), output_names.data(), output_names.size());

  std::unordered_map<std::string, std::vector<float>> outputs;
  for (size_t i = 0; i < output_values.size(); ++i) {
    const std::string &name = output_infos_[i].name;
    auto type_info = output_values[i].GetTensorTypeAndShapeInfo();
    size_t count = type_info.GetElementCount();
    auto element_type = type_info.GetElementType();

    if (element_type == ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT) {
      float *data = output_values[i].GetTensorMutableData<float>();
      outputs[name] = std::vector<float>(data, data + count);
    } else if (element_type == ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT16) {
      Ort::Float16_t *data =
          output_values[i].GetTensorMutableData<Ort::Float16_t>();
      std::vector<float> f32(count);
      for (size_t j = 0; j < count; ++j) {
        f32[j] = static_cast<float>(data[j]);
      }
      outputs[name] = std::move(f32);
    } else {
      throw std::runtime_error("Unsupported output element type for tensor '" +
                               name + "'");
    }
  }

  return outputs;
}

void ONNXModelSession::release() { session_.reset(); }

REGISTER_BACKEND(onnx, ONNXBackend);

} // namespace backend
} // namespace oml
