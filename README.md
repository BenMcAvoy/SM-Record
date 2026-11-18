# SM-Record

SM-Record is an experimental Scrap Mechanic mod that captures frames from the game's Direct3D 11 render pipeline and saves them as numbered PNG files. It combines a small in-game ImGui interface with a DirectX hook and a separate injector project.

This project is a prototype. It saves image sequences rather than encoding a video, and capture can add rendering and disk I/O work. The output path is currently set in `SM-Record/include/state.h` and defaults to `C:\\Users\\Ben\\Desktop\\recording\\`; change it to a suitable directory before building.

## Build

The project targets Windows and uses the Visual Studio solution `SM-Record.sln`. You will need Visual Studio 2022 with the C++ desktop workload, Windows SDK and the vcpkg dependencies declared in `SM-Record/vcpkg.json`.

Open the solution, select an x64 configuration, and build the solution. It contains the `SM-Record` DLL and the `DebugInjector` utility. The injector is intended to launch or attach to Scrap Mechanic and load the mod; use it only with your own game installation.

## Use

When loaded into the game, the overlay has controls to start and stop capture and to clear the current recording. Captured frames are written as `frame_0.png`, `frame_1.png`, and so on. The overlay can be hidden or shown with **Insert**; **End** exits the hook loop.

The capture path and frame handling are still under development. The project does not currently produce a video file or guarantee a particular frame rate.

## Implementation notes

- Kiero hooks the Direct3D 11 swap chain's `Present` and `ResizeBuffers` calls.
- The overlay is rendered with Dear ImGui.
- The current frame is copied through a staging texture and saved with DirectXTex / Windows Imaging Component.
- `DebugInjector` is a separate helper for loading the DLL into the game process.

## License

See [LICENSE](LICENSE) for the project license. Third-party components retain their own licenses.
