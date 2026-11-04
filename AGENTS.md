# AGENTS.md

This file provides guidance to agents when working with code in this repository.

## Project Overview

ntcap-cpp is a GUI application for running cellular automata simulation. The main features are fast processing, easy experimenting.

Supported platforms: **Linux**, **Windows**.

## Tech Stack

- **Language:** C++20
- **UI framework:** Qt 6
- **Build system:** CMake ≥ 3.20.
- **UI is written entirely in C++.** There are **no `.ui` files**, and
  `CMAKE_AUTOUIC` / `CMAKE_AUTORCC` are **not** enabled. Do not introduce
  `.ui` files or turn those flags on. `CMAKE_AUTOMOC` is on — `Q_OBJECT`
  classes are handled automatically.
- **Qt version is not pinned** in `CMakeLists.txt`. Do not hardcode a specific
  Qt 6.x. Resolve via `find_package(Qt6 REQUIRED COMPONENTS ...)` and ask for
  only the modules you actually need.

## Architecture

Source tree by responsibility:

- `core/` — entry point (`main.cpp`), `MainWindow`, the `Automaton` model,
  `AutomatonController`, and `Planes` (the multi-plane cell state).
- `recipes/infra/` — reusable automaton infrastructure: topologies
  (`TorusTopology`), neighborhoods (`MooreN`, `VonNeumannN`), the recipe
  registry (`RecipeRegistry`), the preset registry
  (`RecipePresetRegistry`), shared parameter widgets and the preset dialog.
- `recipes/impl/` — concrete automaton rules: `Life`, `LifeHistory`, `Parity`,
  `TubeWorms`, `StableTubeWorms`, `TimeTunnel`, `CyclicRank`, `BrianBrain`,
  `Histogram`, `Scarves`. Each is a self-contained recipe plus optional
  parameter widget.
- `rendering/` — rendering strategies (`SinglePlaneStrategy`,
  `BitfieldStrategy`, `SaturatingSumStrategy`, `NormalizedSumStrategy`),
  post-processors (`TrailPostProcessor`, `AntiflickerPostProcessor`,
  `GrayscalePostProcessor`), `CellularCanvas`, and color maps in
  `rendering/color_map/`.
- `converters/` — `Planes` ⇄ file formats: `CsvPlanesConverter`,
  `RlePlanesConverter`, `LifeRlePlanesConverter`, `PlaintextPlanesConverter`,
  plus bidirectional `PlanesCsvConverter` / `PlanesRleConverter`.
- `shell/` — user-facing actions, dialogs, and input handling: save/load
  actions, `SimulationActions`, `RecipeActions`, keyboard/mouse controllers,
  status bar manager, init dialogs, color-map actions, `HelpActions` (the Help
  menu), `AboutDialog`, and `KeyMappingsData` / `KeyMappingsDialog` (the Key
  Mappings reference).

Architectural patterns to preserve when adding or modifying code:

- **Strategy** for rendering — new renderers implement the existing strategy
  interface; do not add `if`/`switch` branches inside `CellularCanvas`.
- **Registry** for recipes and presets — new automata register through
  `RecipeRegistry` / `RecipePresetRegistry`; do not hardcode rule names in
  `MainWindow` or the shell layer.
- **Post-processors** are chained; visual effects belong in a post-processor,
  not in individual rendering strategies.
- **Converters** keep `Planes` format-agnostic. New file formats go through
  the converter layer.
- **Key bindings are documented in one place** — `shell/KeyMappingsData.cpp` is
  the single source of truth behind Help → Key Mappings. Every keyboard or
  mouse binding that is added, changed or removed in `KeyboardController`,
  `MouseController` or `SimulationActions` must be reflected there in the same
  change.

When adding a new source file, **add it explicitly** to the
`add_executable(ntcap-gui ...)` list (or the matching test target in
`tests/unit/CMakeLists.txt`). There is no globbing — sources are listed by hand.

## Build & Run

For the tokens economy use `tail -n30` command on build commands output.

### Windows

We use MinGW, because it supports coverage.

cmake -S . -B build -G "MinGW Makefiles"
cmake --build build --parallel
windeployqt build/ntcap-gui.exe

### Linux

cmake -S . -B build
cmake --build build --parallel

## Tests

- Unit tests live under `tests/unit/` and are executed in a build command.

- When adding tests, list them explicitly in `tests/unit/CMakeLists.txt`,
  mirroring the explicit listing style used for `ntcap-gui`.

## Platform Support

- Code must build and run on both **Linux** and **Windows**.
- Prefer Qt abstractions (`QDir`, `QFileInfo`, `QStandardPaths`, `QFile`) over
  raw OS calls or `std::filesystem` for paths, so both platforms keep working.
- Any genuinely platform-specific code must be guarded with `#ifdef Q_OS_WIN`
  / `#ifdef Q_OS_LINUX` and keep both branches functional.
- Do not hardcode path separators (`/` or `\`) in code or tests — compose
  paths with `QDir` / `QFileInfo`.
- Do not mass-convert line endings or encodings in existing files.

## Guidance for AI Agents

- **You may** modify or add files in `core/`, `recipes/`, `rendering/`,
  `converters/`, `shell/`, `tests/`, and `CMakeLists.txt` /
  `tests/CMakeLists.txt` as needed to complete the task.
- **Register new sources.** Every new `.cpp` must be added explicitly to the
  corresponding target. No globbing.
- **Keep the key-mapping reference in sync.** Any change to a keyboard or mouse
  binding must update `shell/KeyMappingsData.cpp` in the same change and keep
  `tests/unit/key_mappings.cpp` passing. The Help → Key Mappings window renders
  that table, so a stale or missing entry is a bug.
- **Extend, don't bypass, the architecture.** New automata go through
  `RecipeRegistry`; new renderers implement the strategy interface; new file
  formats go through a converter. Do not add cross-layer shortcuts.
- **Keep `Planes` format- and render-agnostic.** It is the model; everything
  else composes on top of it.
- **Do not change** the C++ standard, the default values of `BUILD_TESTS` /
  `ENABLE_COVERAGE`, or the `Qt6` module list (`Core`, `Widgets`) without an
  explicit reason and manual user approve.
- **Do not introduce** `CMAKE_AUTOUIC`, `CMAKE_AUTORCC`, `.ui` files, a
  formatter/linter config, pre-commit hooks, or CI configuration unless the
  task explicitly asks for it.
