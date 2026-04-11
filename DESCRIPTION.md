# Hellborne: Detailed Project Documentation

## 1. Project Overview

Hellborne is a 2D action game prototype built with C++17 and SDL2, prepared as a QNX-focused hackathon submission. The implementation emphasizes responsive controls, stable update timing, and practical deployment readiness for QNX-targeted environments.

This project is currently at prototype stage. It demonstrates a playable combat loop, enemy progression, and persistence support while still documenting clear boundaries for features that are not yet implemented.

## 2. Problem Statement and Motivation

Action games require low input latency and stable frame-to-frame behavior to feel responsive and fair. In embedded or RTOS-adjacent deployments, developers also need repeatable build pipelines and predictable runtime behavior.

Hellborne addresses this by:
- using an input-first update strategy
- using fixed-step gameplay updates in the modular engine scaffold
- using a QNX-aware build configuration
- keeping game systems separated into understandable modules

## 3. Project Goals

Primary goals:
- Build a playable side-scrolling combat prototype.
- Keep player controls responsive under varying frame conditions.
- Maintain a codebase structure that is easy to extend.
- Provide a QNX-compatible build path.

Secondary goals:
- Demonstrate save persistence.
- Support phased progression and enemy scaling.
- Keep audio optional through stub-based fallback behavior.

## 4. Current Scope and Repository Layout

The repository currently contains two code paths:

1. Root executable path
- Built by top-level CMake target.
- Uses main.cpp, game.cpp, and game.h.
- Delivers a playable compact prototype.

2. Modular engine scaffold path
- Located under source_files_copy.
- Includes subsystems for core engine loop, input, entities, world, rendering, UI, audio abstraction, and persistence.
- Includes QNX-specific runtime hints and loop comments for latency-aware behavior.

Important scope note:
The top-level build currently compiles the root executable path. The modular scaffold is present and documented but not yet fully wired into the top-level CMake target.

## 5. Gameplay and Feature Set

Implemented gameplay capabilities include:
- Player movement and directional control.
- Jumping and dashing.
- Melee and ranged attacks.
- Weapon switching between melee, pistol, and shotgun modes.
- Projectile updates and grenade logic.
- Enemy spawning and multi-phase progression.
- Boss encounter state.
- Game-over and victory states.

Quality-of-life and progression features:
- Pause support.
- Restart flow.
- Kill and phase tracking.
- Persistent progression data in binary save form (hellgold, floor/run metrics in modular scaffold).

## 6. System Architecture

### 6.1 Core Loop

The architecture follows a standard game loop pattern:
- Capture input/events.
- Update simulation.
- Render current frame.

In the modular scaffold, this is described and organized as latency-aware, input-priority flow and fixed-step updates.

### 6.2 Input Subsystem

Input supports keyboard and mouse actions, including:
- directional movement
- jump press and release behavior
- dash trigger
- attack trigger

The modular path tracks current and previous key states for edge-triggered actions.

### 6.3 Combat and Entity Subsystems

Combat and entity logic include:
- player attack state windows
- enemy health and contact damage
- enemy spawner orchestration
- damage application and kill counting

This supports a practical gameplay loop suitable for demo and iterative balancing.

### 6.4 Rendering and Camera

Rendering is SDL2-based. The modular path includes:
- camera-follow behavior
- world-space to screen-space offset handling
- HUD overlays
- fallback-style primitive rendering where needed

### 6.5 Persistence

Save persistence uses binary read/write of a SaveData struct in the modular scaffold.

Benefits:
- simple and fast for prototype storage

Trade-offs:
- schema/versioning is not yet formalized
- portability across architecture changes is not guaranteed

### 6.6 Audio Abstraction

Audio design is feature-flag based:
- AudioManager for enabled builds
- AudioStub for environments where mixer support is disabled or unavailable

This prevents audio dependency failures from blocking gameplay execution.

## 7. QNX Integration and RTOS-Relevant Design

