#include "modelRegistry.h"
#include "test_framework.h"
#include <fstream>
#include <filesystem>
#include <string>

namespace fs = std::filesystem;

namespace {

std::string createTempRegistry() {
  fs::path tmp = fs::temp_directory_path() / "test_registry.json";
  std::ofstream ofs(tmp);
  ofs << R"({
    "version": "1.0.0",
    "models": {
      "model_a": { "task": "classification", "dir": "a" },
      "model_b": { "task": "detection", "dir": "b" }
    }
  })";
  return tmp.string();
}

} // namespace

TEST(model_registry_loads_entries) {
  auto path = createTempRegistry();
  ModelRegistry reg = ModelRegistry::load(path);

  auto names = reg.modelNames();
  ASSERT_EQ(names.size(), 2u);
  ASSERT_TRUE(reg.contains("model_a"));
  ASSERT_TRUE(reg.contains("model_b"));
  ASSERT_FALSE(reg.contains("model_c"));

  ASSERT_EQ(reg.task("model_a"), "classification");
  ASSERT_EQ(reg.task("model_b"), "detection");

  fs::path base = fs::path(path).parent_path();
  ASSERT_EQ(reg.modelDir("model_a"), (base / "a").string());
  ASSERT_EQ(reg.modelDir("model_b"), (base / "b").string());
}

TEST(model_registry_missing_model_returns_empty) {
  auto path = createTempRegistry();
  ModelRegistry reg = ModelRegistry::load(path);

  ASSERT_EQ(reg.task("missing"), "");
  ASSERT_EQ(reg.modelDir("missing"), "");
}
