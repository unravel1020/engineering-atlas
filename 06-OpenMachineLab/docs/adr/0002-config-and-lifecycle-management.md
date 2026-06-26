# ADR 0002: 配置管理与生命周期管理

- 状态：Proposed
- 日期：2026-06-25
- 范围：`06-OpenMachineLab`

---

## 上下文

`opencv_onnx_demo` 使用两层 JSON 配置：

- `registry.json`：模型注册表
- `model.json`：单个模型的元数据

这种设计对 demo 足够，但工业级场景需要更系统的配置管理：

- 全局运行时配置（日志级别、线程数、后端选择）
- 模型级配置（预处理、后处理、标签）
- 后端级配置（设备 ID、精度模式、缓存路径）
- 配置版本化和校验
- 明确的对象生命周期（初始化 → 加载 → 运行 → 卸载 → 关闭）

---

## 决策

### 1. 三层配置模型

```
GlobalConfig
├── runtime: threads, log_level, default_backend
├── backends: [{ name, device, ... }]
└── models: [{ name, task, backend, dir }]

ModelConfig (per model)
├── task: classification | detection | ...
├── model_file
├── inputs: [{ name, dtype, preprocess }]
├── outputs: [{ name, dtype, postprocess }]
└── labels_file
```

### 2. 统一配置加载器 `ConfigManager`

- 从 JSON 文件加载全局配置
- 合并默认值，校验必填字段
- 为每个模型解析对应的 `model.json`
- 提供只读访问接口

### 3. 生命周期管理 `Application`

用一个顶层 `Application` 对象统一管理生命周期：

```
Application::create(config_path)
  -> initialize()   // init backends, logger
  -> loadModels()   // load all models declared in registry
  -> run()          // main processing loop / service entry
  -> unloadModels() // release model sessions
  -> shutdown()     // release backends
```

### 4. 向后兼容

`06-OpenMachineLab` 的第一版配置格式兼容 `opencv_onnx_demo` 的 `registry.json` + `model.json`，降低迁移成本。

---

## 后果

### 正面

- 配置集中管理，避免散落各处。
- 生命周期清晰，便于资源泄漏排查。
- 全局配置与模型配置分离，职责明确。

### 负面

- 引入配置管理抽象后，新增一个配置项需要同时修改 schema 和 loader。
- 校验逻辑需要维护，否则会出现“配置静默失效”。

---

## 备选方案

### 方案 A：继续使用 opencv_onnx_demo 的两层 JSON

不增加全局配置，每个模块自己读自己的 JSON。

- 放弃原因：工业级部署需要统一控制线程、日志、后端等全局参数。

### 方案 B：使用环境变量替代配置文件

所有配置通过环境变量传入。

- 放弃原因：环境变量难以版本化、结构化，且对多模型场景不友好。

---

## 相关文件

- `06-OpenMachineLab/include/core/GlobalConfig.h`
- `06-OpenMachineLab/include/core/ModelConfig.h`
- `06-OpenMachineLab/include/core/ConfigManager.h`
- `06-OpenMachineLab/include/core/Application.h`
