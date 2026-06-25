#include "result.h"
#include "test_framework.h"
#include <opencv2/opencv.hpp>
#include <sstream>

using namespace result;

TEST(classification_result_prints_topk) {
  PostprocessConfig cfg;
  cfg.topk = 2;

  std::vector<std::string> labels = {"cat", "dog", "bird"};
  std::vector<float> probs = {0.1f, 0.8f, 0.1f};
  std::vector<int> indices = {1, 0};

  ClassificationResult r(".", cfg, std::move(labels), std::move(probs),
                         std::move(indices));

  std::ostringstream oss;
  r.print(oss);
  std::string out = oss.str();

  ASSERT_TRUE(out.find("dog probability=80") != std::string::npos);
  ASSERT_TRUE(out.find("cat probability=10") != std::string::npos);
  ASSERT_TRUE(out.find("sum = 1") != std::string::npos);
}

TEST(classification_result_visualizes_without_crash) {
  PostprocessConfig cfg;
  cfg.topk = 1;

  std::vector<std::string> labels = {"dog"};
  std::vector<float> probs = {1.0f};
  std::vector<int> indices = {0};

  ClassificationResult r(".", cfg, std::move(labels), std::move(probs),
                         std::move(indices));

  cv::Mat img(100, 100, CV_8UC3, cv::Scalar(0, 0, 0));
  cv::Mat out = r.visualize(img);

  ASSERT_EQ(out.rows, 100);
  ASSERT_EQ(out.cols, 100);
}

TEST(detection_result_prints_detections) {
  PostprocessConfig cfg;
  std::vector<std::string> labels = {"person"};
  std::vector<Detection> dets;
  dets.push_back({0, 0.9f, 10.0f, 20.0f, 30.0f, 40.0f});

  DetectionResult r(".", cfg, std::move(labels), std::move(dets));

  std::ostringstream oss;
  r.print(oss);
  std::string out = oss.str();

  ASSERT_TRUE(out.find("Detections: 1") != std::string::npos);
  ASSERT_TRUE(out.find("class=0 conf=0.9 box=[10,20,30,40]") != std::string::npos);
}

TEST(detection_result_visualizes_without_crash) {
  PostprocessConfig cfg;
  std::vector<std::string> labels = {"person"};
  std::vector<Detection> dets;
  dets.push_back({0, 0.9f, 10.0f, 20.0f, 30.0f, 40.0f});

  DetectionResult r(".", cfg, std::move(labels), std::move(dets));

  cv::Mat img(100, 100, CV_8UC3, cv::Scalar(0, 0, 0));
  cv::Mat out = r.visualize(img);

  ASSERT_EQ(out.rows, 100);
  ASSERT_EQ(out.cols, 100);
}

TEST(detection_result_uses_fallback_label_for_unknown_class) {
  PostprocessConfig cfg;
  std::vector<std::string> labels = {"person"};
  std::vector<Detection> dets;
  dets.push_back({5, 0.9f, 0.0f, 0.0f, 10.0f, 10.0f});

  DetectionResult r(".", cfg, std::move(labels), std::move(dets));

  cv::Mat img(100, 100, CV_8UC3, cv::Scalar(0, 0, 0));
  cv::Mat out = r.visualize(img);

  ASSERT_EQ(out.rows, 100);
  ASSERT_EQ(out.cols, 100);
}
