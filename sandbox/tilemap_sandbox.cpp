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
 * Controls: WASD / arrow keys to move, Esc or window close to quit.
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

// The player moves at this many pixels per second (TILE_SIZE/s * factor).
constexpr float PLAYER_SPEED = 5.0f * TILE_SIZE;

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

    // Spawn the player at the centre of a random walkable cell.
    int spawnCol = 0, spawnRow = 0;
    map.randomEmptyCell(spawnCol, spawnRow);
    Vector2D player = map.toWorld(spawnCol, spawnRow);

    // --- Window + camera ----------------------------------------------------
    sf::RenderWindow window(sf::VideoMode({1024u, 768u}),
                            "TileMap sandbox (sfml-tilemap-test)");
    window.setVerticalSyncEnabled(true);

    sf::View camera(toSf(player), {1024.f, 768.f});

    // Reusable shapes drawn every frame.
    sf::RectangleShape tile({TILE_SIZE - 2.f * TILE_INSET,
                             TILE_SIZE - 2.f * TILE_INSET});

    sf::RectangleShape playerShape({TILE_SIZE * 0.7f, TILE_SIZE * 0.7f});
    playerShape.setOrigin({playerShape.getSize().x * 0.5f,
                           playerShape.getSize().y * 0.5f});
    playerShape.setFillColor(sf::Color(220, 60, 60));

    const sf::Color wallColor(40, 44, 52);
    const sf::Color floorColor(90, 98, 110);

    sf::Clock clock;

    // --- Main loop ----------------------------------------------------------
    while (window.isOpen()) {
        while (const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            } else if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
                if (key->code == sf::Keyboard::Key::Escape) window.close();
            }
        }

        const float dt = clock.restart().asSeconds();

        // Movement: read the keyboard directly, normalize so diagonals aren't
        // faster, then step the world position.
        Vector2D dir;
        using Key = sf::Keyboard::Key;
        if (sf::Keyboard::isKeyPressed(Key::A) || sf::Keyboard::isKeyPressed(Key::Left))  dir.x -= 1.f;
        if (sf::Keyboard::isKeyPressed(Key::D) || sf::Keyboard::isKeyPressed(Key::Right)) dir.x += 1.f;
        if (sf::Keyboard::isKeyPressed(Key::W) || sf::Keyboard::isKeyPressed(Key::Up))    dir.y -= 1.f;
        if (sf::Keyboard::isKeyPressed(Key::S) || sf::Keyboard::isKeyPressed(Key::Down))  dir.y += 1.f;
        if (dir.length_squared() > EPSILON) {
            player += dir.normalized() * (PLAYER_SPEED * dt);
        }

        // Camera always focuses on the player.
        camera.setCenter(toSf(player));
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

        playerShape.setPosition(toSf(player));
        window.draw(playerShape);

        window.display();
    }

    return 0;
}
