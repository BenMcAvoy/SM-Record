# SM-Record

A high-performance screen recording mod specifically for **Scrap Mechanic** that captures gameplay directly from the DirectX 11 render pipeline and saves frames as PNG images.

## 🎥 Features

- **Real-time Frame Capture**: Captures game frames directly from the DirectX 11 backbuffer at the game's target FPS
- **Zero Performance Impact**: Efficient frame capture using GPU staging textures with minimal overhead
- **In-Game Controls**: Simple overlay interface with start/stop recording and preview window
- **Automatic DLL Injection**: Debug injector automatically launches Scrap Mechanic and injects the mod
- **Frame Management**: Intelligent frame queue system with debt monitoring to prevent memory issues
- **High Quality Output**: Saves frames as PNG images using DirectX Texture (DirectXTex) library

## 🛠️ Technical Details

**Built with:**
- **DirectX 11 Hooking**: Uses [Kiero](https://github.com/Rebzzel/kiero) for universal D3D11 graphics hooking
- **ImGui Interface**: Real-time overlay using ImGui with docking support
- **DirectXTex**: High-performance texture processing and PNG export
- **MinHook**: Low-level API hooking for seamless integration
- **Modern C++**: Written in C++20 with Visual Studio 2022

**Key Components:**
- `SM-Record.dll`: Main recording module that hooks into Scrap Mechanic's render pipeline
- `DebugInjector.exe`: Launcher utility that automatically injects the DLL into the game process

## 🚀 Quick Start

### Prerequisites

- Visual Studio 2022 with C++ development tools
- Windows 10/11 (x64)
- Scrap Mechanic installed via Steam
- vcpkg package manager (configured automatically)

### Building & Running

1. **Clone the repository**
   ```bash
   git clone https://github.com/BenMcAvoy/SM-Record.git
   cd SM-Record
   ```

2. **Open in Visual Studio**
   ```bash
   # Open the solution file
   SM-Record.sln
   ```

3. **Build and Run**
   - Set build configuration to `Debug|x64` or `Release|x64`
   - Press `F5` or click "Local Windows Debugger"
   - The injector will prompt to launch Scrap Mechanic if not already running
   - The mod will automatically inject once the game is detected

### Usage

1. **Launch the game** - The injector will detect when Scrap Mechanic is running
2. **Open the recording interface** - The SM-Recorder window will appear in-game
3. **Start recording** - Click "Start Recording" to begin frame capture
4. **Stop recording** - Click "Stop Recording" when finished
5. **Find your frames** - PNG files are saved to `C:\Users\[Username]\Desktop\recording\`

## ⚡ 60FPS Recording Setup (Recommended)

For professional-quality 60FPS recordings, it's **highly recommended** to use SM-Record in combination with the [SM-TimeScaleModifier](https://github.com/crackx02/SM-TimeScaleModifier):

> ⚠️ **Important**: You will need to **modify** the TimeScaleModifier source code to set the delta time directly to `0.016` instead of using multiplication. The default TimeScaleModifier behavior won't work correctly with SM-Record.

> 📢 **Future Update**: In upcoming versions, SM-Record will include built-in time control, eliminating the need for the separate TimeScaleModifier mod.

### Why You Need Both Mods (Currently)

- **SM-Record** captures frames from the game's render pipeline
- **Modified SM-TimeScaleModifier** ensures consistent frame timing for smooth 60FPS output
- Without the time scale modifier, recordings may have inconsistent frame rates depending on your actual game FPS

### How It Works

The modified TimeScaleModifier sets the game's delta time to exactly `0.016` seconds (1/60th of a second), which:
- Forces the game to run at a consistent 60FPS timeline
- Ensures each captured frame represents exactly 1/60th of a second
- Results in smooth, professional-quality recordings regardless of your actual game performance

### Setup Instructions

1. **Download and modify SM-TimeScaleModifier**:
   - Download [SM-TimeScaleModifier](https://github.com/crackx02/SM-TimeScaleModifier)
   - **Modify the source code** to set delta time directly to `0.016` instead of multiplication
   - Build the modified version
2. **Set up SM-Record** as described in this README
3. **Launch with modified TimeScaleModifier active** before starting SM-Record
4. **Record normally** - Your output will be perfect 60FPS footage
5. **Convert to video** using your preferred tool (FFmpeg, etc.)

> 💡 **Tip**: Once SM-Record includes built-in time control (coming soon), you'll be able to skip steps 1-3 and use SM-Record standalone.

## 🔧 Configuration

### Recording Path
Edit the recording path in `SM-Record/include/state.h`:
```cpp
std::filesystem::path recordingPath = "C:\\Users\\Ben\\Desktop\\recording\\";
```

### Frame Buffer Settings
The frame queue automatically manages memory, but you can adjust the warning threshold in `main.cpp`:
```cpp
if (frameCount > 10) {
    CWARN("Frame debt: {} frames", frameCount);
}
```

## 📁 Project Structure

```
SM-Record/
├── SM-Record/              # Main recording DLL
│   ├── src/
│   │   ├── dllmain.cpp    # DLL entry point and hook setup
│   │   ├── main.cpp       # Recording logic and UI
│   │   └── hook.cpp       # DirectX hooking implementation
│   ├── include/
│   │   ├── hook.h         # Hook definitions and D3D11 render state
│   │   └── state.h        # Recording state management
│   └── vendor/            # Third-party libraries
│       ├── kiero/         # Graphics API hooking
│       └── carbon/        # Scrap Mechanic game utilities
├── DebugInjector/         # DLL injection utility
│   └── src/main.cpp       # Process detection and injection
└── x64/                   # Build outputs
    ├── Debug/
    └── Release/
```

## 🎮 How It Works

1. **Process Detection**: DebugInjector monitors for `ScrapMechanic.exe`
2. **DLL Injection**: Injects `SM-Record.dll` into the game process
3. **DirectX Hooking**: Hooks the `Present()` function of the DXGI swap chain
4. **Frame Capture**: Copies the backbuffer to a staging texture for CPU access
5. **Async Processing**: Queues captured frames for background PNG encoding
6. **File Output**: Saves numbered PNG frames to the specified directory

## 🔍 Troubleshooting

**Game won't start with injector:**
- Ensure Steam is running and Scrap Mechanic is in your library
- Try running as Administrator
- Check Windows Defender isn't blocking the injector

**No recording interface appears:**
- Verify the DLL was injected successfully
- Look for console output from the injector

**Frame drops or high memory usage:**
- The frame queue automatically manages overflow
- Consider recording at lower resolutions
- Monitor the "Frame debt" warning in console

**Inconsistent frame rates in recordings:**
- Use [SM-TimeScaleModifier](https://github.com/crackx02/SM-TimeScaleModifier) for consistent 60FPS output
- Ensure the time scale modifier is active before starting recording
- This is essential for professional-quality recordings

## 🤝 Contributing

Contributions are welcome! This Scrap Mechanic modding project serves as a great example of:
- DirectX 11 graphics programming
- DLL injection techniques
- Real-time game modification
- Modern C++ development practices

**Related Projects:**
- [SM-TimeScaleModifier](https://github.com/crackx02/SM-TimeScaleModifier) - Essential companion mod for consistent 60FPS recordings

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
