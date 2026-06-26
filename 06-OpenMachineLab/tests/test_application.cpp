#include "core/Application.h"
#include "pipeline/SequentialPipeline.h"
#include "test_framework.h"
#include <opencv2/opencv.hpp>

TEST(application_loads_config_and_initializes) {
  auto app = oml::core::Application::create(
      "../03-Cpp/Cpp-lab/modules/opencv_onnx_demo/models/registry.json");
  ASSERT_TRUE(app != nullptr);
  ASSERT_EQ(app->configManager().global().models.size(), 2u);

  ASSERT_TRUE(app->initialize());
  ASSERT_TRUE(app->initialized());

  auto session = app->getSession("squeezenet1.1-7");
  ASSERT_TRUE(session != nullptr);

  app->shutdown();
  ASSERT_FALSE(app->initialized());
}

TEST(application_creates_pipeline_for_model) {
  auto app = oml::core::Application::create(
      "../03-Cpp/Cpp-lab/modules/opencv_onnx_demo/models/registry.json");
  ASSERT_TRUE(app->initialize());

  auto pipeline = app->createPipeline("squeezenet1.1-7");
  ASSERT_TRUE(pipeline != nullptr);

  auto frame = std::make_shared<oml::pipeline::FrameData>();
  frame->frame_id = 1;
  frame->original_image = cv::imread(
      "../03-Cpp/Cpp-lab/modules/opencv_onnx_demo/models/squeezenet1.1-7/test_data/dog.jpg");
  ASSERT_FALSE(frame->original_image.empty());

  auto result = pipeline->run(frame);
  ASSERT_TRUE(result != nullptr);
  ASSERT_FALSE(result->result_json.empty());
  ASSERT_TRUE(result->result_json.find("topk") != std::string::npos);

  app->shutdown();
}

TEST(application_throws_for_unknown_pipeline) {
  auto app = oml::core::Application::create(
      "../03-Cpp/Cpp-lab/modules/opencv_onnx_demo/models/registry.json");
  ASSERT_TRUE(app->initialize());
  ASSERT_THROW(app->createPipeline("nonexistent"));
  app->shutdown();
}
