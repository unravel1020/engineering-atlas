#pragma once

#include "ConfigManager.h"
#include <memory>
#include <string>

namespace oml {
namespace backend {
class BackendRegistry;
} // namespace backend

namespace core {

// Application owns the top-level lifecycle of OpenMachineLab.
// It orchestrates backend initialization, model loading, and shutdown.
class Application {
public:
  // Factory method ensures proper two-phase initialization.
  static std::unique_ptr<Application> create(const std::string &config_path);

  ~Application();

  // Lifecycle methods. Callers must follow create -> initialize -> run ->
  // shutdown order, or use create() which performs all steps for simple demos.
  bool initialize();
  int run();
  void shutdown();

  const ConfigManager &configManager() const { return *config_manager_; }

private:
  Application() = default;

  std::unique_ptr<ConfigManager> config_manager_;
  bool initialized_ = false;
};

} // namespace core
} // namespace oml
