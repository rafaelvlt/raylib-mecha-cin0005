# NEPTUNE'S SPEAR

[🇧🇷 Leia em português](./README_pt.md)

# English

Neptune's Spear is a first-person 3D mech project built in pure C with Raylib. It is primarily a low-level codebase exploring engine-style architecture in C: a custom Entity Component System (ECS), explicit memory and resource management, modular game systems and centralized managers for resources, events and game state.

> **Inspired by the MechWarrior franchise**

![NEPTUNE'S SPEAR](https://img.shields.io/badge/Status-In%20Development-yellow)
![C](https://img.shields.io/badge/Language-C-blue)
![Raylib](https://img.shields.io/badge/Framework-Raylib-red)

## 🎬 Preview

![Missile lock-on and salvo](./images/missile-salvo.gif)
*Locking a target and firing a guided missile salvo from the player's mech.*

![Second level gameplay](./images/boss-img.png)
*Second level encounter showcasing the 3D environment and enemy mech.*

![Main menu](./images/main-menu.png)
*Main menu layout with the 3D mech preview and navigation options.*

## 📋 Table of Contents

- [About](#about)
- [Technologies](#technologies)
- [Requirements](#requirements)
- [Installation](#installation)
- [Compilation](#compilation)
- [How to Play](#how-to-play)
- [Project Structure](#project-structure)
- [Architecture](#architecture)
- [Roadmap](#roadmap)
- [Authors](#authors)
- [License](#license)

## 🎮 About

NEPTUNE'S SPEAR is primarily a programming project about how to structure a small 3D first-person mech game in C. The basic FPS mechanics exist mainly to exercise the architecture: a custom Entity Component System (ECS), explicit state machines for enemy behaviour, decoupled systems (rendering, movement, combat, HUD) and centralized managers for resources, events and game state.

This project was developed as a final project for the Imperative Programming course (CIN0005/06) at the Informatics Center of UFPE.

## 🛠 Technologies

### Development
- **C (C99)**: Main language
- **Raylib 5.6-dev**: Graphics and audio framework
- **Raymath**: Math library for 3D operations
- **Make**: Build system

### Assets
- **Blender**: 3D modeling and mech animations
- **Audacity**: Audio editing and sound effects

## 📦 Requirements

### Linux

```bash
# Basic dependencies
sudo apt-get update
sudo apt-get install build-essential libgl1-mesa-dev libx11-dev libxrandr-dev libxi-dev libxcursor-dev libxinerama-dev libasound2-dev
```

### Windows

- MinGW-w64 or Visual Studio
- OpenGL 3.3+
- Windows audio libraries

### macOS

```bash
# Using Homebrew
brew install raylib
```

## 🚀 Installation

1. Clone the repository:
```bash
git clone https://github.com/rafaelvlt/raylib-mecha-cin0005.git
cd raylib-mecha-cin0005
```

2. Initialize submodules:
```bash
git submodule update --init --recursive
```

Note: Alternatively, you can clone with submodules in one step:
```bash
git clone --recurse-submodules https://github.com/rafaelvlt/raylib-mecha-cin0005.git
```

3. Compile Raylib first:
```bash
cd lib/raylib/src
make PLATFORM=PLATFORM_DESKTOP
cd ../../..
```

4. Compile and run the game:
```bash
make run
```

## 🔨 Compilation

### Compile the project

```bash
make
```

### Compile and run

```bash
make run
```

### Clean compiled files

```bash
make clean
```

The executable will be generated as `neptune_spear` (Linux/macOS) or `neptune_spear.exe` (Windows).

## 🎯 How to Play

### Controls

#### Movement
- **W / Up Arrow**: Move forward/backward
- **A / D or Left/Right Arrows**: Turn the mech
- **Mouse**: Rotate torso and aim (independent of legs)

#### Combat
- **Left Mouse Button**: Shoot
- **Right Mouse Button**: Zoom
- **1, 2, 3, 4, 5**: Toggle weapon groups
- **TAB**: Lock-on (target lock)

#### Utilities
- **C**: Center torso to legs
- **F**: Center legs to torso

## 📁 Project Structure

```
raylib-mecha-cin0005/
├── include/              # Headers
│   ├── ecs/            # Entity-Component-System
│   │   ├── components.h
│   │   ├── entitymanager.h
│   │   ├── systems.h
│   │   └── types.h
│   ├── screens/        # Game screens
│   ├── systems/        # Main systems
│   └── utility.h
├── src/                # Source code
│   ├── ecs/           # ECS systems
│   │   ├── ai_systems.c
│   │   ├── collision_system.c
│   │   ├── generic_systems.c
│   │   ├── health_system.c
│   │   ├── hud_system.c
│   │   ├── player_systems.c
│   │   ├── projectile_system.c
│   │   └── weapon_system.c
│   ├── screens/       # Screen implementations
│   └── systems/       # Managers
│       ├── audio_manager.c
│       ├── config_manager.c
│       ├── event_manager.c
│       ├── map_loader.c
│       ├── resource_manager.c
│       └── state_manager.c
├── resources/         # Game assets
│   ├── fonts/        # Fonts
│   ├── maps/         # Map files (.map)
│   ├── models/       # 3D models (.glb)
│   ├── musics/       # Background music
│   ├── sounds/       # Sound effects
│   └── textures/     # Textures
├── lib/              # External libraries
│   └── raylib/      # Raylib framework
├── Makefile         # Build system
├── config.ini       # Game configuration
└── README.md        # This file
```

## 🏗 Architecture

The project uses an **ECS (Entity-Component-System)** architecture to manage entities efficiently and modularly.

### Main Components

- **TransformComponent**: Position, rotation and scale
- **PhysicsComponent**: Velocity and acceleration
- **RenderComponent**: 3D model and materials
- **AnimationComponent**: Animation state
- **HealthComponent**: Health and damage reactions
- **WeaponComponent**: Weapon statistics
- **AIControlComponent**: AI behavior
- **CollisionComponent**: Collision boxes

### Main Systems

- **PlayerControlSystem**: Player control
- **AIControlSystem**: Enemy AI
- **WeaponSystem**: Firing logic
- **CollisionSystem**: Collision detection
- **AnimationSystem**: Animation updates
- **RenderSystem**: 3D rendering
- **HealthSystem**: Damage processing

### Execution Flow

```
main
  │
  ├── Initialization
  │   ├── Config Manager
  │   ├── Audio Manager
  │   ├── Resource Manager
  │   ├── Event Manager
  │   └── State Manager
  │
  └── Game Loop
      │
      ├── Update Phase
      │   ├── State Manager
      │   │   └── Current Screen Update
      │   │       ├── Player Control
      │   │       └── ECS Systems
      │   │           ├── AI Control
      │   │           ├── Animation
      │   │           ├── Movement
      │   │           ├── Weapons
      │   │           ├── Collision
      │   │           └── Health
      │   └── Audio Manager
      │
      ├── Draw Phase
      │   ├── State Manager
      │   │   └── Current Screen Draw
      │   │       ├── 3D Rendering
      │   │       │   ├── Level
      │   │       │   ├── Entities
      │   │       │   ├── Effects
      │   │       │   └── HUD 3D
      │   │       └── 2D Rendering
      │   │           ├── HUD
      │   │           ├── Crosshair
      │   │           └── Minimap
      │
      └── Event Manager (Clear)
```

## 🗺 Roadmap

### In Development

1. **Animation and Dismemberment**
   - Complete animations for enemy mechs
   - Dismemberment system (if feasible)

2. **Narrative**
   - Game story at the start of each phase
   - Cutscenes and dialogues

3. **Advanced HUD**
   - Visual weapon selection
   - Arrow indicating leg direction
   - Lock-on system improvements (sound and box)

4. **Menu and Settings**
   - Main menu improvements
   - Functional settings system

5. **Weapon System**
   - Implement real range (not just lifetime)
   - Heat system with HUD
   - New weapons:
     - Hitscan Laser
     - Autocannon/Gauss
     - Short Range Missile (SRM)
     - Machine Gun

6. **Visual Improvements**
   - Enhanced textures and models
   - Improved visual effects
   - CIn and Raylib logos on splash screen

## 👥 Authors

Project developed as a final project for the Imperative Programming course (CIN0005/06) at the Informatics Center of UFPE.

<table>
  <tr>
    <td align="center">
      <a href="https://github.com/rafaelvlt">
        <img src="https://github.com/rafaelvlt.png?size=100" width="100" height="100" style="border-radius: 50%; object-fit: cover;" alt=""/>
        <br />
        <sub><b>Rafael Barbosa</b></sub>
      </a>
      <br />
      <a href="https://github.com/rafaelvlt" title="GitHub">@rafaelvlt</a>
    </td>
    <td align="center">
      <a href="https://github.com/RodrigoSilveiraCin">
        <img src="https://github.com/RodrigoSilveiraCin.png?size=100" width="100" height="100" style="border-radius: 50%; object-fit: cover;" alt=""/>
        <br />
        <sub><b>Rodrigo Silveira</b></sub>
      </a>
      <br />
      <a href="https://github.com/RodrigoSilveiraCin" title="GitHub">@RodrigoSilveiraCin</a>
    </td>
    <td align="center">
      <a href="https://github.com/IrineuACgasoso">
        <img src="https://github.com/IrineuACgasoso.png?size=100" width="100" height="100" style="border-radius: 50%; object-fit: cover;" alt=""/>
        <br />
        <sub><b>Caio Amarante</b></sub>
      </a>
      <br />
      <a href="https://github.com/IrineuACgasoso" title="GitHub">@IrineuACgasoso</a>
    </td>
    <td align="center">
      <a href="https://github.com/ARISE21">
        <img src="https://github.com/ARISE21.png?size=100" width="100" height="100" style="border-radius: 50%; object-fit: cover;" alt=""/>
        <br />
        <sub><b>Eric Santiago</b></sub>
      </a>
      <br />
      <a href="https://github.com/ARISE21" title="GitHub">@ARISE21</a>
    </td>
    <td align="center">
      <a href="https://github.com/HebSP">
        <img src="https://github.com/HebSP.png?size=100" width="100" height="100" style="border-radius: 50%; object-fit: cover;" alt=""/>
        <br />
        <sub><b>Heberty Pinto</b></sub>
      </a>
      <br />
      <a href="https://github.com/HebSP" title="GitHub">@HebSP</a>
    </td>
  </tr>
</table>

## 📄 License

This project is licensed under the terms specified in the `LICENSE` file.

---

**Desenvolvido com ❤️ usando Raylib** | **Developed with ❤️ using Raylib**
