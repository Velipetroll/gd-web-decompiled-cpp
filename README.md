# Geometry Dash Web — Decompilation & Native C++ Port

<p align="center">
  <img src="./assets/banner.png" alt="Geometry Dash Web C++ Port" width="100%">
</p>

> # ⚠️ **Warning:** This project uses AI.
> Some people may not like AI at all for any purpose. If so, just ignore this project. Do not hate on it. I personally believe using AI for both decompilation and porting the entire engine to C++ is a completely valid use case, as doing either of these entirely by hand would take forever.

---

## 🎮 What is this project?

This repository contains two major milestones:
1. **The clean, reverse-engineered deobfuscation** of the official Geometry Dash web demo found on [geometrydash.com](https://geometrydash.com) (originally built on Phaser 3.90.0).
2. **A 100% standalone, lightweight native C++ port** built from scratch with a custom multi-backend rendering architecture (**OpenGL 1.1**, **DirectX 8**, and **DirectX 9**) over **SDL2**, engineered to replicate the exact look, feel, and mechanics of the original release **1:1**, while maximizing framerate and minimizing latency on ancient low-end hardware (such as legacy netbooks, school laptops, and Intel Atom / GMA graphics).

---

## ⚡ The Native C++ Multi-Backend Engine

A lightweight 1:1 C++ port replacing browser runtimes with a custom hardware abstraction layer (`RenderDevice`):

### Key Features
* **Multi-Backend Rendering:** Native **DirectX 8** (32-bit Intel Atom/GMA FastPath), **DirectX 9** (64-bit Windows), and **OpenGL 1.1** (Linux & fallback).
* **Intel Atom / GMA Optimization:** Pre-transformed 2D vertices (`D3DFVF_XYZRHW`) and dynamic buffers (`D3DLOCK_DISCARD`) bypass GPU vertex-processing bottlenecks on vintage Intel GMA 950/3150 hardware.
* **100% Static Standalone Binary:** Linked with `-static -static-libgcc -static-libstdc++`; zero external DLL dependencies in `build/`.
* **Automated Asset Sync:** Scans `assets/` via C++17 `std::filesystem` to index textures (`.png`) and parse BMFont files (`.fnt`).
* **Native Audio & Level Decompression:** Replaces Pako.js with system `zlib`; background music streaming and zero-latency SFX powered by `miniaudio` and `stb_vorbis`.

---

## 🖥️ Platform-Aware Settings & In-Game Selectors

The in-game user interface adapts dynamically depending on the operating system and binary architecture:

* **Main Menu UI:** Dedicated **Settings** button in the top-right corner; **Info / Credits** button located directly below it.
* **Configurable FPS Limiter:** Integrated in-game framerate limiter available in the Settings menu.
* **Conditional In-Game Renderer Selector:**
  * **Linux:** Hidden automatically (defaults exclusively to native OpenGL 1.1).
  * **Windows 64-bit:** Exposes **DirectX 9** and **OpenGL 1.1** (hides DirectX 8, which is unavailable in 64-bit Windows).
  * **Windows 32-bit:** Exposes **DirectX 8**, **DirectX 9**, and **OpenGL 1.1** (full manual control for vintage netbooks).

---

## 🛠️ Building & Running (Linux & Windows)

### Prerequisites
Make sure you have GCC, Make, SDL2, OpenGL, and zlib installed.

#### Linux (Arch Linux / Debian-based)
```bash
# Arch Linux
sudo pacman -S base-devel sdl2 mesa zlib

# Ubuntu / Debian
sudo apt install build-essential libsdl2-dev libgl1-mesa-dev zlib1g-dev
```

#### Windows — 32-bit (Recommended for Intel Atom Netbooks / GMA / DirectX 8)
Open the **MSYS2 MINGW32** terminal:
```bash
pacman -S --needed mingw-w64-i686-toolchain mingw-w64-i686-SDL2 mingw-w64-i686-zlib
```

#### Windows — 64-bit (Modern PCs / DirectX 9)
Open the **MSYS2 MINGW64** terminal:
```bash
pacman -S --needed mingw-w64-x86_64-toolchain mingw-w64-x86_64-SDL2 mingw-w64-x86_64-zlib
```

---

### Compile & Launch
The build system automatically detects your operating system, statically links runtime libraries into a single binary, bundles the `assets/` folder, and embeds the Windows icon (`resource.rc`):

```bash
# Clean previous builds
make clean

# Compile using all CPU cores in parallel
make
# (Or 'mingw32-make' on Windows MSYS2)

# Run the game
./build/GeometryDash       # Linux
./build/GeometryDash.exe   # Windows (or double-click from Explorer)
```

---

### Command-Line Renderer Flags
You can explicitly override the default graphics backend by passing flags via terminal or Windows shortcut targets:

| Flag | Target Backend | Supported Architectures |
| :--- | :--- | :--- |
| `-dx8` / `-d3d8` | **DirectX 8 (GMA FastPath)** | Windows 32-bit |
| `-dx9` / `-d3d9` | **DirectX 9 (FastPath)** | Windows 32-bit & 64-bit |
| `-opengl` / `-gl` | **OpenGL 1.1** | Linux, Windows 32-bit & 64-bit |

*Example:*
```bash
./build/GeometryDash.exe -dx8
```

---

## 📁 Repository Structure

```text
├── assets/                       # Spritesheets, audio, bitmap fonts, and level files
│   ├── banner.png                # Project banner
│   ├── GJ_WebSheet.png
│   ├── GJ_WebSheet.json
│   ├── 1.txt                     # Stereo Madness level data
│   ├── StereoMadness.mp3
│   └── *.ogg                     # Sound effects (explode_11, playSound_01, etc.)
│
├── src/                          # Native C++ Engine (SDL2 Multi-Backend)
│   ├── main.cpp                  # Entry point, architecture detection & precision frame pacing
│   ├── constants.h               # Shared physics constants, sub-stepping & blend synchronization
│   ├── render-device.h / .cpp    # Unified graphics abstraction, OpenGL 1.1 & DirectX 9 backends
│   ├── render-d3d8.cpp           # Isolated DirectX 8 FastPath backend (Win32 GMA)
│   ├── boot-scene.h / .cpp       # Asset preloader & texture catalog management
│   ├── font-helpers.h / .cpp     # BMFont (.fnt) parser & batched bitmap font renderer
│   ├── player-physics-state.h    # State machine (velocity, grounded, ship mode)
│   ├── level-data-helpers.h/.cpp # Atlas frame lookup, scale-9 and quad batching abstractions
│   ├── pako-compression.h / .cpp # Native zlib level parser & object catalog
│   ├── level-renderer.h / .cpp   # Infinite carousel ground & spatial section batching
│   ├── trail-renderer.h / .cpp   # Additive motion trail ribbon for ship mode
│   ├── sprite-layer-helper.h/.cpp# Multi-layer sprite depth/tint helper
│   ├── player.h / .cpp           # Cube/Ship controller, rotation & shard death explosion system
│   ├── tween-value.h             # Color interpolation (Tween) engine
│   ├── color-manager.h / .cpp    # Background and ground color trigger transitions
│   ├── audio-manager.h / .cpp    # Music playback, volume fading & audio-beat metering
│   ├── game-scene.h / .cpp       # Main game loop, camera tracking, HUD, settings & pause overlays
│   ├── win-effects.h / .cpp      # Expanding shockwave rings & win celebration effects
│   ├── stb_image.h               # Public domain PNG image loader
│   ├── stb_vorbis.c              # Public domain OGG Vorbis decoder (for SFX)
│   └── miniaudio.h               # Lightweight audio playback library
│
└── Makefile                      # Incremental native build system with static linking rules
```

---

## 📜 History: The JavaScript Deobfuscation

The web version on `geometrydash.com` originally shipped as a single minified and obfuscated 56,000-line bundle (`index-game.js`). 

The initial phase of this project accomplished:
1. **Vendor Separation:** Isolating the unmodified official `phaser.min.js` (3.90.0) build from the actual game logic.
2. **Deobfuscation:** Resolving ~40,000 hex lookup calls (`_0x4e0e(...)`) into human-readable strings, method identifiers, and property keys.
3. **Dead-Code Elimination:** Stripping out leftover decoder tables, arrays, and dead execution branches.
4. **Scope-Aware Renaming:** Utilizing Babel AST traversals to rename obfuscated variables into semantically meaningful identifiers based on assignments and callback signatures.
5. **Modular Decomposition:** Splitting the monolithic script into 16 clean, Prettier-formatted ES files under `src/`.

---

## 🤝 Credits & Acknowledgements

* **RobTop Games:** Creator of Geometry Dash.
* **Phaser Studio:** Developers of the Phaser HTML5 game engine.
* **Original Decompilation Contributors:** For the initial extraction and reverse-engineering of the browser bundle.
* **stb & miniaudio contributors:** For providing the rock-solid single-header C libraries (`stb_image`, `stb_vorbis`, and `miniaudio`) powering texture loading and multi-format audio playback.
