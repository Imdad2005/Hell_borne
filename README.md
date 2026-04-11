# Hellborne

Hellborne is a 2D SDL2 action game prototype prepared for a QNX hackathon. The project focuses on low-latency input handling, a fixed-timestep update loop, and a QNX-aware build setup rather than networked or multi-process gameplay.

## Project Structure

The repository currently contains two related code paths:

- **Root demo build**: `main.cpp` and `game.cpp` form the default executable defined in `CMakeLists.txt`.
- **Expanded engine scaffold**: `source_files_copy/` contains a more modular version of the game with `GameEngine`, `InputHandler`, `SaveSystem`, `HUDRenderer`, `EnemySpawner`, `Camera`, and audio abstraction.

The default build target currently compiles the root demo build. The `source_files_copy/` tree is useful as technical reference material and for presentation material, but it is not wired into the top-level CMake target yet.

## What the Game Does

The current implementation includes:

- SDL2 window and renderer setup
- keyboard and mouse input handling
- player movement, jumping, dashing, and attacking
- melee, pistol, and shotgun weapon selection
- projectile and grenade logic
- enemy spawning and phase progression
- boss state and victory/game-over screens
- binary save/load support through `SaveSystem`
- HUD and overlay rendering in the modular engine version

## QNX-Focused Design Notes

The modular engine scaffold includes QNX-specific build and runtime choices:

- a QNX toolchain file in `qnx/aarch64le/cmake-toolchain-qnx-aarch64le.cmake`
- QNX fallback logic in `CMakeLists.txt` for SDL2 discovery
- SDL hints that prefer OpenGL ES and QNX hardware-acceleration settings
- an input-first loop order to reduce input-to-pixel latency
- a fixed-timestep update path for stable gameplay timing

## Build Requirements

- CMake 3.16 or newer
- a C++17 compiler
- SDL2 development files
- On QNX: QNX SDP environment variables must be set before configuring

## Build

### Default desktop build

```bash
cmake -S . -B build
cmake --build build
```

### QNX build

Use the QNX toolchain file from `qnx/aarch64le/cmake-toolchain-qnx-aarch64le.cmake` and make sure `QNX_HOST` and `QNX_TARGET` are available in the environment before configuring.

## Run

The executable is named `hellborne`.

## Controls

- Move: `A` / `D` or arrow keys
- Jump: `Space`, `W`, or `Up`
- Dash: `Left Shift`
- Attack: `J` or left mouse button
- Switch weapon: `1`, `2`, `3`, or `Q`
- Pause: `P`
- Restart / quit on end screens: `R`, `Q`, or `Escape`

## Code Quality Checklist Mapping

This section maps directly to common hackathon judging points.

### 1) Clean, modular code structure

Implemented in this project:
- The modular scaffold separates concerns into `core`, `input`, `entities`, `graphics`, `ui`, `persistence`, `audio`, and `world` under `source_files_copy/src`.

### 2) Proper naming conventions

Implemented in this project:
- Class and method names in the scaffold follow clear, descriptive C++ naming (for example `GameEngine`, `InputHandler`, `EnemySpawner`, `SaveSystem`).

### 3) Code comments and readability

Implemented in this project:
- Non-obvious engine behavior and QNX/runtime decisions are commented in core loop and initialization areas.
- Input handling logic includes comments for state transitions and edge-trigger behavior.

### 4) Version control usage (Git)

Implemented in this project:
- The project is maintained in Git with tracked updates to technical documents and source files.

### 5) Static analysis

Implemented in this project:
- Compiler warnings are enabled in the top-level CMake file (`/W3` on MSVC, `-Wall -Wextra -Wpedantic` otherwise).

### 6) README and technical documentation quality

Implemented in this project:
- `README.md` provides build/run instructions, controls, scope boundaries, and QNX notes.
- `DESCRIPTION.md` provides detailed architecture, quality, validation, risks, and rubric-oriented documentation.

## Git Workflow Notes (Current)

- Development currently follows direct Git tracking with incremental commits.

## Static Analysis Notes (Current)

Current baseline quality checks:
- compile with warning flags enabled via CMake
- manual review and runtime smoke testing

## Documentation Status

What is supported in the codebase today:

- QNX-aware build configuration
- low-latency input-first loop design
- modular gameplay and UI scaffolding in `source_files_copy/`
- binary save/load implementation

## Presentation Guidance

For judges, the strongest honest framing is:

"Hellborne is a QNX-targeted SDL2 game prototype that emphasizes deterministic update flow, low-latency input handling, and clean subsystem separation. The project is configured for QNX builds and includes modular scaffolding for input, audio, persistence, rendering, and UI, but it does not claim a multi-process or IPC-heavy RTOS design."

That framing is accurate to the code and avoids overstating the current implementation.