#include "core/Application.h"
#include "pipeline/ThreadPoolPipeline.h"
#include "test_framework.h"
#include <opencv2/opencv.hpp>

TEST(thread_pool_pipeline_runs_classification_in_parallel) {
  auto app = oml::core::Application::create(
      "../03-Cpp/Cpp-lab/modules/opencv_onnx_demo/models/registry.json");
  ASSERT_TRUE(app->initialize());

  auto base_pipeline = app->createPipeline("squeezenet1.1-7");
  ASSERT_TRUE(base_pipeline != nullptr);

  // ThreadPoolPipeline owns its own stages; copy them from the base pipeline.
  oml::pipeline::ThreadPoolPipeline parallel_pipeline(2);
  for (const auto &stage : base_pipeline->stages()) {
    parallel_pipeline.addStage(stage);
  }

  std::string image_path =
      "../03-Cpp/Cpp-lab/modules/opencv_onnx_demo/models/squeezenet1.1-7/test_data/dog.jpg";

  std::vector<oml::pipeline::FrameDataPtr> frames;
  for (int i = 0; i < 4; ++i) {
    auto frame = std::make_shared<oml::pipeline::FrameData>();
    frame->frame_id = i;
    frame->original_image = cv::imread(image_path);
    ASSERT_FALSE(frame->original_image.empty());
    frames.push_back(frame);
  }

  auto results = parallel_pipeline.runBatch(frames);
  ASSERT_EQ(results.size(), 4u);

  for (const auto &result : results) {
    ASSERT_TRUE(result != nullptr);
    ASSERT_FALSE(result->result_json.empty());
    ASSERT_TRUE(result->result_json.find("topk") != std::string::npos);
  }

  app->shutdown();
}

TEST(thread_pool_pipeline_submits_individual_frames) {
  auto app = oml::core::Application::create(
      "../03-Cpp/Cpp-lab/modules/opencv_onnx_demo/models/registry.json");
  ASSERT_TRUE(app->initialize());

  auto base_pipeline = app->createPipeline("squeezenet1.1-7");
  oml::pipeline::ThreadPoolPipeline parallel_pipeline(2);
  for (const auto &stage : base_pipeline->stages()) {
    parallel_pipeline.addStage(stage);
  }

  std::string image_path =
      "../03-Cpp/Cpp-lab/modules/opencv_onnx_demo/models/squeezenet1.1-7/test_data/dog.jpg";

  auto frame = std::make_shared<oml::pipeline::FrameData>();
  frame->frame_id = 42;
  frame->original_image = cv::imread(image_path);
  ASSERT_FALSE(frame->original_image.empty());

  auto future = parallel_pipeline.submit(frame);
  auto result = future.get();
  ASSERT_TRUE(result != nullptr);
  ASSERT_TRUE(result->result_json.find("topk") != std::string::npos);

  app->shutdown();
}
