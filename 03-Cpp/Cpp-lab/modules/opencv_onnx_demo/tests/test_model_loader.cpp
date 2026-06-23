#include "modelLoader.h"
#include "test_framework.h"
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <string>

namespace fs = std::filesystem;

namespace {

std::string createTempModelDir(const std::string &json_content) {
  fs::path tmp = fs::temp_directory_path() / "test_model";
  fs::create_directories(tmp);
  std::ofstream ofs(tmp / "model.json");
  ofs << json_content;
  std::ofstream labels(tmp / "labels.txt");
  labels << "a\nb\nc\n";
  return tmp.string();
}

} // namespace

TEST(model_loader_classification_defaults) {
  std::string dir = createTempModelDir(R"({
    "name": "cls",
    "task": "classification",
    "model_file": "cls.onnx",
    "inputs": [{ "name": "data", "type": "float32" }],
    "outputs": [{ "name": "out", "type": "float32" }]
  })");

  ModelInfo info = ModelLoader::load(dir, "cls");
  ASSERT_EQ(info.task(), "classification");
  ASSERT_EQ(info.modelPath(), dir + "/cls.onnx");
  ASSERT_EQ(info.inputs()[0].name, "data");
  ASSERT_EQ(info.postprocess().topk, 5);
  ASSERT_EQ(info.preprocess().layout, "NCHW");
  ASSERT_EQ(info.preprocess().target_h, 224);
}

TEST(model_loader_detection_metadata) {
  std::string dir = createTempModelDir(R"({
    "name": "yolo",
    "task": "detection",
    "model_file": "yolo.onnx",
    "inputs": [{
      "name": "images",
      "type": "float32",
      "preprocess": {
        "color_format": "RGB",
        "resize_mode": "letterbox",
        "target_h": 640,
        "target_w": 640,
        "pad_color": [114, 114, 114],
        "layout": "NCHW"
      }
    }],
    "outputs": [{
      "name": "output0",
      "type": "float32",
      "postprocess": {
        "conf_threshold": 0.25,
        "nms_threshold": 0.45,
        "labels_file": "labels.txt",
        "num_classes": 10,
        "num_candidates": 1000,
        "box_format": "xywh",
        "output_layout": "N_C_K"
      }
    }]
  })");

  ModelInfo info = ModelLoader::load(dir, "yolo");
  ASSERT_EQ(info.preprocess().color_format, "RGB");
  ASSERT_EQ(info.preprocess().resize_mode, "letterbox");
  ASSERT_EQ(info.preprocess().pad_color.size(), 3u);
  ASSERT_EQ(info.preprocess().pad_color[0], 114);
  ASSERT_EQ(info.postprocess().num_classes, 10);
  ASSERT_EQ(info.postprocess().num_candidates, 1000);
  ASSERT_EQ(info.postprocess().box_format, "xywh");
  ASSERT_EQ(info.postprocess().output_layout, "N_C_K");
}
