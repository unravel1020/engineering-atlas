// C++17 常用特性：结构化绑定、if/switch 初始化语句、std::optional、std::string_view。
// 本示例展示这些特性如何让代码更简洁、更安全。

#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <tuple>

// 返回多个值的函数：使用 std::tuple + 结构化绑定接收。
std::tuple<int, int, std::string> parseShape(std::string_view text) {
  // 简化处理：直接返回固定值，实际场景可解析字符串。
  return {1, 3, std::string(text)};
}

// std::optional 表示可能不存在的值，避免使用特殊值（如 -1）表示无效。
std::optional<int> findIndex(const std::string &text, char ch) {
  if (auto pos = text.find(ch); pos != std::string::npos) {
    return static_cast<int>(pos);
  }
  return std::nullopt;
}

int main() {
  // 结构化绑定：一行拆解 tuple。
  auto [batch, channel, name] = parseShape("input");
  std::cout << "shape: [" << batch << ", " << channel << ", " << name
            << "]\n";

  // if 初始化语句：限制变量作用域，避免污染外部命名空间。
  if (auto idx = findIndex("hello", 'l'); idx.has_value()) {
    std::cout << "found at index " << idx.value() << "\n";
  } else {
    std::cout << "not found\n";
  }

  // std::string_view：只读字符串引用，零拷贝传递字符串字面量和子串。
  constexpr std::string_view model_name = "squeezenet1.1-7";
  std::cout << "model: " << model_name << ", length: " << model_name.size()
            << "\n";

  // 结构化绑定用于 map/optional 等场景（此处用 pair 演示）。
  std::pair<int, std::string> kv = {1000, "classification"};
  auto [id, task] = kv;
  std::cout << "task id=" << id << ", task=" << task << "\n";

  std::cout << "[PASS] C++17 features demo finished\n";
  return 0;
}
