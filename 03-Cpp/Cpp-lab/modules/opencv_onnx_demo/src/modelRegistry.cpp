#include "modelRegistry.h"
#include "json/json.hpp"
#include <fstream>

using json = nlohmann::json;

ModelRegistry::ModelRegistry(
    std::string base_dir,
    std::unordered_map<std::string, ModelEntry> models)
    : base_dir_(std::move(base_dir)), models_(std::move(models)) {}

ModelRegistry ModelRegistry::load(const std::string &registry_path) {
  size_t last_slash = registry_path.find_last_of("/\\");
  std::string base_dir =
      (last_slash == std::string::npos) ? "." : registry_path.substr(0, last_slash);

  std::ifstream ifs(registry_path);

  json j;
  ifs >> j;

  std::unordered_map<std::string, ModelEntry> models;
  for (auto it = j["models"].begin(); it != j["models"].end(); ++it) {
    ModelEntry entry;
    entry.task = it.value().value("task", "unknown");
    entry.dir = it.value().value("dir", it.key());
    models[it.key()] = entry;
  }

  return ModelRegistry(base_dir, models);
}

std::vector<std::string> ModelRegistry::modelNames() const {
  std::vector<std::string> names;
  for (const auto &pair : models_) {
    names.push_back(pair.first);
  }
  return names;
}

std::string ModelRegistry::modelDir(const std::string &name) const {
  auto it = models_.find(name);
  if (it == models_.end()) {
    return "";
  }
  return base_dir_ + "/" + it->second.dir;
}

std::string ModelRegistry::task(const std::string &name) const {
  auto it = models_.find(name);
  if (it == models_.end()) {
    return "";
  }
  return it->second.task;
}

bool ModelRegistry::contains(const std::string &name) const {
  return models_.find(name) != models_.end();
}
