// 移动语义：通过转移资源所有权避免不必要的拷贝，提升性能。
// 本示例展示移动构造函数、移动赋值运算符，以及 std::move 的用法。

#include <iostream>
#include <string>
#include <utility>
#include <vector>

class Buffer {
public:
  explicit Buffer(std::size_t size) : size_(size), data_(new float[size]) {
    std::cout << "Buffer(" << size_ << ") allocated\n";
  }

  // 拷贝构造函数：深拷贝资源。
  Buffer(const Buffer &other)
      : size_(other.size_), data_(new float[other.size_]) {
    std::copy(other.data_, other.data_ + size_, data_);
    std::cout << "Buffer copied\n";
  }

  // 移动构造函数：转移资源所有权，不分配新内存。
  Buffer(Buffer &&other) noexcept : size_(other.size_), data_(other.data_) {
    other.size_ = 0;
    other.data_ = nullptr;
    std::cout << "Buffer moved\n";
  }

  // 移动赋值运算符。
  Buffer &operator=(Buffer &&other) noexcept {
    if (this != &other) {
      delete[] data_;
      size_ = other.size_;
      data_ = other.data_;
      other.size_ = 0;
      other.data_ = nullptr;
      std::cout << "Buffer move-assigned\n";
    }
    return *this;
  }

  // 禁用拷贝赋值以保持示例简洁；实际项目可按需实现。
  Buffer &operator=(const Buffer &) = delete;

  ~Buffer() {
    delete[] data_;
    std::cout << "Buffer(" << size_ << ") destroyed\n";
  }

  std::size_t size() const { return size_; }

private:
  std::size_t size_ = 0;
  float *data_ = nullptr;
};

// 工厂函数按值返回大对象；编译器会优化为移动/返回值优化。
Buffer createBuffer(std::size_t size) { return Buffer(size); }

int main() {
  Buffer a(1000);
  Buffer b = std::move(a); // 显式转移所有权

  if (a.size() == 0) {
    std::cout << "a is empty after move\n";
  }

  std::vector<Buffer> buffers;
  buffers.reserve(2);
  buffers.push_back(createBuffer(500));  // 移动进容器
  buffers.push_back(createBuffer(1500)); // 移动进容器

  std::cout << "[PASS] move semantics demo finished\n";
  return 0;
}
