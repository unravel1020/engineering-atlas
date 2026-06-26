# opencv_onnx_demo

使用 OpenCV + ONNX Runtime 进行图像分类与目标检测的最小可运行示例。

本模块用于学习现代 C++ 工程化落地 AI 推理的流程：

- 用 CMake 管理多模块构建
- 用 JSON 元数据驱动预处理、推理与后处理
- 用抽象接口隔离不同任务（分类 / 检测）的差异
- 用单元测试覆盖核心模块
- 用 Benchmark 评估推理性能

---

## 目录结构

```
.
├── app/
│   └── main.cpp              # 入口：加载注册表，遍历模型运行推理或 benchmark
├── include/                  # 公共头文件
│   ├── benchmark.h
│   ├── inference.h
│   ├── modelInfo.h
│   ├── modelLoader.h
│   ├── modelManager.h
│   ├── modelRegistry.h
│   ├── pipeline.h
│   ├── postProcessor.h
│   ├── preProcessor.h
│   ├── result.h
│   └── utils.h
├── src/                      # 模块实现
├── tests/                    # 单元测试
├── models/
│   ├── registry.json         # 模型注册表：名称 -> 任务与目录
│   ├── squeezenet1.1-7/      # 分类模型示例
│   │   ├── model.json        # 模型元数据
│   │   ├── model.onnx
│   │   ├── labels.txt
│   │   └── test_data/dog.jpg
│   └── yolov8n/              # 检测模型示例
│       ├── model.json
│       ├── model.onnx
│       ├── labels.txt
│       └── test_data/dog.jpg
├── output/                   # 可视化输出（被 .gitignore 忽略）
└── build/                    # CMake 构建目录（被顶层 .gitignore 忽略）
```

---

## 依赖

- CMake >= 3.15
- C++17 编译器
- OpenCV 4.x
- ONNX Runtime 1.16.3（已放在 `../../onnxruntime-linux-x64-1.16.3`）

---

## 构建

```bash
cd 03-Cpp/Cpp-lab/modules/opencv_onnx_demo
export ONNXRUNTIME_DIR=/home/ubuntu/workspace/engineering-atlas/03-Cpp/Cpp-lab/onnxruntime-linux-x64-1.16.3
cmake -S . -B build
cmake --build build -j
```

构建产物：

- `build/app`：主程序
- `build/unit_tests`：单元测试

> 若使用 clangd，构建后会生成 `build/compile_commands.json`，可在模块目录创建软链接使用。

---

## 运行

### 推理

```bash
./build/app
```

程序会读取 `models/registry.json`，依次运行每个模型，并将可视化结果保存到 `output/`。

### Benchmark

```bash
./build/app --benchmark
```

对每个模型进行 warmup + 多次推理，输出 mean / min / max / p50 / p90 / p99 / throughput。

### 单元测试

```bash
./build/unit_tests
```

当前包含 44 个测试，覆盖：

- `PreProcessor`：stretch / letterbox、scale / mean / std、RGB/BGR、异常参数
- `PostProcessor`：softmax、topk、YOLO 检测输出解析、NMS、边界场景
- `PipelineFactory`：分类 / 检测管道创建、未知任务拒绝
- `ModelLoader` / `ModelRegistry` / `ModelManager`：元数据加载与异常路径
- `Result`：分类 / 检测结果的 print 与 visualize
- `Benchmark`：统计计算正确性
- `Utils`：文件读取、标签加载、路径拼接
- 端到端集成测试：SqueezeNet 分类、YOLOv8n 检测

---

## 如何添加新模型

1. 在 `models/` 下新建目录，例如 `models/my_model/`
2. 放入 `model.onnx`、`labels.txt` 和测试图 `test_data/dog.jpg`
3. 编写 `model.json`，参考已有示例：

```json
{
  "name": "my_model",
  "task": "classification",
  "model_file": "model.onnx",
  "inputs": [
    {
      "name": "input",
      "type": "float32",
      "preprocess": {
        "color_format": "BGR",
        "resize_mode": "stretch",
        "target_h": 224,
        "target_w": 224,
        "scale": 0.003921568859,
        "mean": [0.0, 0.0, 0.0],
        "std": [1.0, 1.0, 1.0],
        "layout": "NCHW"
      }
    }
  ],
  "outputs": [
    {
      "name": "output",
      "type": "float32",
      "postprocess": {
        "activation": "softmax",
        "topk": 5,
        "labels_file": "labels.txt"
      }
    }
  ]
}
```

4. 在 `models/registry.json` 中注册：

```json
{
  "models": {
    "my_model": {
      "task": "classification",
      "dir": "my_model"
    }
  }
}
```

5. 重新运行 `./build/app`

---

## 模块职责

| 模块 | 职责 |
|------|------|
| `modelInfo` | 定义 `ModelInfo`、`TensorInfo`、预处理/后处理配置等值对象 |
| `modelLoader` | 读取 `model.json` 并构造 `ModelInfo` |
| `modelRegistry` | 读取 `registry.json`，维护名称到目录的映射 |
| `modelManager` | 管理模型加载、切换与当前模型会话 |
| `inference` | 封装 ONNX Runtime 会话，执行一次前向推理 |
| `preProcessor` | 根据 `PreprocessConfig` 对图像进行缩放、归一化、布局转换 |
| `postProcessor` | 根据 `PostprocessConfig` 解析分类概率或检测框 |
| `pipeline` | 串联 pre → inference → post，按任务分派具体实现 |
| `result` | 定义分类 / 检测结果的统一输出与可视化 |
| `benchmark` | 对 pipeline 进行多次推理并统计 latency |
| `utils` | 跨模块通用工具：文件读取、标签加载、路径拼接 |

---

## 设计要点

- **元数据驱动**：预处理、后处理、输入输出张量信息全部来自 JSON，添加新模型不需要改 C++ 代码。
- **任务抽象**：`Pipeline` 与 `Result` 按 `classification` / `detection` 派生，主程序不直接感知任务差异。
- **可测试**：核心逻辑拆成静态库 `opencv_onnx_demo_core`，测试与主程序链接同一份实现。
- **Out-of-Source Build**：所有生成物放在 `build/`，不污染源码目录。

---

## 注意事项

- 运行前确保 `ONNXRUNTIME_DIR` 指向正确的 ONNX Runtime 根目录。
- 输出的 `output/` 目录被 `.gitignore` 忽略，无需提交。
- 当前仅支持 CPU 推理；后续可通过扩展 `Inference` 支持 GPU / TensorRT 等后端。
