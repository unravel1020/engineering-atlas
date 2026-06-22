#include "modelManager.h"
#include "modelLoader.h"
#include <stdexcept>

ModelManager::ModelManager(const std::string &registry_path)
    : registry_(ModelRegistry::load(registry_path)) {}

std::shared_ptr<Inference> ModelManager::load(const std::string &name) {
  if (!registry_.contains(name)) {
    throw std::runtime_error("Model not found in registry: " + name);
  }

  auto it = models_.find(name);
  if (it != models_.end()) {
    return it->second;
  }

  auto model_dir = registry_.modelDir(name);
  auto model_info = ModelLoader::load(model_dir, name);
  auto model = std::make_shared<Inference>(model_info);
  models_[name] = model;
  return model;
}

std::shared_ptr<Inference> ModelManager::get(const std::string &name) const {
  auto it = models_.find(name);
  if (it != models_.end()) {
    return it->second;
  }
  return nullptr;
}

void ModelManager::select(const std::string &name) {
  current_ = load(name);
}

std::shared_ptr<Inference> ModelManager::current() const { return current_; }

std::vector<std::string> ModelManager::modelNames() const {
  return registry_.modelNames();
}

bool ModelManager::hasModel(const std::string &name) const {
  return registry_.contains(name);
}

std::string ModelManager::task(const std::string &name) const {
  return registry_.task(name);
}

std::vector<float> ModelManager::run(const cv::Mat &img) {
  if (!current_) {
    throw std::runtime_error("No model selected");
  }
  return current_->run(img);
}

std::unordered_map<std::string, std::vector<float>>
ModelManager::run(const std::unordered_map<std::string, cv::Mat> &input_images) {
  if (!current_) {
    throw std::runtime_error("No model selected");
  }
  return current_->run(input_images);
}
