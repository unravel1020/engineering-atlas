#include "pipeline/PostprocessStage.h"
#include "utils.h"
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <numeric>
#include <sstream>

namespace oml {
namespace pipeline {

namespace {

std::vector<std::string> loadLabels(const std::string &path) {
  std::ifstream ifs(path);
  if (!ifs.is_open()) {
    throw std::runtime_error("failed to open labels file: " + path);
  }
  std::vector<std::string> labels;
  std::string line;
  while (std::getline(ifs, line)) {
    labels.push_back(line);
  }
  return labels;
}

std::vector<float> softmax(const std::vector<float> &logits) {
  if (logits.empty()) {
    return {};
  }
  std::vector<float> probs(logits.size());
  float max_value = *std::max_element(logits.begin(), logits.end());
  float sum = 0.0f;
  for (size_t i = 0; i < logits.size(); ++i) {
    probs[i] = std::exp(logits[i] - max_value);
    sum += probs[i];
  }
  for (auto &x : probs) {
    x /= sum;
  }
  return probs;
}

std::vector<int> topk(const std::vector<float> &data, int k) {
  if (k > static_cast<int>(data.size())) {
    k = static_cast<int>(data.size());
  }
  std::vector<int> idx(data.size());
  std::iota(idx.begin(), idx.end(), 0);
  std::partial_sort(idx.begin(), idx.begin() + k, idx.end(),
                    [&](int a, int b) { return data[a] > data[b]; });
  idx.resize(k);
  return idx;
}

struct Detection {
  int class_id = 0;
  float confidence = 0.0f;
  float x1 = 0.0f;
  float y1 = 0.0f;
  float x2 = 0.0f;
  float y2 = 0.0f;
};

float computeIoU(const Detection &a, const Detection &b) {
  float x1 = std::max(a.x1, b.x1);
  float y1 = std::max(a.y1, b.y1);
  float x2 = std::min(a.x2, b.x2);
  float y2 = std::min(a.y2, b.y2);
  float inter = std::max(0.0f, x2 - x1) * std::max(0.0f, y2 - y1);
  float area_a = (a.x2 - a.x1) * (a.y2 - a.y1);
  float area_b = (b.x2 - b.x1) * (b.y2 - b.y1);
  return inter / (area_a + area_b - inter + 1e-6f);
}

} // namespace

PostprocessStage::PostprocessStage(const core::ModelConfig &model_config,
                                   const std::string &model_dir)
    : model_config_(model_config), model_dir_(model_dir) {
  if (!model_config_.outputs.empty() &&
      !model_config_.outputs[0].postprocess.labels_file.empty()) {
    labels_ = loadLabels(oml::utils::joinPath(
        model_dir_, model_config_.outputs[0].postprocess.labels_file));
  }
}

FrameDataPtr PostprocessStage::process(FrameDataPtr input) {
  if (!input) {
    return nullptr;
  }

  if (model_config_.task == "classification") {
    return processClassification(input);
  }
  if (model_config_.task == "detection") {
    return processDetection(input);
  }

  input->result_json = "{\"error\":\"unsupported task\"}";
  return input;
}

FrameDataPtr PostprocessStage::processClassification(FrameDataPtr input) {
  if (model_config_.outputs.empty()) {
    return nullptr;
  }
  const std::string &name = model_config_.outputs[0].name;
  auto it = input->raw_outputs.find(name);
  if (it == input->raw_outputs.end()) {
    return nullptr;
  }

  const auto &cfg = model_config_.outputs[0].postprocess;
  auto probs = softmax(it->second);
  auto indices = topk(probs, cfg.topk);

  std::ostringstream oss;
  oss << "{\"topk\":[";
  for (size_t i = 0; i < indices.size(); ++i) {
    int idx = indices[i];
    std::string label = idx < static_cast<int>(labels_.size()) ? labels_[idx]
                                                               : "";
    if (i > 0) {
      oss << ",";
    }
    oss << "{\"index\":" << idx << ",\"label\":\"" << label
        << "\",\"confidence\":" << std::fixed << std::setprecision(6)
        << probs[idx] << "}";
  }
  oss << "]}";
  input->result_json = oss.str();
  return input;
}

FrameDataPtr PostprocessStage::processDetection(FrameDataPtr input) {
  if (model_config_.outputs.empty()) {
    return nullptr;
  }
  const std::string &name = model_config_.outputs[0].name;
  auto it = input->raw_outputs.find(name);
  if (it == input->raw_outputs.end()) {
    return nullptr;
  }

  const auto &cfg = model_config_.outputs[0].postprocess;
  const auto &output = it->second;

  if (cfg.output_layout != "N_C_K") {
    input->result_json = "{\"error\":\"unsupported detection output layout\"}";
    return input;
  }

  const int num_classes = cfg.num_classes;
  const int num_candidates = cfg.num_candidates;
  const int total_channels = 4 + num_classes;

  std::vector<Detection> detections;

  if (output.size() == static_cast<size_t>(total_channels * num_candidates)) {
    for (int i = 0; i < num_candidates; ++i) {
      float x = output[i];
      float y = output[num_candidates + i];
      float w = output[2 * num_candidates + i];
      float h = output[3 * num_candidates + i];

      float max_score = 0.0f;
      int class_id = 0;
      for (int c = 0; c < num_classes; ++c) {
        float score = output[(4 + c) * num_candidates + i];
        if (score > max_score) {
          max_score = score;
          class_id = c;
        }
      }

      if (max_score < cfg.conf_threshold) {
        continue;
      }

      float x1_model = x;
      float y1_model = y;
      float x2_model = w;
      float y2_model = h;
      if (cfg.box_format == "xywh") {
        x1_model = x - w / 2.0f;
        y1_model = y - h / 2.0f;
        x2_model = x + w / 2.0f;
        y2_model = y + h / 2.0f;
      }

      Detection det;
      det.class_id = class_id;
      det.confidence = max_score;
      det.x1 = x1_model;
      det.y1 = y1_model;
      det.x2 = x2_model;
      det.y2 = y2_model;
      detections.push_back(det);
    }

    std::sort(detections.begin(), detections.end(),
              [](const Detection &a, const Detection &b) {
                return a.confidence > b.confidence;
              });

    std::vector<Detection> nms_result;
    std::vector<bool> suppressed(detections.size(), false);
    for (size_t i = 0; i < detections.size(); ++i) {
      if (suppressed[i]) {
        continue;
      }
      nms_result.push_back(detections[i]);
      for (size_t j = i + 1; j < detections.size(); ++j) {
        if (!suppressed[j] &&
            computeIoU(detections[i], detections[j]) > cfg.nms_threshold) {
          suppressed[j] = true;
        }
      }
    }
    detections = std::move(nms_result);
  }

  std::ostringstream oss;
  oss << "{\"detections\":[";
  for (size_t i = 0; i < detections.size(); ++i) {
    const auto &det = detections[i];
    std::string label = det.class_id < static_cast<int>(labels_.size())
                            ? labels_[det.class_id]
                            : "";
    if (i > 0) {
      oss << ",";
    }
    oss << "{\"class_id\":" << det.class_id << ",\"label\":\"" << label
        << "\",\"confidence\":" << std::fixed << std::setprecision(6)
        << det.confidence << ",\"box\":[" << det.x1 << "," << det.y1 << ","
        << det.x2 << "," << det.y2 << "]}";
  }
  oss << "]}";
  input->result_json = oss.str();
  return input;
}

} // namespace pipeline
} // namespace oml
