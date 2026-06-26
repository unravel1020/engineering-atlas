#include "core/ConfigManager.h"
#include "third_party/json.hpp"
#include "utils.h"

using oml::utils::joinPath;
using oml::utils::readTextFile;
#include <filesystem>
#include <fstream>

namespace oml {
namespace core {

namespace json = nlohmann;
namespace fs = std::filesystem;

namespace {

PreprocessConfig parsePreprocess(const json::json &j) {
  PreprocessConfig cfg;
  if (j.contains("color_format")) {
    cfg.color_format = j["color_format"];
  }
  if (j.contains("resize_mode")) {
    cfg.resize_mode = j["resize_mode"];
  }
  if (j.contains("target_h")) {
    cfg.target_h = j["target_h"];
  }
  if (j.contains("target_w")) {
    cfg.target_w = j["target_w"];
  }
  if (j.contains("scale")) {
    cfg.scale = j["scale"];
  }
  if (j.contains("mean")) {
    cfg.mean = j["mean"].get<std::vector<float>>();
  }
  if (j.contains("std")) {
    cfg.std = j["std"].get<std::vector<float>>();
  }
  if (j.contains("layout")) {
    cfg.layout = j["layout"];
  }
  if (j.contains("pad_color")) {
    cfg.pad_color = j["pad_color"].get<std::vector<uint8_t>>();
  }
  return cfg;
}

PostprocessConfig parsePostprocess(const json::json &j) {
  PostprocessConfig cfg;
  if (j.contains("activation")) {
    cfg.activation = j["activation"];
  }
  if (j.contains("topk")) {
    cfg.topk = j["topk"];
  }
  if (j.contains("labels_file")) {
    cfg.labels_file = j["labels_file"];
  }
  if (j.contains("conf_threshold")) {
    cfg.conf_threshold = j["conf_threshold"];
  }
  if (j.contains("nms_threshold")) {
    cfg.nms_threshold = j["nms_threshold"];
  }
  if (j.contains("num_classes")) {
    cfg.num_classes = j["num_classes"];
  }
  if (j.contains("num_candidates")) {
    cfg.num_candidates = j["num_candidates"];
  }
  if (j.contains("box_format")) {
    cfg.box_format = j["box_format"];
  }
  if (j.contains("output_layout")) {
    cfg.output_layout = j["output_layout"];
  }
  return cfg;
}

TensorConfig parseTensor(const json::json &j, bool is_input) {
  TensorConfig cfg;
  cfg.name = j.value("name", "");
  cfg.dtype = j.value("type", "float32");
  if (is_input && j.contains("preprocess")) {
    cfg.preprocess = parsePreprocess(j["preprocess"]);
  }
  if (!is_input && j.contains("postprocess")) {
    cfg.postprocess = parsePostprocess(j["postprocess"]);
  }
  return cfg;
}

} // namespace

void ConfigManager::loadGlobal(const std::string &global_config_path) {
  if (!fs::exists(global_config_path)) {
    throw ConfigError("global config not found: " + global_config_path);
  }

  json::json j;
  try {
    std::ifstream ifs(global_config_path);
    ifs >> j;
  } catch (const std::exception &e) {
    throw ConfigError(std::string("failed to parse global config: ") + e.what());
  }

  global_.log_level = j.value("log_level", "INFO");
  global_.worker_threads = j.value("worker_threads", 1);
  global_.default_backend = j.value("default_backend", "onnx");

  if (j.contains("backends") && j["backends"].is_array()) {
    for (const auto &b : j["backends"]) {
      BackendConfigEntry entry;
      entry.name = b.value("name", "");
      entry.device = b.value("device", "cpu");
      entry.threads = b.value("threads", 1);
      entry.cache_dir = b.value("cache_dir", "");
      entry.precision = b.value("precision", "fp32");
      global_.backends.push_back(std::move(entry));
    }
  }

  if (j.contains("models") && j["models"].is_object()) {
    for (auto it = j["models"].begin(); it != j["models"].end(); ++it) {
      ModelRegistryEntry entry;
      entry.name = it.key();
      entry.task = it.value().value("task", "unknown");
      entry.dir = it.value().value("dir", it.key());
      entry.backend = it.value().value("backend", global_.default_backend);
      global_.models.push_back(std::move(entry));
    }
  }
}

void ConfigManager::loadModelConfig(const std::string &model_dir,
                                    const std::string &model_name) {
  std::string config_path = joinPath(model_dir, "model.json");
  std::string text;
  try {
    text = readTextFile(config_path);
  } catch (const std::exception &e) {
    throw ConfigError(std::string("failed to load model config: ") + e.what());
  }

  json::json j;
  try {
    j = json::json::parse(text);
  } catch (const std::exception &e) {
    throw ConfigError(std::string("failed to parse model config: ") + e.what());
  }

  ModelConfig cfg;
  cfg.name = j.value("name", model_name);
  cfg.task = j.value("task", "unknown");
  cfg.model_file = j.value("model_file", "model.onnx");

  if (j.contains("inputs") && j["inputs"].is_array()) {
    for (const auto &input : j["inputs"]) {
      cfg.inputs.push_back(parseTensor(input, true));
    }
  }
  if (j.contains("outputs") && j["outputs"].is_array()) {
    for (const auto &output : j["outputs"]) {
      cfg.outputs.push_back(parseTensor(output, false));
    }
  }

  model_configs_[model_name] = std::move(cfg);
}

const ModelConfig &ConfigManager::modelConfig(const std::string &name) const {
  auto it = model_configs_.find(name);
  if (it == model_configs_.end()) {
    throw ConfigError("model config not found: " + name);
  }
  return it->second;
}

bool ConfigManager::hasModelConfig(const std::string &name) const {
  return model_configs_.find(name) != model_configs_.end();
}

} // namespace core
} // namespace oml
