#pragma once

#include <string>
#include <unordered_map>
#include <vector>

class ModelRegistry {
public:
  static ModelRegistry load(const std::string &registry_path);

  std::vector<std::string> modelNames() const;

  std::string modelDir(const std::string &name) const;

  bool contains(const std::string &name) const;

private:
  ModelRegistry(std::string base_dir,
                std::unordered_map<std::string, std::string> model_dirs);

  std::string base_dir_;

  std::unordered_map<std::string, std::string> model_dirs_;
};
