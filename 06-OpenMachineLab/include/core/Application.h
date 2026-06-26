#pragma once

#include "ConfigManager.h"
#include "backend/IBackend.h"
#include "backend/IModelSession.h"
#include "pipeline/SequentialPipeline.h"
#include <memory>
#include <string>
#include <unordered_map>

namespace oml {
namespace core {

// Application owns the top-level lifecycle of OpenMachineLab.
// It orchestrates configuration loading, backend initialization, model loading,
// pipeline creation, and shutdown.
class Application {
public:
  // Factory method: loads global and per-model configurations.
  static std::unique_ptr<Application> create(const std::string &config_path);

  ~Application();

  // Lifecycle methods.
  bool initialize();
  int run();
  void shutdown();

  // Create a ready-to-run pipeline for a loaded model.
  std::unique_ptr<pipeline::SequentialPipeline>
  createPipeline(const std::string &model_name) const;

  // Direct access to a loaded session for tests and diagnostics.
  std::shared_ptr<backend::IModelSession>
  getSession(const std::string &model_name) const;

  const ConfigManager &configManager() const { return *config_manager_; }
  bool initialized() const { return initialized_; }

private:
  Application() = default;

  std::unique_ptr<ConfigManager> config_manager_;
  std::string config_base_dir_;

  std::unordered_map<std::string, backend::BackendPtr> backends_;
  std::unordered_map<std::string, backend::ModelSessionPtr> sessions_;

  bool initialized_ = false;
};

} // namespace core
} // namespace oml
