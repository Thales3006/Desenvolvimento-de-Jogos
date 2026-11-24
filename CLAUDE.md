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

**Concept:** escape from a Soviet prison (dungeon style). The player spawns in the room furthest from the exit of a BSP-generated dungeon. The exit (a lock) needs a key, which one randomly chosen guard carries, so the player must explore and fight the guards to get out.

> All design decisions below came entirely from the user, not from AI suggestions.

### Decisions

| Requirement | Decision |
|---|---|
| Scenario | BSP dungeon (`Dungeon2D`) rasterised into a `TileMap` |
| Player | Arrow/WASD movement; pistol only (no shotgun); starts with 12 bullets (limited ammo) |
| Enemy (guards) | Same look for all guards. Armed with firearms (single bullets). One random guard carries the key and drops it on death; some guards drop ammo on death |
| Enemy AI | **State machine** — Idle → Chase → Attack, plus Return (go back to spawn and idle). Seeing the player (line of sight through the tile map, so walls block it) triggers Chase. Losing sight starts a short tolerance timer (~0.5 s); when it runs out the guard goes to Return. Seeing the player again at any time goes back to Chase |
| Projectile | Guards and the player shoot single bullets |
| Item adicional | **Key** (held by one guard, opens the exit) and **ammo drops** (from some guards) |
| Physics detail | Some rooms (small chance) have a wind zone: fans on one side push **only the player** horizontally toward the other side, where water tiles are. An AABB zone applies the horizontal wind force inside it |
| Water | Falling into water costs one life (it does not end the game by itself) |
| Score (placar) | Text scoreboard with lives (start with 3; lost on hit or falling in water) + points (for each guard killed and for picking up the key) |
| Lives / restart | Losing one life: player continues in the same place. Losing all lives: the game restarts |
| Exit condition | Reach the exit (lock) tile **holding the key** |

### Where the game lives

All game code goes in `source/` alongside the engine classes. The game is **not** a sandbox — it is the final deliverable, built with SFML using the top-level `Makefile` (no CMake).

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
