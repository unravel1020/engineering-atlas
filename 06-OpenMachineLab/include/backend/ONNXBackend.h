#pragma once

#include "IBackend.h"
#include "IModelSession.h"
#include <onnxruntime_cxx_api.h>
#include <memory>
#include <string>

namespace oml {
namespace backend {

// ONNX Runtime CPU backend implementation.
// This is the first concrete backend for OpenMachineLab and mirrors the
// capabilities proven in opencv_onnx_demo::Inference.
class ONNXBackend : public IBackend {
public:
  ONNXBackend();
  ~ONNXBackend() override;

  bool initialize(const BackendConfig &config) override;
  ModelSessionPtr loadModel(const std::string &model_path,
                            const std::string &model_config_path) override;
  std::string getName() const override;
  std::vector<std::string> getSupportedDevices() const override;
  void shutdown() override;

private:
  std::shared_ptr<Ort::Env> env_;
  Ort::SessionOptions options_;
  bool initialized_ = false;
};

// ONNX Runtime model session. Holds one Ort::Session and resolves tensor
// metadata so the pipeline can run inference without knowing ONNX APIs.
class ONNXModelSession : public IModelSession {
public:
  ONNXModelSession(std::shared_ptr<Ort::Env> env,
                   const Ort::SessionOptions &options,
                   const std::string &model_path);

  std::vector<TensorInfo> getInputInfos() const override;
  std::vector<TensorInfo> getOutputInfos() const override;

  std::unordered_map<std::string, std::vector<float>>
  run(const std::unordered_map<std::string, std::vector<float>> &inputs) override;

  void release() override;

private:
  std::shared_ptr<Ort::Env> env_;
  std::unique_ptr<Ort::Session> session_;
  std::vector<TensorInfo> input_infos_;
  std::vector<TensorInfo> output_infos_;
};

} // namespace backend
} // namespace oml
