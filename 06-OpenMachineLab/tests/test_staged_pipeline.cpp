#include "core/Application.h"
#include "pipeline/StagedPipeline.h"
#include "test_framework.h"
#include <opencv2/opencv.hpp>

TEST(staged_pipeline_runs_classification_with_stage_overlap) {
  auto app = oml::core::Application::create(
      "../03-Cpp/Cpp-lab/modules/opencv_onnx_demo/models/registry.json");
  ASSERT_TRUE(app->initialize());

  auto base_pipeline = app->createPipeline("squeezenet1.1-7");
  ASSERT_TRUE(base_pipeline != nullptr);

  oml::pipeline::StagedPipeline staged_pipeline;
  for (const auto &stage : base_pipeline->stages()) {
    staged_pipeline.addStage(stage);
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

  auto results = staged_pipeline.runBatch(frames);
  ASSERT_EQ(results.size(), 4u);

  for (const auto &result : results) {
    ASSERT_TRUE(result != nullptr);
    ASSERT_FALSE(result->result_json.empty());
    ASSERT_TRUE(result->result_json.find("topk") != std::string::npos);
  }

  app->shutdown();
}
