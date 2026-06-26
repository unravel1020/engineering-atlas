#pragma once

#include "GlobalConfig.h"
#include "ModelConfig.h"
#include <stdexcept>
#include <string>
#include <unordered_map>

namespace oml {
namespace core {

// Thrown when configuration files are missing or invalid.
class ConfigError : public std::runtime_error {
public:
  explicit ConfigError(const std::string &msg) : std::runtime_error(msg) {}
};

// Loads and validates global and per-model configurations.
class ConfigManager {
public:
  // Load global config from a JSON file (e.g. registry.json).
  void loadGlobal(const std::string &global_config_path);

  // Load a single model config from its directory.
  void loadModelConfig(const std::string &model_dir,
                       const std::string &model_name);

  const GlobalConfig &global() const { return global_; }
  const ModelConfig &modelConfig(const std::string &name) const;

  bool hasModelConfig(const std::string &name) const;

private:
  GlobalConfig global_;
  std::unordered_map<std::string, ModelConfig> model_configs_;
};

} // namespace core
} // namespace oml
