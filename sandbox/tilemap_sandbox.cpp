/**
 * @file tilemap_sandbox.cpp
 * @brief Throwaway SFML viewer to eyeball the Dungeon2D -> TileMap pipeline.
 *
 * This is NOT part of the project proper. It lives on the sfml-tilemap-test
 * branch only, as a visual sanity check: generate a BSP dungeon, rasterize it
 * into a TileMap, drop a player square on a random empty cell, and let a camera
 * (sf::View) follow the player while WASD/arrows move it.
 *
 * Everything the game actually ships (TileMap, Dungeon2D, Global conventions)
 * is reused unchanged; only this file knows about SFML.
 *
 * Movement is discrete: each key press steps the player exactly one tile, and
 * steps into walls (or off the map) are rejected, so the player stays on floor.
 *
 * Controls: WASD / arrow keys to move one tile, Esc or window close to quit.
 */

#include <SFML/Graphics.hpp>

#include "Dungeon2D.hpp"
#include "Global.hpp"
#include "TileMap.hpp"
#include "Vector2D.hpp"

#include <random>

namespace {

// Size of the test dungeon, in tiles.
constexpr int   DUNGEON_W = 60;
constexpr int   DUNGEON_H = 40;

// A thin inset so neighbouring tiles read as a grid rather than one blob.
constexpr float TILE_INSET = 1.0f;

/// Converts our engine Vector2D (pixels, y-down) straight to SFML's.
sf::Vector2f toSf(const Vector2D& v) { return {v.x, v.y}; }

} // namespace

int main() {
    // --- Build the world the game would build -------------------------------
    std::random_device rd;
    const unsigned seed = rd();

    Dungeon2D dungeon(DUNGEON_W, DUNGEON_H);
    dungeon.generate(seed);

    TileMap map;
    map.loadFromDungeon(dungeon);

    // Spawn the player on a random walkable cell. randomEmptyCell only ever
    // returns an EMPTY_TILE, so the player never starts inside a wall. The
    // player's state is the cell it occupies, not a continuous position.
    int playerCol = 0, playerRow = 0;
    map.randomEmptyCell(playerCol, playerRow);

    // --- Window + camera ----------------------------------------------------
    sf::RenderWindow window(sf::VideoMode({1024u, 768u}),
                            "TileMap sandbox (sfml-tilemap-test)");
    window.setVerticalSyncEnabled(true);

    sf::View camera(toSf(map.toWorld(playerCol, playerRow)), {1024.f, 768.f});

    // Reusable shapes drawn every frame.
    sf::RectangleShape tile({TILE_SIZE - 2.f * TILE_INSET,
                             TILE_SIZE - 2.f * TILE_INSET});

    sf::RectangleShape playerShape({TILE_SIZE * 0.7f, TILE_SIZE * 0.7f});
    playerShape.setOrigin({playerShape.getSize().x * 0.5f,
                           playerShape.getSize().y * 0.5f});
    playerShape.setFillColor(sf::Color(220, 60, 60));

    const sf::Color wallColor(40, 44, 52);
    const sf::Color floorColor(90, 98, 110);

    // --- Main loop ----------------------------------------------------------
    while (window.isOpen()) {
        while (const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            } else if (const auto* resized = event->getIf<sf::Event::Resized>()) {
                // Keep a 1:1 pixel mapping: match the view size to the window's
                // new pixel size so resizing widens/narrows the field of view
                // instead of stretching the world.
                camera.setSize({static_cast<float>(resized->size.x),
                                static_cast<float>(resized->size.y)});
            } else if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
                using Key = sf::Keyboard::Key;
                if (key->code == Key::Escape) { window.close(); continue; }

                // Discrete movement: one key press == one single-tile step.
                int stepCol = 0, stepRow = 0;
                switch (key->code) {
                    case Key::A: case Key::Left:  stepCol = -1; break;
                    case Key::D: case Key::Right: stepCol =  1; break;
                    case Key::W: case Key::Up:    stepRow = -1; break;
                    case Key::S: case Key::Down:  stepRow =  1; break;
                    default: break;
                }
                if (stepCol != 0 || stepRow != 0) {
                    const int targetCol = playerCol + stepCol;
                    const int targetRow = playerRow + stepRow;
                    // Reject steps off the map or into a wall; the player only
                    // ever stands on an EMPTY_TILE.
                    const bool walkable =
                        targetCol >= 0 && targetCol < map.width()  &&
                        targetRow >= 0 && targetRow < map.height() &&
                        !map.isWall(targetCol, targetRow);
                    if (walkable) {
                        playerCol = targetCol;
                        playerRow = targetRow;
                    }
                }
            }
        }

        // Camera always focuses on the player's cell.
        const Vector2D playerWorld = map.toWorld(playerCol, playerRow);
        camera.setCenter(toSf(playerWorld));
        window.setView(camera);

        // --- Render ---------------------------------------------------------
        window.clear(sf::Color(20, 22, 26));

        // Only draw the tiles the camera can actually see.
        const sf::Vector2f viewCenter = camera.getCenter();
        const sf::Vector2f viewSize   = camera.getSize();
        int minCol, minRow, maxCol, maxRow;
        map.toIndices({viewCenter.x - viewSize.x * 0.5f,
                       viewCenter.y - viewSize.y * 0.5f}, minCol, minRow);
        map.toIndices({viewCenter.x + viewSize.x * 0.5f,
                       viewCenter.y + viewSize.y * 0.5f}, maxCol, maxRow);
        minCol = std::max(0, minCol);  minRow = std::max(0, minRow);
        maxCol = std::min(map.width() - 1,  maxCol);
        maxRow = std::min(map.height() - 1, maxRow);

        for (int row = minRow; row <= maxRow; ++row) {
            for (int col = minCol; col <= maxCol; ++col) {
                tile.setPosition({col * TILE_SIZE + TILE_INSET,
                                  row * TILE_SIZE + TILE_INSET});
                tile.setFillColor(map.isWall(col, row) ? wallColor : floorColor);
                window.draw(tile);
            }
        }

        playerShape.setPosition(toSf(playerWorld));
        window.draw(playerShape);

        window.display();
    }

    return 0;
}
