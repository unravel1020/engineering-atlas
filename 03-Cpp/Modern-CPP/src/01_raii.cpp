// Modern C++ 核心习惯：用对象生命周期管理资源，避免裸 new/delete。
// 本示例通过自定义文件句柄包装类展示 RAII 的基本形态。

#include <cstdio>
#include <iostream>
#include <stdexcept>
#include <string>

// File 封装 C 文件句柄：构造函数获取资源，析构函数释放资源。
// 这样即使发生异常或提前返回，资源也能被正确释放。
class File {
public:
  explicit File(const std::string &path, const std::string &mode)
      : file_(std::fopen(path.c_str(), mode.c_str())) {
    if (!file_) {
      throw std::runtime_error("failed to open file: " + path);
    }
  }

  // 禁用拷贝，避免重复关闭同一句柄。
  File(const File &) = delete;
  File &operator=(const File &) = delete;

  // 支持移动，转移资源所有权。
  File(File &&other) noexcept : file_(other.file_) { other.file_ = nullptr; }
  File &operator=(File &&other) noexcept {
    if (this != &other) {
      close();
      file_ = other.file_;
      other.file_ = nullptr;
    }
    return *this;
  }

  ~File() { close(); }

  void write(const std::string &text) {
    if (std::fwrite(text.data(), 1, text.size(), file_) != text.size()) {
      throw std::runtime_error("failed to write file");
    }
  }

private:
  void close() {
    if (file_) {
      std::fclose(file_);
      file_ = nullptr;
    }
  }

  std::FILE *file_ = nullptr;
};

int main() {
  try {
    File out("build/01_raii_tmp.txt", "w");
    out.write("RAII: resource is managed by object lifetime.\n");
    // 离开作用域时自动关闭文件，无需手动 fclose。
  } catch (const std::exception &e) {
    std::cerr << "error: " << e.what() << "\n";
    return 1;
  }

  std::cout << "[PASS] RAII file handle closed automatically\n";
  return 0;
}
