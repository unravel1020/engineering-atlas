#include "backend/BackendRegistry.h"
#include "core/ConfigManager.h"
#include "pipeline/InferenceStage.h"
#include "pipeline/PostprocessStage.h"
#include "pipeline/PreprocessStage.h"
#include "pipeline/SequentialPipeline.h"
#include "test_framework.h"
#include <opencv2/opencv.hpp>

TEST(sequential_pipeline_runs_classification_end_to_end) {
  // Load configuration.
  oml::core::ConfigManager cm;
  cm.loadGlobal(
      "../03-Cpp/Cpp-lab/modules/opencv_onnx_demo/models/registry.json");
  cm.loadModelConfig(
      "../03-Cpp/Cpp-lab/modules/opencv_onnx_demo/models/squeezenet1.1-7",
      "squeezenet1.1-7");

  const auto &model_cfg = cm.modelConfig("squeezenet1.1-7");
  const auto &global = cm.global();

  // Full model directory (registry entry.dir is relative to registry base).
  std::string model_dir =
      "../03-Cpp/Cpp-lab/modules/opencv_onnx_demo/models/squeezenet1.1-7";

  // Create backend and load model session.
  auto backend = oml::backend::BackendRegistry::instance().createBackend("onnx");
  ASSERT_TRUE(backend != nullptr);

  oml::backend::BackendConfig backend_cfg;
  backend_cfg.device = "cpu";
  ASSERT_TRUE(backend->initialize(backend_cfg));

  auto session = backend->loadModel(
      model_dir + "/" + model_cfg.model_file, "");
  ASSERT_TRUE(session != nullptr);

  // Build sequential pipeline.
  oml::pipeline::SequentialPipeline pipeline;
  pipeline.addStage(
      std::make_shared<oml::pipeline::PreprocessStage>(model_cfg));
  pipeline.addStage(std::make_shared<oml::pipeline::InferenceStage>(session));
  pipeline.addStage(
      std::make_shared<oml::pipeline::PostprocessStage>(model_cfg, model_dir));

  // Prepare input frame.
  auto frame = std::make_shared<oml::pipeline::FrameData>();
  frame->frame_id = 1;
  frame->original_image =
      cv::imread(model_dir + "/test_data/dog.jpg");
  ASSERT_FALSE(frame->original_image.empty());

  // Run pipeline.
  auto result = pipeline.run(frame);
  ASSERT_TRUE(result != nullptr);
  ASSERT_FALSE(result->result_json.empty());
  ASSERT_TRUE(result->result_json.find("topk") != std::string::npos);
  ASSERT_TRUE(result->result_json.find("Samoyed") != std::string::npos ||
              result->result_json.find("dog") != std::string::npos);
}
