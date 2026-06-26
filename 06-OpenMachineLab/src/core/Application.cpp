#include "core/Application.h"
#include "backend/BackendRegistry.h"
#include "backend/IBackend.h"
#include "backend/IModelSession.h"
#include "pipeline/InferenceStage.h"
#include "pipeline/PostprocessStage.h"
#include "pipeline/PreprocessStage.h"
#include "pipeline/SequentialPipeline.h"
#include "utils.h"
#include <stdexcept>

namespace oml {
namespace core {

std::unique_ptr<Application>
Application::create(const std::string &config_path) {
  auto app = std::unique_ptr<Application>(new Application());
  app->config_manager_ = std::make_unique<ConfigManager>();
  app->config_manager_->loadGlobal(config_path);

  // Derive base directory from config path so model directories can be resolved
  // relative to the config file location, independent of the working directory.
  std::string base_dir = ".";
  size_t last_slash = config_path.find_last_of("/\\");
  if (last_slash != std::string::npos) {
    base_dir = config_path.substr(0, last_slash);
  }
  app->config_base_dir_ = base_dir;

  // Load per-model configs eagerly so errors surface during initialization.
  for (const auto &entry : app->config_manager_->global().models) {
    app->config_manager_->loadModelConfig(
        utils::joinPath(base_dir, entry.dir), entry.name);
  }

  return app;
}

Application::~Application() { shutdown(); }

bool Application::initialize() {
  if (initialized_) {
    return true;
  }

  const auto &global = config_manager_->global();

  // Initialize all declared backends. If none are declared, fall back to the
  // default backend so existing registry.json files remain compatible.
  if (global.backends.empty()) {
    auto backend = backend::BackendRegistry::instance().createBackend(global.default_backend);
    if (!backend) {
      throw std::runtime_error("Default backend not registered: " +
                               global.default_backend);
    }
    backend::BackendConfig cfg;
    cfg.device = "cpu";
    cfg.threads = global.worker_threads;
    if (!backend->initialize(cfg)) {
      throw std::runtime_error("Failed to initialize default backend: " +
                               global.default_backend);
    }
    backends_[global.default_backend] = std::move(backend);
  } else {
    for (const auto &backend_cfg : global.backends) {
      auto backend =
          backend::BackendRegistry::instance().createBackend(backend_cfg.name);
      if (!backend) {
        throw std::runtime_error("Backend not registered: " + backend_cfg.name);
      }

      backend::BackendConfig cfg;
      cfg.device = backend_cfg.device;
      cfg.threads = backend_cfg.threads;
      if (!backend->initialize(cfg)) {
        throw std::runtime_error("Failed to initialize backend: " +
                                 backend_cfg.name);
      }

      backends_[backend_cfg.name] = std::move(backend);
    }
  }

  // Load model sessions.
  for (const auto &entry : global.models) {
    auto backend_it = backends_.find(entry.backend);
    if (backend_it == backends_.end()) {
      throw std::runtime_error("Backend '" + entry.backend +
                               "' not available for model '" + entry.name + "'");
    }

    const auto &model_cfg = config_manager_->modelConfig(entry.name);
    std::string model_path =
        utils::joinPath(utils::joinPath(config_base_dir_, entry.dir),
                        model_cfg.model_file);

    auto session = backend_it->second->loadModel(model_path, "");
    if (!session) {
      throw std::runtime_error("Failed to load model: " + entry.name);
    }

    sessions_[entry.name] = std::move(session);
  }

  initialized_ = true;
  return true;
}

std::unique_ptr<pipeline::SequentialPipeline>
Application::createPipeline(const std::string &model_name) const {
  if (!initialized_) {
    throw std::runtime_error("Application is not initialized");
  }

  auto session_it = sessions_.find(model_name);
  if (session_it == sessions_.end()) {
    throw std::runtime_error("Model not loaded: " + model_name);
  }

  const auto &model_cfg = config_manager_->modelConfig(model_name);

  // Find the model directory for labels and test data.
  std::string model_dir;
  for (const auto &entry : config_manager_->global().models) {
    if (entry.name == model_name) {
      model_dir = utils::joinPath(config_base_dir_, entry.dir);
      break;
    }
  }

  auto pipe = std::make_unique<pipeline::SequentialPipeline>();
  pipe->addStage(std::make_shared<pipeline::PreprocessStage>(model_cfg));
  pipe->addStage(std::make_shared<pipeline::InferenceStage>(session_it->second));
  pipe->addStage(
      std::make_shared<pipeline::PostprocessStage>(model_cfg, model_dir));
  return pipe;
}

std::shared_ptr<backend::IModelSession>
Application::getSession(const std::string &model_name) const {
  auto it = sessions_.find(model_name);
  if (it == sessions_.end()) {
    return nullptr;
  }
  return it->second;
}

int Application::run() {
  if (!initialized_) {
    throw std::runtime_error("Application is not initialized");
  }
  // The first version of Application::run is a no-op entry point. Concrete
  // service loops and batch processing will be added once the pipeline and
  // scheduling abstractions are fully validated.
  return 0;
}

void Application::shutdown() {
  sessions_.clear();
  backends_.clear();
  initialized_ = false;
}

} // namespace core
} // namespace oml
