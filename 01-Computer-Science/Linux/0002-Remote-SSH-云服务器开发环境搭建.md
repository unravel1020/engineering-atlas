# Remote SSH 云服务器开发环境搭建

## 背景

目标：

* 使用本地 Windows + VSCode
* 连接腾讯云 Ubuntu 服务器
* 实现远程开发
* Linux 编译运行
* GitHub 仓库管理
* Clang/CMake 开发环境

最终架构：

```text
Windows
│
├── VSCode
│
└── Remote SSH
        │
        ▼
Ubuntu Server
        │
        ├── Git
        ├── CMake
        ├── GCC/Clang
        ├── GDB
        └── GitHub
```

---

## 1. 测试 SSH 连接

本地 CMD：

```bash
ssh ubuntu@175.178.171.149
```

首次连接：

```text
Are you sure you want to continue connecting?
```

输入：

```text
yes
```

然后输入服务器密码。

成功后：

```bash
ubuntu@VM-0-6-ubuntu:~$
```

说明已经进入远程 Linux 环境。

---

## 2. VSCode 安装 Remote SSH

安装插件：

```text
Remote - SSH
```

发布者：

```text
Microsoft
```

---

## 3. 配置 SSH Host

打开：

```text
Ctrl + Shift + P
```

执行：

```text
Remote-SSH: Add New SSH Host
```

输入：

```text
ssh ubuntu@175.178.171.149
```

保存到：

```text
~/.ssh/config
```

---

## 4. 连接远程主机

执行：

```text
Remote-SSH: Connect to Host
```

选择：

```text
ubuntu@175.178.171.149
```

首次连接会自动安装：

```text
.vscode-server
```

目录：

```bash
~/.vscode-server
```

---

## 5. 验证 VSCode Server

服务器执行：

```bash
ps -ef | grep vscode
```

查看 VSCode Server 是否运行。

查看安装目录：

```bash
ls -la ~/.vscode-server
```

---

## 6. 创建工作区

```bash
mkdir -p ~/workspace

cd ~/workspace
```

创建目录：

```bash
mkdir engineering-atlas
mkdir linux-lab
mkdir ai-lab
mkdir openmachinelab
mkdir experiments
```

目录结构：

```text
workspace
├── engineering-atlas
├── linux-lab
├── ai-lab
├── openmachinelab
└── experiments
```

---

## 7. 初始化 Git 仓库

进入项目：

```bash
cd engineering-atlas
```

初始化：

```bash
git init
```

修改默认分支：

```bash
git branch -m main
```

查看状态：

```bash
git status
```

---

## 8. 配置 Git 用户信息

查看配置：

```bash
git config --global --list
```

配置用户名：

```bash
git config --global user.name "unravel1020"
```

配置邮箱：

```bash
git config --global user.email "1551751306@qq.com"
```

再次确认：

```bash
git config --global --list
```

---

## 9. 关联 GitHub 仓库

查看远程仓库：

```bash
git remote -v
```

新增远程仓库：

```bash
git remote add origin https://github.com/unravel1020/engineering-atlas.git
```

确认：

```bash
git remote -v
```

输出：

```text
origin xxx(fetch)
origin xxx(push)
```

---

## 10. 首次提交

添加文件：

```bash
git add .
```

提交：

```bash
git commit -m "init engineering atlas"
```

推送：

```bash
git push -u origin main
```

成功后：

```text
Branch 'main' set up to track remote branch 'main'
```

---

## 11. 日常工作流

查看状态：

```bash
git status
```

查看提交记录：

```bash
git log --oneline
```

提交：

```bash
git add .

git commit -m "update"
```

推送：

```bash
git push
```

拉取：

```bash
git pull
```

---

## 总结

完成：

* VSCode Remote SSH
* Ubuntu 开发环境
* Git 配置
* GitHub 远程仓库连接
* Linux 远程开发工作流

以后开发流程：

```text
VSCode
↓
Remote SSH
↓
Ubuntu
↓
Git Commit
↓
Git Push
↓
GitHub
```
