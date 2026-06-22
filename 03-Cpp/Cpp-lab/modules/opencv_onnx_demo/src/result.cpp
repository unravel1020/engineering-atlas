#include "result.h"
#include <fstream>
#include <iomanip>
#include <numeric>

namespace result {

namespace {

std::vector<std::string> loadLabels(const std::string &path) {
  std::ifstream ifs(path);
  std::vector<std::string> labels;
  std::string line;
  while (std::getline(ifs, line)) {
    labels.push_back(line);
  }
  return labels;
}

} // namespace

void ClassificationResult::print(std::ostream &os) const {
  os << "Top-" << postprocess_.topk << " indices:\n";
  for (auto i : indices_) {
    os << labels_[i] << " probability=" << probs_[i] * 100.0f << "%\n";
  }

  float total = 0.0f;
  for (auto p : probs_) {
    total += p;
  }
  os << "sum = " << total << std::endl;
}

cv::Mat ClassificationResult::visualize(const cv::Mat &img) const {
  cv::Mat out = img.clone();
  const int margin = 10;
  const int line_height = 20;
  int y = margin + line_height;

  for (size_t rank = 0; rank < indices_.size() && rank < postprocess_.topk;
       ++rank) {
    int idx = indices_[rank];
    std::string text = labels_[idx] + ": " +
                       std::to_string(static_cast<int>(probs_[idx] * 100.0f)) +
                       "%";
    cv::putText(out, text, cv::Point(margin, y), cv::FONT_HERSHEY_SIMPLEX, 0.5,
                cv::Scalar(0, 255, 0), 1);
    y += line_height;
  }
  return out;
}

void ClassificationResult::save(const std::string &base_path,
                                const cv::Mat &img) const {
  cv::Mat out = visualize(img);
  cv::imwrite(base_path, out);
}

void DetectionResult::print(std::ostream &os) const {
  os << "Detections: " << detections_.size() << "\n";
  for (const auto &det : detections_) {
    os << "class=" << det.class_id << " conf=" << det.confidence
       << " box=[" << det.x1 << "," << det.y1 << "," << det.x2 << ","
       << det.y2 << "]\n";
  }
}

cv::Mat DetectionResult::visualize(const cv::Mat &img) const {
  cv::Mat out = img.clone();
  for (const auto &det : detections_) {
    cv::Point pt1(static_cast<int>(det.x1), static_cast<int>(det.y1));
    cv::Point pt2(static_cast<int>(det.x2), static_cast<int>(det.y2));
    cv::rectangle(out, pt1, pt2, cv::Scalar(0, 255, 0), 2);

    std::string label =
        (det.class_id >= 0 && det.class_id < static_cast<int>(labels_.size()))
            ? labels_[det.class_id]
            : "class_" + std::to_string(det.class_id);
    std::string text = label + " " +
                       std::to_string(static_cast<int>(det.confidence * 100)) +
                       "%";

    int baseline = 0;
    cv::Size text_size =
        cv::getTextSize(text, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseline);
    cv::rectangle(out, pt1,
                  cv::Point(pt1.x + text_size.width,
                            pt1.y - text_size.height - 4),
                  cv::Scalar(0, 255, 0), -1);
    cv::putText(out, text, cv::Point(pt1.x, pt1.y - 2),
                cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0), 1);
  }
  return out;
}

void DetectionResult::save(const std::string &base_path,
                           const cv::Mat &img) const {
  cv::Mat out = visualize(img);
  cv::imwrite(base_path, out);
}

} // namespace result
