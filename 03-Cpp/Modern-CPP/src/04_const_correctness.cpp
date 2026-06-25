// const 正确性：明确表达对象是否会被修改，帮助编译器检查和优化代码。
// 本示例展示 const 成员函数、const 引用参数、const 指针/引用的区别。

#include <iostream>
#include <string>
#include <vector>

class ModelInfo {
public:
  ModelInfo(std::string name, int input_size)
      : name_(std::move(name)), input_size_(input_size) {}

  // const 成员函数承诺不修改对象状态，可被 const 对象调用。
  const std::string &name() const { return name_; }
  int inputSize() const { return input_size_; }

  // 非 const 成员函数用于真正需要修改状态的场景。
  void setInputSize(int size) { input_size_ = size; }

private:
  std::string name_;
  int input_size_ = 0;
};

// 按 const 引用传递：避免拷贝，同时保证函数不会修改传入对象。
void printModelInfo(const ModelInfo &info) {
  std::cout << "model: " << info.name() << ", input size: " << info.inputSize()
            << "\n";
  // info.setInputSize(224); // 编译错误：info 是 const
}

// 按值传递会触发拷贝；对于小对象可以接受，对于大对象优先 const 引用。
void printString(const std::string &text) { std::cout << text << "\n"; }

int main() {
  const ModelInfo squeezenet("squeezenet", 224);
  printModelInfo(squeezenet);

  ModelInfo yolo("yolov8n", 640);
  yolo.setInputSize(320); // 非 const 对象可以调用非 const 函数
  printModelInfo(yolo);

  // const 引用绑定到容器，避免拷贝大对象。
  const std::vector<float> mean = {0.485f, 0.456f, 0.406f};
  for (const auto &value : mean) {
    std::cout << value << " ";
  }
  std::cout << "\n";

  std::cout << "[PASS] const correctness demo finished\n";
  return 0;
}
