# Tetris
俄罗斯方块小游戏

## Run in VS Code
1. Install one of:
   - Visual Studio/Build Tools (MSBuild + C++ workload), or
   - MinGW-w64 toolchain (g++, windres) and add `mingw64/bin` to PATH.
2. Open this folder in VS Code with the C/C++ extension installed.
3. Press `Ctrl+Shift+B`:
   - Default task uses MinGW (`build (mingw x64)`); it will call `windres` then `g++` and produce `Tetris.exe` in the project root.
   - MSBuild tasks remain available (`msbuild Debug|x64` / `Win32`) if you have VS installed.
4. Go to "Run and Debug":
   - Use "Launch Tetris (mingw x64)" for the MinGW build, or
   - "Launch Tetris (Debug x64/Win32)" for MSBuild outputs (`x64/Debug/Tetris.exe` or `Debug/Tetris.exe`).
