#include "modelLoader.h"
#include "json/json.hpp"
#include <fstream>

using json = nlohmann::json;

namespace {

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

  return cfg;
}

} // namespace

ModelInfo ModelLoader::load(const std::string &model_dir,
                            const std::string &model_name) {
  std::ifstream ifs(model_dir + "/model.json");

  json j;
  ifs >> j;

  std::string model_file = j.value("model_file", "model.onnx");
  std::string task = j.value("task", "unknown");

  std::vector<TensorInfo> inputs;
  for (const auto &input : j["inputs"]) {
    TensorInfo info;
    info.name = input["name"];
    info.type = parseType(input.value("type", "float32"));
    inputs.push_back(info);
  }

  std::vector<TensorInfo> outputs;
  for (const auto &output : j["outputs"]) {
    TensorInfo info;
    info.name = output["name"];
    info.type = parseType(output.value("type", "float32"));
    outputs.push_back(info);
  }

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
