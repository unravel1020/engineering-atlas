#include "inference.h"
#include "modelManager.h"
#include "pipeline.h"
#include "test_framework.h"
#include <opencv2/opencv.hpp>

// End-to-end integration test using the bundled squeezenet model.
// It exercises ModelManager, Inference, Pipeline and Result together.
TEST(integration_classification_pipeline_runs_on_squeezenet) {
  ModelManager manager("models/registry.json");
  manager.select("squeezenet1.1-7");
  auto model = manager.current();

  ASSERT_TRUE(model != nullptr);
  ASSERT_EQ(model->modelInfo().task(), "classification");

  cv::Mat img =
      cv::imread(model->modelInfo().modelDir() + "/test_data/dog.jpg");
  ASSERT_FALSE(img.empty());

  auto pipe = pipeline::PipelineFactory::create("classification", model);
  auto result = pipe->run(img);

  ASSERT_TRUE(result != nullptr);

  // Verify the output can be rendered without crashing.
  std::ostringstream oss;
  result->print(oss);
  ASSERT_TRUE(oss.str().find("Top-") != std::string::npos);

  cv::Mat vis = result->visualize(img);
  ASSERT_EQ(vis.rows, img.rows);
  ASSERT_EQ(vis.cols, img.cols);
}

// End-to-end integration test using the bundled yolov8n model.
TEST(integration_detection_pipeline_runs_on_yolov8n) {
  ModelManager manager("models/registry.json");
  manager.select("yolov8n");
  auto model = manager.current();

  ASSERT_TRUE(model != nullptr);
  ASSERT_EQ(model->modelInfo().task(), "detection");

  cv::Mat img =
      cv::imread(model->modelInfo().modelDir() + "/test_data/dog.jpg");
  ASSERT_FALSE(img.empty());

  auto pipe = pipeline::PipelineFactory::create("detection", model);
  auto result = pipe->run(img);

  ASSERT_TRUE(result != nullptr);

  std::ostringstream oss;
  result->print(oss);
  ASSERT_TRUE(oss.str().find("Detections:") != std::string::npos);

  cv::Mat vis = result->visualize(img);
  ASSERT_EQ(vis.rows, img.rows);
  ASSERT_EQ(vis.cols, img.cols);
}
