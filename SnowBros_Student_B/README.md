# Snow Bros - OOP Project

A C++ implementation of the classic Snow Bros arcade game using SFML for CS1004 Object-Oriented Programming.

## Project Overview

- **10 playable levels** with increasing difficulty
- **Boss fights** (Level 5 & Level 10)
- **Player authentication** system with database persistence
- **Gem economy** and in-game shop
- **Leaderboard** system
- **Multi-player support** (local 2-player)

## Build Instructions

### Requirements
- **C++ 17** or higher
- **CMake 3.16+**
- **SFML 2.5+**
- **Visual Studio Community 2026** (or compatible compiler)

### Building on Windows (Visual Studio)
1. Open `SnowBros_Student_B.sln` in Visual Studio
2. Select Build > Build Solution (Ctrl+Shift+B)
3. Run with F5

### Building with CMake
```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

## Project Structure

```
SnowBros_Student_B/
├── src/
│   ├── main.cpp                 # Entry point
│   ├── Game.h / Game.cpp        # Main game loop & state machine
│   ├── Entities/
│   │   ├── Player.h / .cpp
│   │   ├── Enemy.h / .cpp       # Abstract base class
│   │   ├── Projectile.h / .cpp
│   │   └── PowerUp.h / .cpp
│   ├── Systems/
│   │   ├── CollisionDetector.h / .cpp
│   │   ├── InputManager.h / .cpp
│   │   ├── AuthManager.h / .cpp
│   │   ├── DatabaseManager.h / .cpp
│   │   └── ScoreManager.h / .cpp
│   ├── UI/
│   │   ├── MainMenu.h / .cpp
│   │   ├── HUD.h / .cpp
│   │   ├── PauseMenu.h / .cpp
│   │   └── Shop.h / .cpp
│   ├── Levels/
│   │   ├── LevelManager.h / .cpp
│   │   └── LevelConfig.h
│   └── Utils/
│       ├── Logger.h / .cpp
│       └── EventBus.h / .cpp
├── assets/
│   ├── textures/
│   ├── sounds/
│   └── fonts/
├── data/
│   ├── users.db
│   ├── levels_config.json
│   └── leaderboard.txt
├── CMakeLists.txt
├── .gitignore
└── README.md
```

## Group Members

- **Student A**: Game Loop, Player Movement, Snowball Mechanics, Enemy AI, Level Manager
- **Student B**: OOP Design, UI Systems, Authentication, Database, Economy, Leaderboard

## Development Workflow

1. **Main Branch**: Production-ready code only
2. **Develop Branch**: Active development
3. **Feature Branches**: Individual features (e.g., `feature/auth-system`, `feature/shop-ui`)

### Branching Convention
- Feature: `feature/feature-name`
- Bugfix: `bugfix/bug-description`
- Hotfix: `hotfix/critical-fix`

## Key OOP Concepts Used

- ✅ **Encapsulation**: Private members with getter/setter methods
- ✅ **Inheritance**: Enemy hierarchy (Botom → FlyingEnemy → Tornado)
- ✅ **Polymorphism**: Virtual update() and draw() methods
- ✅ **Abstract Classes**: Enemy, PowerUp, Projectile base classes
- ✅ **Design Patterns**: Factory (EnemyFactory), Observer (EventBus), State (Boss phases)

## Submission Checklist

- [ ] All 10 levels implemented
- [ ] All enemy types (Botom, Flyng Fooga Foog, Tornado, Mogera, Gamakichi + 1 custom)
- [ ] Player authentication & database
- [ ] In-game shop and gem economy
- [ ] Leaderboard system
- [ ] Scoring system with chains
- [ ] Hit-box debug toggle (F1)
- [ ] All UI screens
- [ ] Stable 30 FPS minimum
- [ ] README documentation
- [ ] Demo video (3-5 minutes)

## How to Contribute

1. Create a feature branch from `develop`
2. Make your changes
3. Test thoroughly
4. Create a Pull Request with a clear description
5. Wait for review from your partner
6. Merge to `develop` after approval
7. Periodically merge `develop` → `main` for stable releases

## Contact & Questions

- For ambiguities, refer to the assignment specification
- Document any design decisions in your code comments
- Keep commit messages clear and descriptive

---

**Last Updated**: [Current Date]
