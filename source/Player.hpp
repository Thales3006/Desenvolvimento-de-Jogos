#ifndef GAME_PLAYER_HPP
#define GAME_PLAYER_HPP

#include <SFML/Graphics.hpp>
#include "Entity.hpp"
#include "TileMap.hpp"
#include "Global.hpp"

struct Player : Entity {
    sf::RectangleShape shape;
    float speed{200.f};

    Player() {
        id = PLAYER_ID;
        collision.halfExtents = {TILE_SIZE / 2.f, TILE_SIZE / 2.f};
        shape.setSize({TILE_SIZE, TILE_SIZE});
        shape.setOrigin({TILE_SIZE / 2.f, TILE_SIZE / 2.f});
        shape.setFillColor(sf::Color::Red);
    }

    void handleInput(float dt, const TileMap& map) {
        Vector2D dir{};
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)  ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))     dir.x -= 1.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))     dir.x += 1.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)    ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))     dir.y -= 1.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)  ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))     dir.y += 1.f;

        if (dir.length_squared() > EPSILON)
            dir.normalize();

        // Separate axes so the player slides along walls instead of stopping.
        Vector2D tryX{body.position.x + dir.x * speed * dt, body.position.y};
        if (!wallAt(tryX, map))
            body.position.x = tryX.x;

        Vector2D tryY{body.position.x, body.position.y + dir.y * speed * dt};
        if (!wallAt(tryY, map))
            body.position.y = tryY.y;

        shape.setPosition({body.position.x, body.position.y});
    }

private:
    bool wallAt(const Vector2D& pos, const TileMap& map) const {
        AABB box = collision.bounds(pos);
        // 1-pixel inset on corners avoids false positives on shared edges.
        auto solid = [&](float wx, float wy) -> bool {
            int col, row;
            map.toIndices({wx, wy}, col, row);
            if (col < 0 || col >= map.width() || row < 0 || row >= map.height())
                return true;
            return map.isWall(col, row);
        };
        return solid(box.min.x + 1.f, box.min.y + 1.f)
            || solid(box.max.x - 1.f, box.min.y + 1.f)
            || solid(box.min.x + 1.f, box.max.y - 1.f)
            || solid(box.max.x - 1.f, box.max.y - 1.f);
    }
};

#endif // GAME_PLAYER_HPP
