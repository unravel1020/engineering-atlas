#pragma once

#include <string>
#include <unordered_map>
#include <vector>

struct ModelEntry {
  std::string task;
  std::string dir;
};

// ModelRegistry parses the top-level registry.json and resolves model names to
// their on-disk directories. It does not touch ONNX files itself, keeping
// registry loading lightweight and independent of ONNX Runtime.
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
