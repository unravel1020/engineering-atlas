// 智能指针替代裸指针，自动管理内存所有权，避免内存泄漏和重复释放。
// 本示例展示 unique_ptr、shared_ptr、weak_ptr 的典型用法。

#include <iostream>
#include <memory>
#include <string>
#include <vector>

class Tensor {
public:
  explicit Tensor(std::string name) : name_(std::move(name)) {
    std::cout << "Tensor(" << name_ << ") created\n";
  }
  ~Tensor() { std::cout << "Tensor(" << name_ << ") destroyed\n"; }

  const std::string &name() const { return name_; }

private:
  std::string name_;
};

// unique_ptr 表示独占所有权，适合工厂函数返回的资源。
std::unique_ptr<Tensor> makeTensor(const std::string &name) {
  return std::make_unique<Tensor>(name);
}

// shared_ptr 表示共享所有权，适合多个对象引用同一资源。
void sharedOwnerDemo() {
  auto input = std::make_shared<Tensor>("input");
  {
    auto alias = input; // 引用计数 +1
    std::cout << "inside block, use_count = " << input.use_count() << "\n";
  } // alias 离开作用域，引用计数 -1
  std::cout << "after block, use_count = " << input.use_count() << "\n";
}

// weak_ptr 用于观察 shared_ptr 管理的对象，不增加引用计数，避免循环引用。
void weakObserverDemo() {
  auto output = std::make_shared<Tensor>("output");
  std::weak_ptr<Tensor> observer = output;

  if (auto locked = observer.lock()) {
    std::cout << "observed: " << locked->name() << "\n";
  }

  output.reset(); // 释放对象
  if (observer.expired()) {
    std::cout << "observed object has been destroyed\n";
  }
}

int main() {
  // unique_ptr：所有权唯一，可移动但不可拷贝。
  auto a = makeTensor("logits");
  std::unique_ptr<Tensor> b = std::move(a); // 所有权转移
  if (!a) {
    std::cout << "a is empty after move\n";
  }

  // 容器存储 unique_ptr，管理动态对象数组。
  std::vector<std::unique_ptr<Tensor>> tensors;
  tensors.push_back(makeTensor("conv1"));
  tensors.push_back(makeTensor("conv2"));

  sharedOwnerDemo();
  weakObserverDemo();

  std::cout << "[PASS] smart pointers demo finished\n";
  return 0;
}
