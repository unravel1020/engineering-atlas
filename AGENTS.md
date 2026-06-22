# Engineering Atlas Agent Rules

Version: 1.1
Last Updated: 2026-06-22

# AGENTS.md

# Engineering Atlas Agent Rules

## Project Vision

Engineering Atlas 是个人长期技术成长项目。

目标：

1. 补齐计算机基础
2. 补齐 AI 基础
3. 建立工业软件能力
4. 建立开源项目能力
5. 建立可持续演进的知识体系

核心原则：

* 长期主义
* 持续迭代
* 实践驱动
* 工程化思维
* 知识沉淀优先

---

# Development Philosophy

## Incremental Evolution

优先增量迭代。

禁止：

* 无必要推翻重构
* 每个版本重新设计架构
* 为未来需求提前设计复杂系统

优先：

* 在当前架构基础上演进
* 小步提交
* 可运行优先
* 逐步优化

---

## Learning First

当前阶段以学习为核心。

项目首先服务于：

* 理解技术
* 积累经验
* 建立知识体系

而不是追求商业化产品。

---

## Architecture Over Features

对于长期项目：

Architecture > Maintainability > Features

新增功能前优先考虑：

* 职责是否清晰
* 模块是否解耦
* 是否易于扩展
* 是否符合当前架构

---

# Repository Structure

目录代表知识领域。

不要轻易修改顶层目录结构。

当前结构：

01-Computer-Science
02-AI
03-Cpp
04-Industrial
05-Architecture
06-OpenMachineLab
07-Career

新增内容优先归入现有目录。

不要随意新增顶层目录。

---

# C++ Rules

默认使用：

C++17

优先使用：

* STL
* RAII
* 智能指针
* 标准库算法
* const正确性

避免：

* 裸new/delete
* 全局变量
* 巨型函数
* 重复代码

---

# CMake Rules

统一使用：

cmake -S . -B build

cmake --build build -j

采用：

Out-of-Source Build

不要提交：

* build/
* CMake缓存文件

---

## Clangd

每次重建工程后：

必须重新生成：

compile_commands.json

推荐流程：

cmake -S . -B build 
-DCMAKE_EXPORT_COMPILE_COMMANDS=ON

cmake --build build -j

如果工程使用：

clangd

则保持：

compile_commands.json

与当前工程同步。

修改以下内容后：

* CMakeLists.txt
* include目录
* src目录
* 模块结构

应重新生成：

compile_commands.json

---

# Git Rules

提交原则：

小步提交。

一次提交只做一件事情。

推荐：

feat:
fix:
refactor:
docs:
test:
build:

避免：

update
modify
temp

Commit Message 应描述实际改动内容。

---

# File Naming

类文件：

PascalCase

例如：

Inference.h
Inference.cpp

EventSystem.h
EventSystem.cpp

Timer.h
Timer.cpp

模块文件：

camelCase

例如：

preProcessor.h
preProcessor.cpp

postProcessor.h
postProcessor.cpp

避免：

test_new.cpp
utils2.cpp
temp.cpp

---

# Type Naming

类：

PascalCase

例如：

Inference
Machine
EventSystem

结构体：

PascalCase

例如：

TensorInfo

枚举：

PascalCase

例如：

MachineState
ModelType

---

# Variable Naming

局部变量：

camelCase

例如：

inputCount
outputCount
tensorInfo

成员变量：

snake_case

例如：

input_name
output_name
model_path

避免混用命名风格。

---

# Function Naming

成员函数：

camelCase

例如：

parseModelInfo()
printModelInfo()

工具函数：

保持领域语义优先。

例如：

loadLabels()
softmax()
topk()

避免：

doSomething()
handleData()
process()

这类无业务语义命名。

---

# Comment Rules

注释解释：

为什么这样设计。

而不是：

代码正在做什么。

不要：

// resize image
cv::resize(...)

不要：

// calculate softmax
softmax(...)

优先：

// ImageNet模型固定输入224x224

// ONNX Runtime要求NCHW布局

// 使用max-trick避免softmax数值溢出

---

# Module Design

遵循：

Single Responsibility Principle

单个模块只负责单一职责。

例如：

preProcessor

负责：

图像预处理

postProcessor

负责：

模型输出处理

Inference

负责：

模型推理

不要：

* 万能utils
* 巨型管理类
* God Object

---

# OpenMachineLab Rules

遵循：

Experiment First

先验证。

再抽象。

最后沉淀框架。

禁止：

为了未来可能需求提前设计复杂架构。

遵循：

Rule Of Three

同一种模式出现三次以上。

再考虑抽象。

---

# ADR Rules

重要架构决策应记录ADR。

ADR关注：

* 为什么这样设计
* 备选方案是什么
* 为什么放弃其它方案

不要记录：

代码实现细节。

ADR记录设计决策。

代码记录实现。

---

# AI Response Rules

修改代码时：

优先增量修改。

不要重写整个文件。

修改配置文件时：

必须提供完整配置。

不要只提供片段。

修改架构时：

先说明原因。

再说明收益。

最后给出迁移方案。

不要直接推翻现有实现。

如果现有代码能够工作：

优先修复。

不要重构。

如果不确定项目上下文：

先询问。

不要自行假设项目结构。

---

# Current Learning Focus

当前重点：

1. Linux
2. Git
3. CMake
4. Modern C++
5. OpenCV
6. ONNX Runtime
7. 软件架构
8. OpenMachineLab

当前阶段：

学习和工程能力建设优先。

暂不追求：

* 微服务
* 云原生
* 大规模分布式系统
* 过度工程化设计

---

# Decision Making

优先级：

简单 > 复杂

可运行 > 理论完美

增量演进 > 推倒重来

实践验证 > 主观推测

真实需求 > 未来假设

学习价值 > 炫技设计

## Existing Code First

分析问题时：

优先基于现有代码分析。

不要假设项目结构。

不要假设文件内容。

不要假设用户尚未完成某步骤。

如果需要判断问题原因：

先查看实际代码和实际输出。

再给出结论。

避免脱离上下文推测。

## Change Scope Control

修改代码时：

优先修改必要范围。

不要为了修复一个问题：

- 修改无关文件
- 重命名无关变量
- 调整无关格式
- 重构无关模块

一次提交只解决一个问题。