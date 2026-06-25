// Lambda 表达式：在需要的地方内联定义匿名函数，常用于算法、回调和异步任务。
// 本示例展示 lambda 捕获、泛型 lambda（C++14）以及作为算法谓词的使用。

#include <algorithm>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

int main() {
  std::vector<int> values = {1, 2, 3, 4, 5};

  // 基本 lambda：按值捕获，对每个元素翻倍。
  auto doubled = values;
  std::transform(doubled.begin(), doubled.end(), doubled.begin(),
                 [](int x) { return x * 2; });

  std::cout << "doubled: ";
  for (int v : doubled) {
    std::cout << v << " ";
  }
  std::cout << "\n";

  // 捕获外部变量：按引用捕获 threshold，实现动态过滤条件。
  int threshold = 3;
  auto it = std::find_if(values.begin(), values.end(),
                         [&threshold](int x) { return x > threshold; });
  if (it != values.end()) {
    std::cout << "first value > " << threshold << " is " << *it << "\n";
  }

  // std::function 包装 lambda，实现类型擦除，适合回调接口。
  std::function<std::string(const std::string &)> greeter =
      [](const std::string &name) { return "hello, " + name; };
  std::cout << greeter("modern cpp") << "\n";

  // 泛型 lambda（C++14）：参数使用 auto，可接受多种类型。
  auto add_one = [](auto x) { return x + 1; };
  std::cout << "add_one(3) = " << add_one(3) << "\n";
  std::cout << "add_one(3.14) = " << add_one(3.14) << "\n";

  std::cout << "[PASS] lambda demo finished\n";
  return 0;
}
