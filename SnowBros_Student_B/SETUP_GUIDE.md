# Environment Setup Guide

## Prerequisites

Before you can build and run Snow Bros, you need to set up your development environment.

### Requirements
- **C++ 17** or higher
- **CMake 3.16+**
- **Visual Studio Community 2026** (already installed ✓)
- **SFML 2.5+**
- **Git** (for version control)

---

## Step 1: Install SFML (Windows)

SFML is required for graphics and windowing.

### Option A: Using vcpkg (Recommended)

1. **Install vcpkg** (if not already installed):
   ```bash
   cd C:\
   git clone https://github.com/Microsoft/vcpkg.git
   cd vcpkg
   .\bootstrap-vcpkg.bat
   ```

2. **Install SFML**:
   ```bash
   .\vcpkg install sfml:x64-windows
   ```

3. **Integrate with Visual Studio** (one-time):
   ```bash
   .\vcpkg integrate install
   ```

### Option B: Download Pre-built

1. Download SFML for Visual Studio from: https://www.sfml-dev.org/download/sfml/2.5.1/
2. Extract to a known location (e.g., `C:\SFML-2.5.1`)
3. Update CMakeLists.txt with the path

---

## Step 2: Install CMake

1. Download from: https://cmake.org/download/
2. Install with default settings
3. Verify: Open PowerShell and run:
   ```bash
   cmake --version
   ```

---

## Step 3: Build the Project

### Using Visual Studio with CMake

1. In Visual Studio: `File` → `Open Folder` → Select `SnowBros_Student_B`
2. VS will auto-detect CMakeLists.txt
3. Choose your configuration (Release recommended)
4. Build: `Build` → `Build All` or press Ctrl+Shift+B

### Using Command Line

```bash
cd SnowBros_Student_B
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build . --config Release
```

### After Successful Build

The executable will be in:
- `build/Release/SnowBros.exe` (on Windows)
- `build/SnowBros` (on Linux/Mac)

Run it:
```bash
cd build/Release
./SnowBros.exe
```

---

## Step 4: First-Time Git Setup

Both students need to set up Git credentials:

```bash
git config --global user.name "Your Full Name"
git config --global user.email "your.email@example.com"
```

To make it repository-specific (recommended for group work):
```bash
cd SnowBros_Student_B
git config user.name "Student B"
git config user.email "studentb@school.edu"
```

---

## Troubleshooting

### "SFML not found" Error

If CMake can't find SFML:

1. **Using vcpkg** (automatic):
   ```bash
   cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
   ```

2. **Manual path**:
   Edit CMakeLists.txt and add:
   ```cmake
   set(SFML_DIR "C:/path/to/SFML/lib/cmake/SFML")
   ```

### Build Succeeds but Fails to Run

1. Make sure SFML DLLs are in the executable directory
2. With vcpkg, they're automatically copied
3. If manual install, copy `SFML/bin/*.dll` to `build/Release/`

### CMake Cache Issues

```bash
cd build
rm -r *           # or del * on Windows
cmake .. -DCMAKE_TOOLCHAIN_FILE=<path>
cmake --build . --config Release
```

---

## Typical First Build (After Clone)

```bash
# 1. Clone repository
git clone https://github.com/USERNAME/SnowBrows.git
cd SnowBrows

# 2. Create build directory
mkdir build
cd build

# 3. Configure with CMake (using vcpkg)
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake -G "Visual Studio 17 2022"

# 4. Build
cmake --build . --config Release

# 5. Run
./Release/SnowBros.exe
```

---

## For Student A and Student B

**First time setting up:**
1. Clone the repository
2. Follow the steps above
3. Verify the project builds
4. You should see a black window when running
5. Close the window (ESC key)

**If build fails:**
1. Check that SFML is installed: `vcpkg list` should show `sfml`
2. Try clearing build cache and rebuilding
3. Check CMakeLists.txt includes all source files
4. Compare your system with partner's - should be identical

---

## Daily Development Workflow

After initial setup, each work session:

```bash
# 1. Pull latest changes
git pull origin develop

# 2. Create/switch to your feature branch
git checkout -b feature/student-x-task-name

# 3. Make changes in your preferred IDE

# 4. Rebuild to verify
cd build
cmake --build . --config Release

# 5. If successful, commit and push
cd ..
git add src/              # Only add source files
git commit -m "[STUDENT_X] Component: description"
git push origin feature/student-x-task-name
```

---

## IDEs Supported

### Visual Studio Community 2026 ✓
- Native CMake support
- Open folder → auto-detects CMakeLists.txt
- Built-in debugging

### Visual Studio Code (Alternative)
- Install "CMake Tools" extension
- Open folder → auto-configures
- F5 to build and run

### JetBrains CLion (Alternative)
- Native CMake support
- Auto-configures on open
- Excellent debugging tools

---

## Questions?

If you get stuck:
1. Check this guide first
2. Review GIT_WORKFLOW.md for collaboration issues
3. Compare build outputs with your partner
4. Check the README.md for project structure

Good luck! 🎮
