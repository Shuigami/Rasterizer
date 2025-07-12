# 🎨 Software Rasterizer 

A modern C++ software rasterizer built from scratch with multiple shading techniques and real-time rendering capabilities! 🚀

## ✨ Features

- 🎯 **Multiple Shading Models**
  - Phong Shading for realistic lighting
  - Toon Shading for stylized cartoon effects
  - Flat Shading for simple solid colors

- 💡 **Advanced Lighting System**
  - Point lights with configurable intensity and range
  - Real-time shadow mapping
  - Ambient, diffuse, and specular lighting components

- 🔧 **Rendering Pipeline**
  - Wireframe and solid rendering modes
  - Z-buffer depth testing
  - Triangle rasterization with proper interpolation
  - Real-time camera controls

- 🎮 **Interactive Features**
  - SDL2-based window management
  - Keyboard/mouse event handling
  - Live shader switching
  - Shadow toggle functionality

## 🛠️ Technical Stack

- **Language**: C++17
- **Graphics**: SDL2 & SDL2_image
- **Build System**: CMake
- **Architecture**: Modular design with separate components

## 📁 Project Structure

```
📦 isim-project/
├── 📂 src/           # Source code
│   ├── main.cpp      # Application entry point
│   ├── rasterizer.cpp # Core rendering engine
│   ├── mesh.cpp      # 3D mesh handling
│   ├── shader.cpp    # Shader implementations
│   ├── camera.cpp    # Camera system
│   └── logger.cpp    # Logging utilities
├── 📂 include/       # Header files
├── 📂 assets/        # 3D models (.obj files)
│   ├── car.obj       # 🚗 Car model
│   ├── cube.obj      # 📦 Cube primitive
│   ├── moto.obj      # 🏍️ Motorcycle model
│   ├── sword.obj     # ⚔️ Sword model
│   └── well.obj      # 🏛️ Well model
└── 📂 build/         # Build output
```

## 🚀 Getting Started

### Prerequisites

- **C++ Compiler** (g++, clang++)
- **CMake** (3.10+)
- **SDL2** development libraries
- **SDL2_image** development libraries

### 📦 Installation

#### Ubuntu/Debian
```bash
sudo apt update
sudo apt install cmake g++ libsdl2-dev libsdl2-image-dev
```

#### Fedora/CentOS
```bash
sudo dnf install cmake gcc-c++ SDL2-devel SDL2_image-devel
```

#### macOS
```bash
brew install cmake sdl2 sdl2_image
```

### 🔨 Building

1. **Clone the repository**
   ```bash
   git clone <repository-url>
   cd isim-project
   ```

2. **Create build directory**
   ```bash
   mkdir build
   cd build
   ```

3. **Configure with CMake**
   ```bash
   cmake ..
   ```

4. **Build the project**
   ```bash
   make -j$(nproc)
   ```

5. **Run the rasterizer**
   ```bash
   ./rasterizer
   ```

## 🎮 Controls

- **Shader Switching**: Use number keys to switch between different shading models
- **Wireframe Mode**: Toggle wireframe rendering
- **Shadow Toggle**: Enable/disable real-time shadows
- **Camera Controls**: Mouse and keyboard navigation

## 🧪 Debugging

The project includes comprehensive debugging support:

- **GDB Integration**: Pre-configured for debugging
- **Logging System**: Built-in logger for runtime information
- **Profiling**: Valgrind callgrind support included

### Debug Build
```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j$(nproc)
gdb ./rasterizer
```

## 📊 Performance

- **Resolution**: 1920x1080 default
- **Shadow Maps**: 2048x2048 resolution
- **Multi-threading**: CPU-based parallel processing
- **Memory Management**: Efficient buffer management

## 🎯 Key Components

### 🖼️ Rasterizer Engine
- Triangle rasterization with barycentric coordinates
- Depth buffer for proper 3D rendering
- Color buffer management
- SDL2 surface rendering

### 🎨 Shader System
- **PhongShader**: Realistic lighting with ambient, diffuse, and specular components
- **ToonShader**: Cartoon-style shading with configurable levels and outlines
- **FlatShader**: Simple solid color rendering

### 📷 Camera System
- Perspective projection matrix
- View matrix calculations
- Configurable field of view and aspect ratio

### 🌟 Lighting
- Point light sources
- Intensity and range controls
- Shadow mapping for realistic shadows

## 🔧 Configuration

The project uses `.clangd` for LSP support and includes comprehensive CMake configuration for cross-platform builds.

## 🤝 Contributing

Feel free to contribute to this project! Whether it's bug fixes, new features, or performance improvements, all contributions are welcome.

## 📄 License

This project is part of an academic assignment for computer graphics and rendering techniques.

---

*Built with ❤️ for learning computer graphics and software rendering techniques* 🎓
