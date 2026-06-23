#include "modelManager.h"
#include "test_framework.h"
#include <filesystem>
#include <fstream>
#include <string>

namespace fs = std::filesystem;

namespace {

std::string createTempRegistry() {
  fs::path tmp = fs::temp_directory_path() / "test_manager_registry.json";
  std::ofstream ofs(tmp);
  ofs << R"({
    "models": {
      "cls": { "task": "classification", "dir": "cls_dir" },
      "det": { "task": "detection", "dir": "det_dir" }
    }
  })";
  return tmp.string();
}

} // namespace

TEST(model_manager_registry_interface) {
  auto path = createTempRegistry();
  ModelManager manager(path);

  auto names = manager.modelNames();
  ASSERT_EQ(names.size(), 2u);
  ASSERT_TRUE(manager.hasModel("cls"));
  ASSERT_TRUE(manager.hasModel("det"));
  ASSERT_EQ(manager.task("cls"), "classification");
  ASSERT_EQ(manager.task("det"), "detection");
  ASSERT_EQ(manager.get("cls"), nullptr);
}

TEST(model_manager_load_missing_model_throws) {
  auto path = createTempRegistry();
  ModelManager manager(path);
  ASSERT_THROW(manager.load("missing"));
}
