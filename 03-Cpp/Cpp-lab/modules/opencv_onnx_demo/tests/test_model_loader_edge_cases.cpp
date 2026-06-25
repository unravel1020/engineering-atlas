#include "modelLoader.h"
#include "test_framework.h"
#include <filesystem>
#include <fstream>
#include <string>

namespace fs = std::filesystem;

TEST(model_loader_missing_model_json_throws) {
  fs::path tmp = fs::temp_directory_path() / "test_model_missing";
  fs::create_directories(tmp);
  // No model.json written.
  ASSERT_THROW(ModelLoader::load(tmp.string(), "missing"));
}

TEST(model_loader_malformed_json_throws) {
  fs::path tmp = fs::temp_directory_path() / "test_model_malformed";
  fs::create_directories(tmp);
  {
    std::ofstream ofs(tmp / "model.json");
    ofs << "{ invalid json }";
  }
  ASSERT_THROW(ModelLoader::load(tmp.string(), "malformed"));
}

TEST(model_loader_missing_inputs_field_throws) {
  fs::path tmp = fs::temp_directory_path() / "test_model_no_inputs";
  fs::create_directories(tmp);
  {
    std::ofstream ofs(tmp / "model.json");
    ofs << R"({ "name": "x", "task": "classification" })";
  }
  ASSERT_THROW(ModelLoader::load(tmp.string(), "no_inputs"));
}
