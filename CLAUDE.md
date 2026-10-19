# CLAUDE.md

## Repository layout — what each folder is for

- **`source/`** — the implementation of the project's classes (the engine:
  `Vector2D`, `Transform2D`, `RigidBody2D`, `Collision2D`, `TileMap`,
  `Dungeon2D`, `Global`, ...). AI assistance is allowed for writing this code.

- **`doc/`** — the students' own documentation of the classes implemented in
  `source/`. One `.md` per class. **These explanations must be written by the
  students themselves**, not generated — the teacher allows AI for *coding*,
  but the `doc/` explanations of the code are the students' work. Do not
  auto-generate or rewrite files in `doc/`.

## Commit conventions

- Use **Conventional Commits**, and keep the message to a **single line**. If a
  commit needs more than one line to explain it, the commit is too large — split
  it into smaller commits.
- For any change to `source/` or `doc/`, the scope is the homework it belongs
  to: `hw<N>`, where `hw` abbreviates *homework* and `<N>` is the homework
  number (`hw1`, `hw2`, `hw3`, ...). For example `feat(hw1): ...`,
  `docs(hw3): ...`.

## Branch: `sfml-tilemap-test`

**This branch exists only to visually test the labyrinth/dungeon (tilemap)
rendering with SFML.** Keep it isolated — do not merge SFML or the sandbox into
`main`, and do not add SFML dependencies to `source/`. The engine stays
rendering-agnostic; only `sandbox/` knows about SFML.

### What it does

`sandbox/tilemap_sandbox.cpp` opens a small window that:
- generates a BSP dungeon (`Dungeon2D`) and rasterizes it into a `TileMap`;
- draws the tile world (dark = `WALL_TILE`, light = `EMPTY_TILE`);
- places a red square (the player) on a random empty cell;
- lets WASD / arrow keys move the player, with a camera (`sf::View`) that
  always stays centred on it. Esc or the close button quits.

The sandbox reuses the real engine files (`TileMap`, `Dungeon2D`, `Vector2D`)
unchanged, so what it shows is exactly the shipped pipeline.

### Build & run

SFML 3 is required. The reproducible way is the flake in `sandbox/`, which
provides SFML 3, CMake and a C++ toolchain:

```sh
nix develop ./sandbox            # enter the dev shell (SFML/CMake on PATH)
cmake -S sandbox -B sandbox/build
cmake --build sandbox/build
./sandbox/build/tilemap_sandbox
```

Run from the repo root so the sandbox can reach `../source`. Without nix, any
SFML 3 install found by `find_package(SFML 3 ...)` works; if CMake can't locate
it, point it there with `export CMAKE_PREFIX_PATH=<sfml-prefix>:$CMAKE_PREFIX_PATH`.
