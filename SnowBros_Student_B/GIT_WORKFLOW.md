# GitHub & Project Setup Guide

## For Both Students

### What's in the Main Branch

This is the **stable foundation** that both students can pull from and push to. It contains:

✅ **Included:**
- `.gitignore` - Excludes build files and IDE artifacts
- `README.md` - Project overview and build instructions
- `DESIGN_DOCUMENT.txt` - Full OOP architecture and design patterns
- `CMakeLists.txt` - Cross-platform build configuration
- `src/` folder structure with core classes:
  - `Entities/` - Abstract base classes and initial implementations
  - `Systems/` - EventBus (Observer pattern)
  - `Game.h/cpp` - Main game loop
  - `SnowBros_Student_B.cpp` - Entry point

❌ **NOT Included:**
- `.vcxproj`, `.sln`, `.filters` files (IDE-specific)
- Compiled binaries, `.exe`, `.dll`, `.lib`
- `Debug/` and `Release/` folders
- `.vs/` folder

---

## Workflow for Team Development

### Step 1: Initial Clone (Both Students)

Student B (or whoever creates the repo):
```bash
git clone https://github.com/USERNAME/SnowBrows.git
cd SnowBrows
git checkout main
```

Student A (getting added as collaborator):
```bash
git clone https://github.com/USERNAME/SnowBrows.git
cd SnowBrows
git checkout main
```

### Step 2: Create Development Branch

```bash
git checkout -b develop
git push -u origin develop
```

### Step 3: Create Feature Branches (Daily Work)

**Student A** works on features:
```bash
git checkout develop
git pull origin develop
git checkout -b feature/student-a-snowball-mechanics
# Make changes...
git add .
git commit -m "[STUDENT_A] Snowball: implement throwing and collision detection"
git push origin feature/student-a-snowball-mechanics
```

**Student B** works on features:
```bash
git checkout develop
git pull origin develop
git checkout -b feature/student-b-auth-system
# Make changes...
git add .
git commit -m "[STUDENT_B] Auth: implement login with password hashing"
git push origin feature/student-b-auth-system
```

### Step 4: Create Pull Request (Code Review)

When your feature is ready:
1. Go to GitHub repository
2. Click **"Pull requests"** → **"New pull request"**
3. Set:
   - **Base**: `develop`
   - **Compare**: `feature/student-a-...` (your branch)
4. Add description of changes
5. Request review from your partner
6. Partner reviews and comments
7. Make fixes if needed
8. Merge when approved

### Step 5: Update Develop Branch

After merging:
```bash
git checkout develop
git pull origin develop
git branch -d feature/student-a-snowball-mechanics
```

---

## Daily Sync Routine

Start each work session:
```bash
git checkout develop
git pull origin develop        # Get latest from partner
git checkout -b feature/student-x-new-task
```

End of work session:
```bash
git add .
git commit -m "[STUDENT_X] Component: description"
git push origin feature/student-x-new-task
```

---

## Merge Develop to Main (Milestones)

Every 2-3 days when features are stable:
```bash
git checkout develop
git pull origin develop
git checkout main
git pull origin main
git merge develop
git push origin main
```

---

## Current Project Structure

```
SnowBros_Student_B/
├── CMakeLists.txt                 # Build configuration
├── SnowBros_Student_B.cpp         # main() entry point
├── README.md                      # Documentation
├── DESIGN_DOCUMENT.txt            # Architecture guide
├── .gitignore                     # Git ignore rules
│
├── src/
│   ├── Game.h / Game.cpp
│   ├── Entities/
│   │   ├── Entity.h / Entity.cpp           # Base class
│   │   ├── Enemy.h / Enemy.cpp             # Abstract enemy
│   │   └── Botom.h / Botom.cpp             # First enemy type
│   └── Systems/
│       └── EventBus.h / EventBus.cpp       # Observer pattern
│
├── assets/
│   ├── textures/                  # (Create when ready)
│   ├── sounds/                    # (Create when ready)
│   └── fonts/                     # (Create when ready)
│
└── data/
    └── (Database files, configs)  # (Create when ready)
```

---

## Division of Work

### Student A Tasks (Completed independently)
- Game loop & state management ✅ (Uses Game class from main branch)
- Player movement & physics ✅ (Create Player : Entity)
- Snowball mechanics
- Enemy AI implementations
- Level manager

### Student B Tasks (Working from main branch)
- OOP hierarchy design ✅ (Already in main)
- Concrete entity classes (keep in main)
- Authentication system
- Database layer
- UI systems
- Scoring & leaderboard

---

## Building the Project

### Option 1: Using CMake (Cross-platform)

```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
./Release/SnowBros.exe  # Windows
./SnowBros              # Linux/Mac
```

### Option 2: Using Visual Studio

1. Install CMake extension in VS
2. Open folder: `File` → `Open Folder` → Select project root
3. VS auto-configures CMake
4. Build: `Build` → `Build All`
5. Run: Press F5

---

## Important Rules

1. **Always branch from `develop`**, not main
2. **Pull before starting work**: `git pull origin develop`
3. **Commit messages must be descriptive** with student name
4. **Never push directly to main** - only merge via PR
5. **Test before pushing** - build and run locally
6. **Review your partner's code** before merging
7. **No IDE files (.vcxproj) in commits**
8. **One feature per branch** - keep it focused

---

## If Merge Conflicts Occur

```bash
git pull origin develop     # This will show conflicts
# Fix conflicts in your editor (remove << >> markers)
git add .
git commit -m "Resolve merge conflicts"
git push origin feature/your-branch
```

---

## Debugging Build Issues

If build fails:
1. Make sure SFML is installed: `vcpkg install sfml:x64-windows`
2. Clear build cache: `rm -r build/` then `mkdir build` and rebuild
3. Check CMake is finding SFML
4. Use `-DCMAKE_TOOLCHAIN_FILE` if needed

---

## Questions?

- Check DESIGN_DOCUMENT.txt for architecture
- Review existing code in `src/` before creating new classes
- Commit frequently - easier to debug small changes
- Keep `main` branch clean for final submission
