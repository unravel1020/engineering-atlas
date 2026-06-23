#pragma once

#include <onnxruntime_cxx_api.h>
#include <cstdint>
#include <string>
#include <vector>

// Tensor metadata is parsed from model.json and then synchronized against the
// actual ONNX session to support dynamic dimensions (e.g. batch size -1).
struct TensorInfo {
  std::string name;

  std::vector<int64_t> shape;

  ONNXTensorElementDataType type;
};

// PreprocessConfig drives image preprocessing entirely from model metadata.
// Keeping all knobs in JSON avoids hard-coding task-specific defaults in code.
struct PreprocessConfig {
  std::string color_format = "BGR";

  std::string resize_mode = "stretch";

  int target_h = 224;

  int target_w = 224;

  float scale = 1.0f / 255.0f;

  std::vector<float> mean = {0.0f, 0.0f, 0.0f};

  std::vector<float> std = {1.0f, 1.0f, 1.0f};

  std::string layout = "NCHW";

  std::vector<uint8_t> pad_color = {114, 114, 114};
};

// PreprocessInfo records the geometric transform applied to the original image
// so that detection postprocess can map model-output boxes back to original
// image coordinates.
struct PreprocessInfo {
  int orig_h = 0;

  int orig_w = 0;

  int model_h = 0;

  int model_w = 0;

  float scale = 1.0f;

  int pad_top = 0;

  int pad_bottom = 0;

  int pad_left = 0;

  int pad_right = 0;

  std::string resize_mode;
};

// PostprocessConfig is intentionally metadata-driven: the same detect() code
// can serve different detection models by varying num_classes, num_candidates,
// box_format and output_layout instead of hard-coding YOLOv8 constants.
struct PostprocessConfig {
  std::string activation = "softmax";

  int topk = 5;

  std::string labels_file;

  float conf_threshold = 0.25f;

  float nms_threshold = 0.45f;

  // Detection-specific metadata: these knobs are grouped here so the same
  // PostprocessConfig struct can serve both classification and detection.
  int num_classes = 80;

  int num_candidates = 8400;

  std::string box_format = "xywh"; // xywh | xyxy

  std::string output_layout = "N_C_K"; // N_C_K: e.g. 1 x 84 x 8400
};

// ModelInfo is the central value object produced by ModelLoader and consumed
// by Inference, Pipeline and post-processing. It deliberately separates JSON
// configuration from runtime state such as the ONNX session.
class ModelInfo {
public:
  ModelInfo() = default;

  ModelInfo(const std::string &model_dir, const std::string &model_file,
            const std::string &task,
            const std::vector<TensorInfo> &inputs,
            const std::vector<TensorInfo> &outputs,
            const PreprocessConfig &preprocess,
            const PostprocessConfig &postprocess);

  const std::string &modelDir() const;

  std::string modelPath() const;

  const std::string &task() const;

  const std::vector<TensorInfo> &inputs() const;

  const std::vector<TensorInfo> &outputs() const;

  const PreprocessConfig &preprocess() const;

  const PostprocessConfig &postprocess() const;

  void setInputs(const std::vector<TensorInfo> &inputs);

  void setOutputs(const std::vector<TensorInfo> &outputs);

  void print() const;

private:
  std::string model_dir_;

  std::string model_file_;

  std::string task_;

  std::vector<TensorInfo> inputs_;

  std::vector<TensorInfo> outputs_;

  PreprocessConfig preprocess_;

  PostprocessConfig postprocess_;
};
