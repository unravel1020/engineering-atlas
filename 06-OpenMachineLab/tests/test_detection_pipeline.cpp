#include "core/Application.h"
#include "pipeline/SequentialPipeline.h"
#include "pipeline/StagedPipeline.h"
#include "pipeline/ThreadPoolPipeline.h"
#include "test_framework.h"
#include <opencv2/opencv.hpp>

namespace {

std::shared_ptr<oml::pipeline::FrameData> makeFrame(const std::string &path,
                                                    uint64_t id) {
  auto frame = std::make_shared<oml::pipeline::FrameData>();
  frame->frame_id = id;
  frame->original_image = cv::imread(path);
  return frame;
}

} // namespace

TEST(sequential_pipeline_runs_detection_end_to_end) {
  auto app = oml::core::Application::create(
      "../03-Cpp/Cpp-lab/modules/opencv_onnx_demo/models/registry.json");
  ASSERT_TRUE(app->initialize());

  auto pipeline = app->createPipeline("yolov8n");
  ASSERT_TRUE(pipeline != nullptr);

  std::string image_path =
      "../03-Cpp/Cpp-lab/modules/opencv_onnx_demo/models/yolov8n/test_data/dog.jpg";
  auto frame = makeFrame(image_path, 1);
  ASSERT_FALSE(frame->original_image.empty());

  auto result = pipeline->run(frame);
  ASSERT_TRUE(result != nullptr);
  ASSERT_FALSE(result->result_json.empty());
  ASSERT_TRUE(result->result_json.find("detections") != std::string::npos);

  app->shutdown();
}

TEST(staged_pipeline_runs_detection_with_stage_overlap) {
  auto app = oml::core::Application::create(
      "../03-Cpp/Cpp-lab/modules/opencv_onnx_demo/models/registry.json");
  ASSERT_TRUE(app->initialize());

  auto base_pipeline = app->createPipeline("yolov8n");
  oml::pipeline::StagedPipeline staged;
  for (const auto &stage : base_pipeline->stages()) {
    staged.addStage(stage);
  }

  std::string image_path =
      "../03-Cpp/Cpp-lab/modules/opencv_onnx_demo/models/yolov8n/test_data/dog.jpg";
  std::vector<oml::pipeline::FrameDataPtr> frames;
  for (int i = 0; i < 3; ++i) {
    frames.push_back(makeFrame(image_path, i));
    ASSERT_FALSE(frames.back()->original_image.empty());
  }

  auto results = staged.runBatch(frames);
  ASSERT_EQ(results.size(), 3u);
  for (const auto &result : results) {
    ASSERT_TRUE(result != nullptr);
    ASSERT_TRUE(result->result_json.find("detections") != std::string::npos);
  }

  app->shutdown();
}

TEST(thread_pool_pipeline_runs_detection_in_parallel) {
  auto app = oml::core::Application::create(
      "../03-Cpp/Cpp-lab/modules/opencv_onnx_demo/models/registry.json");
  ASSERT_TRUE(app->initialize());

  auto base_pipeline = app->createPipeline("yolov8n");
  oml::pipeline::ThreadPoolPipeline parallel(2);
  for (const auto &stage : base_pipeline->stages()) {
    parallel.addStage(stage);
  }

  std::string image_path =
      "../03-Cpp/Cpp-lab/modules/opencv_onnx_demo/models/yolov8n/test_data/dog.jpg";
  std::vector<oml::pipeline::FrameDataPtr> frames;
  for (int i = 0; i < 3; ++i) {
    frames.push_back(makeFrame(image_path, i));
    ASSERT_FALSE(frames.back()->original_image.empty());
  }

  auto results = parallel.runBatch(frames);
  ASSERT_EQ(results.size(), 3u);
  for (const auto &result : results) {
    ASSERT_TRUE(result != nullptr);
    ASSERT_TRUE(result->result_json.find("detections") != std::string::npos);
  }

  app->shutdown();
}
