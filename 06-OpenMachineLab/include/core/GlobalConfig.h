#pragma once

#include <string>
#include <vector>

namespace oml {
namespace core {

// Backend-specific runtime configuration.
struct BackendConfigEntry {
  std::string name;     // e.g. "onnx_cpu", "tensorrt"
  std::string device;   // e.g. "cpu", "cuda:0"
  int threads = 1;
  // Backend-specific options, kept generic to avoid early over-design.
  std::string cache_dir;
  std::string precision = "fp32"; // fp32 / fp16 / int8
};

// Model entry in the global registry.
struct ModelRegistryEntry {
  std::string name;
  std::string task;
  std::string dir;
  std::string backend; // references BackendConfigEntry::name
};

// Global application configuration.
struct GlobalConfig {
  std::string log_level = "INFO";
  int worker_threads = 1;
  std::string default_backend = "onnx_cpu";

  std::vector<BackendConfigEntry> backends;
  std::vector<ModelRegistryEntry> models;
};

} // namespace core
} // namespace oml
