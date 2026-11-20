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

## The game: Dungeon Escape

**Concept:** player spawns in the room furthest from the exit of a BSP-generated dungeon and must fight their way out.

> All design decisions below came entirely from the user, not from AI suggestions.

### Decisions

| Requirement | Decision |
|---|---|
| Scenario | BSP dungeon (`Dungeon2D`) rasterised into a `TileMap` |
| Player | Arrow/WASD movement; starts with a pistol (1 bullet); can pick up a shotgun |
| Enemy AI | **State machine** — at minimum: Idle → Chase → Attack |
| Projectile | Enemies shoot single bullets; player starts with pistol, can find shotgun (fires 3 bullets in a spread) |
| Item adicional | **Shotgun pickup** scattered in rooms — player switches to it on collect |
| Physics detail | Some rooms have open areas with water tiles (instant death on contact); an AABB zone applies an upward wind force inside them |
| Score (placar) | Lives (lost on hit or falling in water) + points (per enemy killed) |
| Exit condition | Reach the exit tile/room |

### Where the game lives

All game code goes in `source/` alongside the engine classes. The game is **not** a sandbox — it is the final deliverable, built with SFML (same CMake setup as `sandbox/`).

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

SFML 3 is required. Install via `nix profile add nixpkgs#sfml`; headers and libs
land in `~/.nix-profile/include` and `~/.nix-profile/lib` automatically.

```sh
cmake -S sandbox -B sandbox/build \
  -DCMAKE_PREFIX_PATH="$HOME/.nix-profile"
cmake --build sandbox/build
./sandbox/build/tilemap_sandbox
```
