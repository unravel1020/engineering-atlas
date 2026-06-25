#include "modelLoader.h"
#include "json/json.hpp"
#include <fstream>

using json = nlohmann::json;

namespace {

// ONNX Runtime element types are represented as enum values. We only expose a
// small subset in JSON because most vision models use FP32; unknown strings
// safely fall back to FP32.
ONNXTensorElementDataType parseType(const std::string &type_str) {
  if (type_str == "float32") {
    return ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT;
  }
  if (type_str == "int64") {
    return ONNX_TENSOR_ELEMENT_DATA_TYPE_INT64;
  }
  if (type_str == "int32") {
    return ONNX_TENSOR_ELEMENT_DATA_TYPE_INT32;
  }
  return ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT;
}

PreprocessConfig parsePreprocess(const json &j) {
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

PostprocessConfig parsePostprocess(const json &j) {
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

} // namespace

ModelInfo ModelLoader::load(const std::string &model_dir,
                            const std::string &model_name) {
  std::string config_path = model_dir + "/model.json";
  std::ifstream ifs(config_path);
  if (!ifs.is_open()) {
    throw std::runtime_error("model.json not found: " + config_path);
  }

  json j;
  try {
    ifs >> j;
  } catch (const std::exception &e) {
    throw std::runtime_error(std::string("failed to parse model.json: ") +
                             e.what());
  }

  if (!j.contains("inputs") || !j["inputs"].is_array()) {
    throw std::runtime_error("model.json must contain an 'inputs' array");
  }
  if (!j.contains("outputs") || !j["outputs"].is_array()) {
    throw std::runtime_error("model.json must contain an 'outputs' array");
  }

  std::string model_file = j.value("model_file", "model.onnx");
  std::string task = j.value("task", "unknown");

  std::vector<TensorInfo> inputs;
  for (const auto &input : j["inputs"]) {
    TensorInfo info;
    info.name = input.value("name", "");
    info.type = parseType(input.value("type", "float32"));
    inputs.push_back(info);
  }

  std::vector<TensorInfo> outputs;
  for (const auto &output : j["outputs"]) {
    TensorInfo info;
    info.name = output.value("name", "");
    info.type = parseType(output.value("type", "float32"));
    outputs.push_back(info);
  }

  // Currently only the first input/output preprocess/postprocess block is used.
  // Multi-input models can still declare all tensors; their preprocessing would
  // be selected by name in a future extension.
  PreprocessConfig preprocess;
  if (!j["inputs"].empty() && j["inputs"][0].contains("preprocess")) {
    preprocess = parsePreprocess(j["inputs"][0]["preprocess"]);
  }

  PostprocessConfig postprocess;
  if (!j["outputs"].empty() && j["outputs"][0].contains("postprocess")) {
    postprocess = parsePostprocess(j["outputs"][0]["postprocess"]);
  }

  return ModelInfo(model_dir, model_file, task, inputs, outputs, preprocess,
                   postprocess);
}
