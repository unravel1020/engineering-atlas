#pragma once

#include "TensorInfo.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace oml {
namespace backend {

// A model session represents one loaded model instance on a specific backend.
// It is intentionally decoupled from IBackend so sessions can be created,
// moved and destroyed independently.
class IModelSession {
public:
  virtual ~IModelSession() = default;

  // Metadata queried from the loaded model. The pipeline uses these to
  // configure preprocessing and postprocessing.
  virtual std::vector<TensorInfo> getInputInfos() const = 0;
  virtual std::vector<TensorInfo> getOutputInfos() const = 0;

  // Run one inference iteration. Inputs and outputs are keyed by tensor name.
  virtual std::unordered_map<std::string, std::vector<float>>
  run(const std::unordered_map<std::string, std::vector<float>> &inputs) = 0;

  // Release backend-native resources. After release() the session must not be
  // used again; calling it twice is a no-op.
  virtual void release() = 0;
};

using ModelSessionPtr = std::shared_ptr<IModelSession>;

} // namespace backend
} // namespace oml
