# Linux C++ 工具链与 CMake 初探

## 环境检查

查看 GCC：

```bash
g++ --version
```

查看 Clang：

```bash
clang++ --version
```

查看 CMake：

```bash
cmake --version
```

查看 clangd：

```bash
clangd --version
```

查看 GDB：

```bash
gdb --version
```

查看安装路径：

```bash
which g++
which clang++
which clangd
which cmake
which gdb
```

---

## 安装 LLVM 工具链

更新软件源：

```bash
sudo apt update
```

安装：

```bash
sudo apt install -y \
clang \
clangd \
clang-format \
clang-tidy \
lld \
gdb
```

验证：

```bash
clang++ --version

clangd --version

clang-format --version

clang-tidy --version
```

---

## 第一个 CMake 项目

目录结构：

```text
Cpp-lab
├── app
│   └── main.cpp
├── timer
│   ├── Timer.cpp
│   └── Timer.h
└── CMakeLists.txt
```

---

## CMake 工作流程

### 生成构建系统

```bash
cmake -B build
```

生成：

```text
build
├── Makefile
├── CMakeCache.txt
└── CMakeFiles
```

注意：

```text
cmake -B build
只是生成构建系统

不会编译代码
```

---

### 编译项目

```bash
cmake --build build
```

输出：

```text
Built target timer
Built target main
```

---

### 运行程序

```bash
./build/main
```

---

## 常用 CMake 命令

Debug：

```bash
cmake -B build \
    -DCMAKE_BUILD_TYPE=Debug
```

Release：

```bash
cmake -B build \
    -DCMAKE_BUILD_TYPE=Release
```

导出 compile_commands：

```bash
cmake -B build \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
```

删除构建目录：

```bash
rm -rf build
```

重新构建：

```bash
rm -rf build

cmake -B build

cmake --build build
```

---

## clangd

查看版本：

```bash
clangd --version
```

功能：

* 自动补全
* 跳转定义
* 查找引用
* 重构
* 错误提示

复杂项目建议：

```bash
cmake -B build \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
```

生成：

```text
build/compile_commands.json
```

---

## GDB 调试

启动：

```bash
gdb ./build/main
```

进入：

```text
(gdb)
```

---

### 设置断点

函数断点：

```gdb
b main
```

行号断点：

```gdb
b 20
```

查看断点：

```gdb
info breakpoints
```

删除断点：

```gdb
delete
```

---

### 运行程序

```gdb
r
```

---

### 单步调试

进入函数（类似 VS F11）：

```gdb
s
```

下一步（类似 VS F10）：

```gdb
n
```

继续运行：

```gdb
c
```

---

### 查看变量

查看变量：

```gdb
p x
```

查看局部变量：

```gdb
info locals
```

---

### 查看调用栈

```gdb
bt
```

---

### 退出

```gdb
q
```

---

## VSCode 推荐插件

必装：

```text
clangd
CMake Tools
CodeLLDB
GitLens
Error Lens
```

如果完全使用 clangd：

```text
C/C++ 插件可以不安装
```

---

## 今日成果

完成：

* LLVM 工具链安装
* Clang / clangd
* CMake 基础使用
* 静态库链接
* VSCode + clangd
* GDB 基础调试

下一阶段：

```text
03-Cpp
├── 多目录 CMake
├── clang-format
├── clang-tidy
├── CTest
├── GoogleTest
└── GitHub Actions
```
