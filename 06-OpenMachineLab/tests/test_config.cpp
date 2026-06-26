#include "core/ConfigManager.h"
#include "test_framework.h"

TEST(config_manager_loads_global_and_model_configs) {
  oml::core::ConfigManager cm;
  cm.loadGlobal(
      "../03-Cpp/Cpp-lab/modules/opencv_onnx_demo/models/registry.json");

  ASSERT_EQ(cm.global().models.size(), 2u);
  ASSERT_TRUE(cm.global().models[0].name == "yolov8n" ||
              cm.global().models[0].name == "squeezenet1.1-7");

  cm.loadModelConfig(
      "../03-Cpp/Cpp-lab/modules/opencv_onnx_demo/models/squeezenet1.1-7",
      "squeezenet1.1-7");

  ASSERT_TRUE(cm.hasModelConfig("squeezenet1.1-7"));
  const auto &model = cm.modelConfig("squeezenet1.1-7");
  ASSERT_EQ(model.task, "classification");
  ASSERT_EQ(model.inputs.size(), 1u);
  ASSERT_EQ(model.outputs.size(), 1u);
  ASSERT_EQ(model.inputs[0].preprocess.target_h, 224);
}

TEST(config_manager_throws_on_missing_global_config) {
  oml::core::ConfigManager cm;
  ASSERT_THROW(cm.loadGlobal("/nonexistent/config.json"));
}
