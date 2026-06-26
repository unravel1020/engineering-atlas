#include "backend/BackendRegistry.h"
#include "backend/TensorRTBackend.h"
#include <stdexcept>

namespace oml {
namespace backend {

// ------------------------------------------------------------------
// TensorRTBackend
// ------------------------------------------------------------------

TensorRTBackend::TensorRTBackend() = default;
TensorRTBackend::~TensorRTBackend() { shutdown(); }

bool TensorRTBackend::initialize(const BackendConfig &config) {
  config_ = config;
  initialized_ = true;
  return true;
}

ModelSessionPtr TensorRTBackend::loadModel(const std::string & /*model_path*/,
                                           const std::string & /*model_config_path*/) {
  // This is a compile-time placeholder. A real implementation would build or
  // deserialize a TensorRT engine here using config_.device, config_.precision,
  // and config_.cache_dir.
  throw std::runtime_error(
      "TensorRT backend is a placeholder and not linked against the TensorRT SDK");
}

std::string TensorRTBackend::getName() const { return "tensorrt"; }

std::vector<std::string> TensorRTBackend::getSupportedDevices() const {
  return {"cuda:0", "cuda"};
}

void TensorRTBackend::shutdown() { initialized_ = false; }

// ------------------------------------------------------------------
// TensorRTModelSession
// ------------------------------------------------------------------

std::vector<TensorInfo> TensorRTModelSession::getInputInfos() const {
  throw std::runtime_error("TensorRTModelSession is a placeholder");
}

std::vector<TensorInfo> TensorRTModelSession::getOutputInfos() const {
  throw std::runtime_error("TensorRTModelSession is a placeholder");
}

std::unordered_map<std::string, std::vector<float>>
TensorRTModelSession::run(const std::unordered_map<std::string, std::vector<float>> & /*inputs*/) {
  throw std::runtime_error("TensorRTModelSession::run is a placeholder");
}

void TensorRTModelSession::release() {}

REGISTER_BACKEND(tensorrt, TensorRTBackend);

} // namespace backend
} // namespace oml
