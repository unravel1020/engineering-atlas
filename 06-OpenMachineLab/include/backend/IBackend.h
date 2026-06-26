#pragma once

#include "IModelSession.h"
#include <memory>
#include <string>
#include <vector>

namespace oml {
namespace backend {

// Backend initialization configuration. Concrete backends extend this with
// their own device-specific fields.
struct BackendConfig {
  // Common fields
  std::string device = "cpu"; // "cpu", "cuda:0", "tensorrt", etc.
  int threads = 1;

  // Optional backend-specific tuning knobs, parsed from registry.json.
  std::string precision = "fp32";  // fp32 / fp16 / int8
  std::string cache_dir;           // engine/cache directory (e.g. TensorRT)
};

// Abstract interface for all inference backends. The core pipeline depends only
// on this interface, never on a concrete framework.
class IBackend {
public:
  virtual ~IBackend() = default;

  // One-time runtime initialization. May allocate devices, load runtime libs.
  virtual bool initialize(const BackendConfig &config) = 0;

  // Load a model from disk and return a session handle.
  virtual ModelSessionPtr loadModel(const std::string &model_path,
                                    const std::string &model_config_path) = 0;

  // Query backend capabilities.
  virtual std::string getName() const = 0;
  virtual std::vector<std::string> getSupportedDevices() const = 0;

  // Release global backend resources.
  virtual void shutdown() = 0;
};

using BackendPtr = std::shared_ptr<IBackend>;

} // namespace backend
} // namespace oml
