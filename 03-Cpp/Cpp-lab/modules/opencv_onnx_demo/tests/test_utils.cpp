#include "test_framework.h"
#include "utils.h"
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

TEST(utils_read_text_file_reads_content) {
  fs::path tmp = fs::temp_directory_path() / "utils_read_test.txt";
  {
    std::ofstream ofs(tmp);
    ofs << "hello\nworld";
  }

  std::string content = utils::readTextFile(tmp.string());
  ASSERT_EQ(content, "hello\nworld");
}

TEST(utils_read_text_file_missing_throws) {
  ASSERT_THROW(utils::readTextFile("/nonexistent/path/file.txt"));
}

TEST(utils_load_labels_preserves_empty_lines) {
  fs::path tmp = fs::temp_directory_path() / "utils_labels_test.txt";
  {
    std::ofstream ofs(tmp);
    ofs << "cat\n\ndog\n";
  }

  auto labels = utils::loadLabels(tmp.string());
  ASSERT_EQ(labels.size(), 3u);
  ASSERT_EQ(labels[0], "cat");
  ASSERT_EQ(labels[1], "");
  ASSERT_EQ(labels[2], "dog");
}

TEST(utils_load_labels_missing_throws) {
  ASSERT_THROW(utils::loadLabels("/nonexistent/path/labels.txt"));
}

TEST(utils_join_path_combines_components) {
  ASSERT_EQ(utils::joinPath("a", "b"), "a/b");
  ASSERT_EQ(utils::joinPath("a/", "b"), "a/b");
  ASSERT_EQ(utils::joinPath("", "b"), "b");
  ASSERT_EQ(utils::joinPath("a", ""), "a");
}
