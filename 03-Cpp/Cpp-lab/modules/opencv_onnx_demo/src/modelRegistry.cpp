#include "modelRegistry.h"
#include "json/json.hpp"
#include <fstream>

using json = nlohmann::json;

ModelRegistry::ModelRegistry(
    std::string base_dir,
    std::unordered_map<std::string, std::string> model_dirs)
    : base_dir_(std::move(base_dir)), model_dirs_(std::move(model_dirs)) {}

ModelRegistry ModelRegistry::load(const std::string &registry_path) {
  size_t last_slash = registry_path.find_last_of("/\\");
  std::string base_dir =
      (last_slash == std::string::npos) ? "." : registry_path.substr(0, last_slash);

  std::ifstream ifs(registry_path);

  json j;
  ifs >> j;

  std::unordered_map<std::string, std::string> model_dirs;
  for (auto it = j["models"].begin(); it != j["models"].end(); ++it) {
    std::string name = it.key();
    std::string dir = it.value().value("dir", name);
    model_dirs[name] = base_dir + "/" + dir;
  }

  return ModelRegistry(base_dir, model_dirs);
}

std::vector<std::string> ModelRegistry::modelNames() const {
  std::vector<std::string> names;
  for (const auto &pair : model_dirs_) {
    names.push_back(pair.first);
  }
  return names;
}

std::string ModelRegistry::modelDir(const std::string &name) const {
  auto it = model_dirs_.find(name);
  if (it == model_dirs_.end()) {
    return "";
  }
  return it->second;
}

bool ModelRegistry::contains(const std::string &name) const {
  return model_dirs_.find(name) != model_dirs_.end();
}
