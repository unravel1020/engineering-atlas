#pragma once

#include <string>
#include <unordered_map>
#include <vector>

struct ModelEntry {
  std::string task;
  std::string dir;
};

class ModelRegistry {
public:
  static ModelRegistry load(const std::string &registry_path);

  std::vector<std::string> modelNames() const;

  std::string modelDir(const std::string &name) const;

  std::string task(const std::string &name) const;

  bool contains(const std::string &name) const;

private:
  ModelRegistry(std::string base_dir,
                std::unordered_map<std::string, ModelEntry> models);

  std::string base_dir_;

  std::unordered_map<std::string, ModelEntry> models_;
};
