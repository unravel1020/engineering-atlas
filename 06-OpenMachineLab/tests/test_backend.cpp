#include "backend/BackendRegistry.h"
#include "backend/ONNXBackend.h"
#include "backend/TensorRTBackend.h"
#include "test_framework.h"

TEST(backend_registry_contains_onnx) {
  auto &reg = oml::backend::BackendRegistry::instance();
  ASSERT_TRUE(reg.hasBackend("onnx"));
}

TEST(backend_registry_creates_onnx_backend) {
  auto &reg = oml::backend::BackendRegistry::instance();
  auto backend = reg.createBackend("onnx");
  ASSERT_TRUE(backend != nullptr);
  ASSERT_EQ(backend->getName(), "onnx");
}

TEST(onnx_backend_initializes_and_loads_squeezenet) {
  auto backend = std::make_shared<oml::backend::ONNXBackend>();
  oml::backend::BackendConfig cfg;
  cfg.device = "cpu";
  cfg.threads = 1;

  ASSERT_TRUE(backend->initialize(cfg));

  auto session = backend->loadModel(
      "../03-Cpp/Cpp-lab/modules/opencv_onnx_demo/models/squeezenet1.1-7/model.onnx",
      "");
  ASSERT_TRUE(session != nullptr);

  auto inputs = session->getInputInfos();
  ASSERT_EQ(inputs.size(), 1u);
  ASSERT_EQ(inputs[0].name, "data");

  auto outputs = session->getOutputInfos();
  ASSERT_EQ(outputs.size(), 1u);
}

TEST(tensorrt_backend_is_registered) {
  auto &reg = oml::backend::BackendRegistry::instance();
  ASSERT_TRUE(reg.hasBackend("tensorrt"));

  auto backend = reg.createBackend("tensorrt");
  ASSERT_TRUE(backend != nullptr);
  ASSERT_EQ(backend->getName(), "tensorrt");
}

TEST(tensorrt_backend_initializes_with_gpu_device) {
  auto backend = std::make_shared<oml::backend::TensorRTBackend>();
  oml::backend::BackendConfig cfg;
  cfg.device = "cuda:0";
  cfg.threads = 1;
  cfg.precision = "fp16";
  cfg.cache_dir = "trt_cache";

  ASSERT_TRUE(backend->initialize(cfg));

  auto devices = backend->getSupportedDevices();
  ASSERT_TRUE(devices.size() >= 1u);
  ASSERT_EQ(devices[0], "cuda:0");
}

TEST(tensorrt_loadModel_throws_not_implemented) {
  auto backend = std::make_shared<oml::backend::TensorRTBackend>();
  oml::backend::BackendConfig cfg;
  cfg.device = "cuda:0";
  ASSERT_TRUE(backend->initialize(cfg));

  ASSERT_THROW(backend->loadModel("dummy.onnx", ""));
}
