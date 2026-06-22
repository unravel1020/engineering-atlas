#include "postProcessor.h"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <fstream>
#include <numeric>
#include <vector>

namespace postProcessor {

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

std::vector<std::string> loadLabels(const std::string &path) {
  std::ifstream ifs(path);

  std::vector<std::string> labels;

  std::string line;

  while (std::getline(ifs, line)) {
    labels.push_back(line);
  }

  return labels;
}

std::vector<float> softmax(const std::vector<float> &logits) {
  std::vector<float> probs(logits.size());

  float maxValue = *std::max_element(logits.begin(), logits.end());

  float sum = 0.0f;

  for (size_t i = 0; i < logits.size(); i++) {
    probs[i] = std::exp(logits[i] - maxValue);
    sum += probs[i];
  }

  for (auto &x : probs) {
    x /= sum;
  }
  return probs;
}

ClassificationResult classify(const std::vector<float> &output,
                              const PostprocessConfig &cfg,
                              const std::string &model_dir) {
  std::vector<float> probs = softmax(output);
  std::vector<int> indices = topk(probs, cfg.topk);
  std::vector<std::string> labels = loadLabels(model_dir + "/" + cfg.labels_file);

  // 防御 labels 文件行数不足导致的越界访问
  while (labels.size() < output.size()) {
    labels.emplace_back();
  }

  return ClassificationResult(model_dir, cfg, std::move(labels),
                              std::move(probs), std::move(indices));
}

namespace {

struct Candidate {
  int index;
  float confidence;
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

DetectionResult detect(const std::vector<float> &output,
                       const PostprocessConfig &cfg,
                       const std::string &model_dir,
                       const PreprocessInfo &preprocess_info) {
  std::vector<Detection> detections;

  // YOLOv8 default ONNX output shape: 1 x 84 x 8400
  const int num_classes = 80;
  const int num_candidates = 8400;
  const int total_channels = 84;

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

      float confidence = max_score;
      if (confidence < cfg.conf_threshold) {
        continue;
      }

      // xywh -> xyxy in model input coordinate system
      float x1_model = x - w / 2.0f;
      float y1_model = y - h / 2.0f;
      float x2_model = x + w / 2.0f;
      float y2_model = y + h / 2.0f;

      // map back to original image
      float scale = preprocess_info.scale;
      float x1 = (x1_model - preprocess_info.pad_left) / scale;
      float y1 = (y1_model - preprocess_info.pad_top) / scale;
      float x2 = (x2_model - preprocess_info.pad_left) / scale;
      float y2 = (y2_model - preprocess_info.pad_top) / scale;

      // clip to original image bounds
      x1 = std::max(0.0f, std::min(x1, static_cast<float>(preprocess_info.orig_w)));
      y1 = std::max(0.0f, std::min(y1, static_cast<float>(preprocess_info.orig_h)));
      x2 = std::max(0.0f, std::min(x2, static_cast<float>(preprocess_info.orig_w)));
      y2 = std::max(0.0f, std::min(y2, static_cast<float>(preprocess_info.orig_h)));

      Detection det;
      det.class_id = class_id;
      det.confidence = confidence;
      det.x1 = x1;
      det.y1 = y1;
      det.x2 = x2;
      det.y2 = y2;
      detections.push_back(det);
    }

    // NMS
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
        if (suppressed[j]) {
          continue;
        }
        if (computeIoU(detections[i], detections[j]) > cfg.nms_threshold) {
          suppressed[j] = true;
        }
      }
    }

    detections = std::move(nms_result);
  }

  std::vector<std::string> labels = loadLabels(model_dir + "/" + cfg.labels_file);
  return DetectionResult(model_dir, cfg, std::move(labels),
                         std::move(detections));
}

} // namespace postProcessor
