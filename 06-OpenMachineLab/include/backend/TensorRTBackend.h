#pragma once

#include "IBackend.h"
#include "IModelSession.h"
#include <memory>
#include <string>

namespace oml {
namespace backend {

// TensorRT / GPU backend placeholder.
//
// This backend registers under the name "tensorrt" so registry.json can target
// GPU execution, but it does not link against the TensorRT SDK. It is intended
// as an integration scaffold: a future implementation can swap the stub
// methods for real TensorRT engine building and inference while keeping the
// same IBackend/IModelSession contract.
class TensorRTBackend : public IBackend {
public:
  TensorRTBackend();
  ~TensorRTBackend() override;

  bool initialize(const BackendConfig &config) override;
  ModelSessionPtr loadModel(const std::string &model_path,
                            const std::string &model_config_path) override;
  std::string getName() const override;
  std::vector<std::string> getSupportedDevices() const override;
  void shutdown() override;

private:
  BackendConfig config_;
  bool initialized_ = false;
};

// Placeholder model session. The TensorRT backend intentionally does not load
// real models in this skeleton; run() throws if it is ever invoked.
class TensorRTModelSession : public IModelSession {
public:
  std::vector<TensorInfo> getInputInfos() const override;
  std::vector<TensorInfo> getOutputInfos() const override;

  std::unordered_map<std::string, std::vector<float>>
  run(const std::unordered_map<std::string, std::vector<float>> &inputs) override;

  void release() override;
};

} // namespace backend
} // namespace oml
