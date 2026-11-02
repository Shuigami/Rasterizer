# 🎨 Software Rasterizer 

A modern C++ software rasterizer built from scratch with multiple shading techniques and real-time rendering capabilities! 🚀

**▶️ Try it in your browser: [shuigami.github.io/Rasterizer](https://shuigami.github.io/Rasterizer/)** (compiled to WebAssembly)

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
- **Graphics**: SDL2 (window and frame presentation only; all rendering is done on the CPU)
- **Web**: Emscripten → WebAssembly, published with GitHub Actions
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

### 📦 Installation

#### Ubuntu/Debian
```bash
sudo apt update
sudo apt install cmake g++ libsdl2-dev
```

#### Fedora/CentOS
```bash
sudo dnf install cmake gcc-c++ SDL2-devel
```

#### Arch Linux
```bash
sudo pacman -S cmake gcc sdl2
```

#### macOS
```bash
brew install cmake sdl2
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

### 🌐 Web build (WebAssembly)

With the [Emscripten SDK](https://emscripten.org/docs/getting_started/downloads.html) activated:

```bash
emcmake cmake -S . -B build-web
cmake --build build-web
cd build-web && python3 -m http.server 8000   # open http://localhost:8000
```

The page around the canvas is `web/shell.html`. The web build renders at 960×540
(1920×1080 natively) and uses 1024² shadow maps to stay fast and light in the browser.
Every push to `main` rebuilds and publishes it through `.github/workflows/pages.yml`.

## 🎮 Controls

| Input | Action |
|-------|--------|
| `1`–`5` | Gallery scenes: studio, village, temple, garden, orbits |
| `6`–`9` | Animated demos: orbiting light, shadows, well model, solar system |
| Drag / wheel | Orbit / zoom the camera (gallery scenes) |
| `R` | Reset the gallery camera |
| `E` | Next shader (Phong → Toon → Flat) |
| `S` | Toggle shadow mapping (used by the orbiting light and shadows demos) |
| `W` | Toggle wireframe |
| `D` | Toggle debug logging |
| `Esc` | Quit (native only) |

## 🖼️ Gallery scenes

`src/gallery_scenes.cpp` builds the five static scenes shown on the portfolio from
procedural shapes only (boxes, spheres, cylinders, gable roofs, rings, trees): no OBJ
or texture. They share a 40° camera, a warm directional key light and a cool fill
light, Phong with ambient 0.42 (0.5 for the orbits), and no cast shadows. Since they
do not move, they are only redrawn when the camera, shader or wireframe changes.

On the web page, the same actions are also available as buttons.

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
