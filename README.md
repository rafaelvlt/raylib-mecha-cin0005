# MECHA GAME

[🇧🇷 Português](#português) | [🇺🇸 English](#english)

---

# Português

Jogo de ação em primeira pessoa desenvolvido em C com Raylib, focado em combate entre mechas. Projeto final para a disciplina de Programação Imperativa (CIN0005/06).

> **Inspirado na franquia MechWarrior**

![Mecha Game](https://img.shields.io/badge/Status-Em%20Desenvolvimento-yellow)
![C](https://img.shields.io/badge/Language-C-blue)
![Raylib](https://img.shields.io/badge/Framework-Raylib-red)

## 📋 Índice

- [Sobre](#sobre)
- [Tecnologias](#tecnologias)
- [Requisitos](#requisitos)
- [Instalação](#instalação)
- [Compilação](#compilação)
- [Como Jogar](#como-jogar)
- [Estrutura do Projeto](#estrutura-do-projeto)
- [Arquitetura](#arquitetura)
- [Roadmap](#roadmap)
- [Autores](#autores)
- [Licença](#licença)

## 🎮 Sobre

MECHA GAME é um jogo de combate tático em primeira pessoa onde você pilota um mecha em batalhas contra inimigos controlados por IA. O jogo apresenta um sistema de combate baseado em armas com diferentes características, sistema de mira e lock-on, animações fluidas, e uma arquitetura ECS (Entity-Component-System) para gerenciamento eficiente de entidades.

Este projeto foi desenvolvido como trabalho final para a disciplina de Programação Imperativa (CIN0005/06) do Centro de Informática da UFPE.

## 🛠 Tecnologias

### Desenvolvimento
- **C (C99)**: Linguagem principal
- **Raylib 5.6-dev**: Framework gráfico e de áudio
- **Raymath**: Biblioteca matemática para operações 3D
- **Make**: Sistema de build

### Assets
- **Blender**: Modelagem 3D e animações dos mechas
- **Audacity**: Edição de áudio e efeitos sonoros

## 📦 Requisitos

### Linux

```bash
# Dependências básicas
sudo apt-get update
sudo apt-get install build-essential libgl1-mesa-dev libx11-dev libxrandr-dev libxi-dev libxcursor-dev libxinerama-dev libasound2-dev
```

### Windows

- MinGW-w64 ou Visual Studio
- OpenGL 3.3+
- Bibliotecas de áudio do Windows

### macOS

```bash
# Usando Homebrew
brew install raylib
```

## 🚀 Instalação

1. Clone o repositório:
```bash
git clone https://github.com/rafaelvlt/raylib-mecha-cin0005.git
cd raylib-mecha-cin0005
```

2. Compile o Raylib primeiro:
```bash
cd lib/raylib/src
make PLATFORM=PLATFORM_DESKTOP
cd ../../..
```

3. Compile e execute o jogo:
```bash
make run
```

## 🔨 Compilação

### Compilar o projeto

```bash
make
```

### Compilar e executar

```bash
make run
```

O executável será gerado como `mechaGame` (Linux/macOS) ou `mechaGame.exe` (Windows).

## 🎯 Como Jogar

### Controles

#### Movimento
- **W / Seta para Cima**: Mover para frente/trás
- **A / D ou Setas Esquerda/Direita**: Girar o mecha
- **Mouse**: Rotacionar torso e mira (independente das pernas)

#### Combate
- **Botão Esquerdo do Mouse**: Atirar
- **Botão Direito do Mouse**: Zoom
- **1, 2, 3, 4, 5**: Alternar grupos de armas
- **TAB**: Lock-on (travar alvo)

#### Utilidades
- **C**: Centralizar torso nas pernas
- **F**: Centralizar pernas no torso


## 📁 Estrutura do Projeto

```
raylib-mecha-cin0005/
├── include/              # Headers
│   ├── ecs/            # Entity-Component-System
│   │   ├── components.h
│   │   ├── entitymanager.h
│   │   ├── systems.h
│   │   └── types.h
│   ├── screens/        # Telas do jogo
│   ├── systems/        # Sistemas principais
│   └── utility.h
├── src/                # Código-fonte
│   ├── ecs/           # Sistemas ECS
│   │   ├── ai_systems.c
│   │   ├── collision_system.c
│   │   ├── generic_systems.c
│   │   ├── health_system.c
│   │   ├── hud_system.c
│   │   ├── player_systems.c
│   │   ├── projectile_system.c
│   │   └── weapon_system.c
│   ├── screens/       # Implementação das telas
│   └── systems/       # Gerenciadores
│       ├── audio_manager.c
│       ├── config_manager.c
│       ├── event_manager.c
│       ├── map_loader.c
│       ├── resource_manager.c
│       └── state_manager.c
├── resources/         # Assets do jogo
│   ├── fonts/        # Fontes
│   ├── maps/         # Arquivos de mapa (.map)
│   ├── models/       # Modelos 3D (.glb)
│   ├── musics/       # Músicas de fundo
│   ├── sounds/       # Efeitos sonoros
│   └── textures/     # Texturas
├── lib/              # Bibliotecas externas
│   └── raylib/      # Raylib framework
├── Makefile         # Sistema de build
├── config.ini       # Configurações do jogo
└── README.md        # Este arquivo
```

## 🏗 Arquitetura

O projeto utiliza uma arquitetura **ECS (Entity-Component-System)** para gerenciar entidades de forma eficiente e modular.

### Componentes Principais

- **TransformComponent**: Posição, rotação e escala
- **PhysicsComponent**: Velocidade e aceleração
- **RenderComponent**: Modelo 3D e materiais
- **AnimationComponent**: Estado de animação
- **HealthComponent**: Vida e reações a dano
- **WeaponComponent**: Estatísticas de armas
- **AIControlComponent**: Comportamento de IA
- **CollisionComponent**: Caixas de colisão

### Sistemas Principais

- **PlayerControlSystem**: Controle do jogador
- **AIControlSystem**: IA dos inimigos
- **WeaponSystem**: Lógica de disparo
- **CollisionSystem**: Detecção de colisões
- **AnimationSystem**: Atualização de animações
- **RenderSystem**: Renderização 3D
- **HealthSystem**: Processamento de dano

### Fluxo de Execução

```
main
  │
  ├── Inicialização
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

### Em Desenvolvimento

1. **Animação e Desmembramento**
   - Animações completas para mechas inimigos
   - Sistema de desmembramento (se viável)

2. **Narrativa**
   - História do jogo no início de cada fase
   - Cutscenes e diálogos

3. **HUD Avançado**
   - Seleção visual de armas
   - Seta indicando direção das pernas
   - Melhorias no sistema de lock-on (som e caixa)

4. **Menu e Configurações**
   - Melhorias no menu principal
   - Sistema de configurações funcional

5. **Sistema de Armas**
   - Implementar range real (não apenas lifetime)
   - Sistema de aquecimento com HUD
   - Novas armas:
     - Laser hitscan
     - Autocannon/Gauss
     - Short Range Missile (SRM)
     - Machine Gun

6. **Melhorias Visuais**
   - Texturas e modelos aprimorados
   - Efeitos visuais melhorados
   - Logos do CIn e Raylib na tela inicial

## 👥 Autores

Projeto desenvolvido como trabalho final para a disciplina de Programação Imperativa (CIN0005/06) do Centro de Informática da UFPE.

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

## 📄 Licença

Este projeto está sob a licença especificada no arquivo `LICENSE`.

---

# English

First-person action game developed in C with Raylib, focused on mech combat. Final project for the Imperative Programming course (CIN0005/06).

> **Inspired by the MechWarrior franchise**

![Mecha Game](https://img.shields.io/badge/Status-In%20Development-yellow)
![C](https://img.shields.io/badge/Language-C-blue)
![Raylib](https://img.shields.io/badge/Framework-Raylib-red)

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

MECHA GAME is a tactical first-person combat game where you pilot a mech in battles against AI-controlled enemies. The game features a combat system based on weapons with different characteristics, aiming and lock-on system, fluid animations, and an ECS (Entity-Component-System) architecture for efficient entity management.

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

2. Compile Raylib first:
```bash
cd lib/raylib/src
make PLATFORM=PLATFORM_DESKTOP
cd ../../..
```

3. Compile and run the game:
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

The executable will be generated as `mechaGame` (Linux/macOS) or `mechaGame.exe` (Windows).

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