QNX-facing implementation elements include:
- dedicated QNX toolchain configuration under qnx/aarch64le
- CMake fallback SDL2 detection for QNX library/include paths
- QNX-specific SDL hints in the modular engine scaffold
- OpenGL ES renderer hint for compatible rendering path selection

RTOS relevance that is currently demonstrated:
- deterministic-minded update structure (fixed-step in scaffold)
- low-latency intent through input-first loop ordering
- deployment awareness through explicit toolchain and target setup

RTOS features not currently implemented:
- explicit IPC channels
- multi-process partitioning
- QNX resource manager integration
- thread-priority orchestration with scheduling policy documentation

## 8. Performance and Stability Strategy

Current strategy:
- Keep update logic bounded and simple.
- Use fixed timestep in modular path to reduce frame-dependent behavior drift.
- Maintain input-first ordering to reduce perceived control lag.
- Continue running when optional systems (such as audio) are unavailable.

Current evidence status:
- architecture-level decisions are present
- formal benchmark reports and latency histograms are not yet included

## 9. Build and Execution

Desktop build flow:
1. Configure with CMake.
2. Build target hellborne.
3. Run produced executable.

QNX build flow:
1. Ensure QNX environment variables are sourced and available.
2. Configure with QNX toolchain file.
3. Build and deploy target executable.

Dependency baseline:
- CMake 3.16+
- C++17 compiler
- SDL2 development libraries

## 10. Code Quality and Maintainability Notes

Strengths:
- clear subsystem separation in modular scaffold
- compile-time audio fallback pattern
- practical naming and manageable class boundaries for a prototype

Areas to improve:
- unify root build and modular scaffold into one canonical target
- add API-level comments for non-obvious logic paths
- increase technical docs around data formats and lifecycle states

### 10.1 Checklist Coverage for Judges

Clean, modular code structure:
- Implemented: subsystem-based scaffold under source_files_copy/src (core, input, entities, graphics, ui, persistence, audio, world).

Proper naming conventions:
- Implemented: descriptive C++ type and method names across scaffold modules.

Code comments and readability:
- Implemented: comments around QNX setup, loop priorities, and input logic.

Version control usage (Git):
- Implemented: active tracked evolution of source and documentation through Git.

Static analysis:
- Implemented: warning flags enabled through CMake in current build configuration.

README and technical documentation quality:
- Implemented: README provides operational onboarding; DESCRIPTION provides deep architectural and quality context.

## 11. Validation and Testing Status

Current validation style:
- manual functional gameplay testing
- compile/build verification on available paths

Static-analysis usage in current workflow:
- compiler warning levels in CMake
- manual review during development

## 12. Known Gaps and Risks

Known gaps:
- no microkernel IPC usage in gameplay architecture
- no multi-process decomposition
- no formal networking or message framing layer
- no automated test suite
- partial divergence between top-level build target and modular scaffold

Risks:
- feature drift between root and scaffold code paths
- persistence schema brittleness if SaveData structure evolves without migration handling

## 13. Roadmap

Recommended next milestones:
1. Make modular engine path the primary top-level build target.
2. Add documentation for subsystem interfaces and state transitions.
3. Introduce save-file versioning and migration support.
4. Add baseline automated tests for input, combat, and persistence.
5. Add profiling and latency instrumentation reports for presentation evidence.
6. Evaluate optional IPC-based component split if stronger RTOS demonstration is required.

## 14. Rubric-Oriented Implementation Highlights

QNX RTOS Architecture and Usage:
- QNX build/deploy setup through a dedicated toolchain path.
- QNX-aware SDL configuration and runtime hints in the modular scaffold.

Real-Time Performance and Stability:
- input-priority loop structure.
- fixed-step gameplay update approach in the scaffold.

Code Quality and Documentation:
- subsystem modularity in source_files_copy/src.
- expanded README and detailed technical description.

Demo and Presentation:
- playable prototype with combat, progression, HUD, and persistence features.

Innovation and Use-Case Relevance:
- QNX-targeted game prototype framing with low-latency gameplay emphasis.


