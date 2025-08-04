# FFmpegEntity Video Generating

## 项目简介
本项目是一个基于 FFmpeg 的视频生成工具，旨在提供一套直观易用的接口，供用户进行视频生成、剪辑等操作。

## 功能特性
- 视频剪辑
- 视频转码
- 视频合并
- 其他自定义处理

## 项目结构
```
FFmpegEntity Video Generating/
├── FFmpegEntity/               # 核心功能模块
│   ├── Basic/                  # 基础功能（帧、包、格式等）
│   │   ├── Basic.h             # 基础工具类（如错误处理、数学工具）
│   │   ├── Color.h             # 颜色处理类（支持 RGBA 操作）
│   │   ├── Formats.h           # 音视频格式定义与转换
│   │   ├── Frame.h             # 帧数据封装类（支持 AVFrame 操作）
│   │   └── Packet.h            # 数据包封装类（支持 AVPacket 操作）
│   ├── Codec/                  # 编解码器相关
│   ├── Format/                 # 输入输出格式处理
│   │   ├── AVFormat.h          # 基础格式封装类
│   │   ├── AVInput.h           # 输入格式处理类
│   │   ├── AVOutput.h          # 输出格式处理类
│   │   └── Stream.h           # 流处理类
│   ├── CMakeLists.txt          # 模块构建配置
│   └── QtSupport.cpp           # 其他支持
├── resource/                   # 资源文件
├── tools/                      # 工具脚本
├── main.cpp                    # 主程序入口
├── CMakeLists.txt              # 项目构建配置
├── LICENSE                     # 许可证文件
└── README                      # 项目说明
```
## FFmpegEntity

FFmpegEntity 是一个基于 FFmpeg 的 C++ 封装库，提供了视频和音频处理的简化接口。以下是核心 API 的简要介绍：

### Basic 模块

#### 1. Basic.h
- 提供基础工具类和异常处理。
- 包含 `FFmpegError`、`FileError`、`ConvertError`、`CodecError` 和 `MemoryError` 等异常类。
- 提供模板函数 `lim` 和 `avg` 用于数值限制和加权平均计算。

#### 2. Color.h
- 定义 `Color` 类，用于表示 RGBA 颜色。
- 支持颜色通道的读写操作（`red`, `green`, `blue`, `alpha`）。
- 提供与 `QRgb` 的相互转换功能。

#### 3. Formats.h
- 定义视频和音频格式类 `VideoFormat` 和 `AudioFormat`。
- 提供格式转换工具 `Swscale`（视频）和 `SwResample`（音频）。
- 支持格式比较操作（`==` 和 `!=`）。

#### 4. Frame.h
- 定义 `Frame` 类，封装 `AVFrame`，用于视频帧和音频帧的处理。
- 提供 `VideoFrame` 类，支持视频帧的创建、修改和转换。
- 提供 `AudioBuffer` 类，用于音频数据的缓冲和处理。

#### 5. Packet.h
- 定义 `Packet` 类，封装 `AVPacket`，用于音视频数据包的传输。
- 支持数据包的克隆和引用计数管理。

### Codec 模块

#### 1. Codec.h
- 定义 `Codec` 模板类，封装编解码器的通用逻辑。
- 提供 `BasicEncoder` 和 `BasicDecoder` 作为基础编解码器。
- 支持编解码器的发送（`send`）、接收（`receive`）和重置（`reset`）操作。

#### 2. Encoder.h
- 定义 `Encoder` 类，继承自 `BasicEncoder`，用于音视频编码。
- 支持配置编码参数（如 `bitRate` 和 `options`）。
- 提供 `configure` 方法用于编码器配置。

#### 3. Decoder.h
- 定义 `Decoder` 类，继承自 `BasicDecoder`，用于音视频解码。
- 支持从 `AVStream` 初始化解码器。

#### 4. AudioEncoder.h
- 定义 `AudioEncoder` 类，继承自 `Encoder`，专用于音频编码。
- 支持音频格式转换和缓冲区管理。

#### 5. VideoEncoder.h
- 定义 `VideoEncoder` 类，继承自 `Encoder`，专用于视频编码。
- 支持视频帧格式转换和帧率控制。

### Format 模块

#### 1. AVFormat.h
- 定义 `AVFormat` 类，封装 `AVFormatContext`，提供基础格式处理功能。
- 支持格式上下文的初始化和释放。

#### 2. AVInput.h
- 定义 `AVInput` 类，继承自 `AVFormat`，用于输入格式处理。
- 支持从 URL 打开输入流，并提供流索引和元数据访问。

#### 3. AVOutput.h
- 定义 `AVOutput` 类，继承自 `AVFormat`，用于输出格式处理。
- 支持创建输出文件，并管理输出流和元数据。

#### 4. Stream.h
- 定义 `BasicStream`、`ReadStream` 和 `WriteStream` 类，用于流处理。
- `ReadStream` 支持从输入流解码数据。
- `WriteStream` 支持向输出流编码数据。

## 构建与运行

### 依赖安装
1. 确保已安装 FFmpeg 库：
   ```bash
   sudo apt-get install ffmpeg
   ```
2. 安装 CMake（用于构建项目）：
   ```bash
   sudo apt-get install cmake
   ```

### 构建项目
1. 克隆项目到本地：
   ```bash
   git clone <项目仓库地址>
   cd FFmpegEntity-Video-Generating
   ```
2. 创建构建目录并编译：
   ```bash
   mkdir build && cd build
   cmake ..
   make
   ```

### 运行示例
1. 运行主程序：
   ```bash
   ./main
   ```
2. 使用示例代码（参考 `main.cpp` 或 `README` 中的示例）。

## 使用示例

```cpp
#include "FFmpegEntity/Basic/Frame.h"
#include "FFmpegEntity/Format/AVInput.h"
#include "FFmpegEntity/Format/AVOutput.h"

int main() {
    // 打开输入文件
    FFmpeg::AVInput input("input.mp4");
    
    // 创建输出文件
    FFmpeg::AVOutput output("output.mp4", { /* 编码器列表 */ });
    
    // 读取并处理数据
    auto frames = input[0].decode(10.0); // 解码前10秒数据
    output[0].encode(frames); // 编码并写入输出文件
    
    return 0;
}
```

## 注意事项
- 项目依赖 FFmpeg 库，请确保已正确安装并配置环境变量。
- 所有类均支持移动语义（`swap` 和移动构造函数）。
- 异常处理机制完善，建议捕获 `FFmpegError` 或其子类以处理错误。
- 如需扩展功能，请参考模块设计文档或联系开发者。