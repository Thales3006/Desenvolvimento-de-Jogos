#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>
#include <algorithm>
#include <ctime>
#include "Dungeon2D.hpp"
#include "TileMap.hpp"
#include "Player.hpp"
#include "CollisionManager.hpp"
#include "Global.hpp"

class Game {
    static constexpr unsigned WINDOW_W  = 1280;
    static constexpr unsigned WINDOW_H  = 720;
    static constexpr int      DUNGEON_W = 50;
    static constexpr int      DUNGEON_H = 40;

    sf::RenderWindow m_window;
    Dungeon2D        m_dungeon{DUNGEON_W, DUNGEON_H};
    TileMap          m_tilemap;
    Player           m_player;
    CollisionManager m_collisions;
    sf::VertexArray  m_tileVerts{sf::PrimitiveType::Triangles};
    sf::View         m_camera;

    void buildTileMesh() {
        const std::size_t count =
            static_cast<std::size_t>(m_tilemap.width() * m_tilemap.height() * 6);
        m_tileVerts.resize(count);
        std::size_t idx = 0;

        for (int row = 0; row < m_tilemap.height(); ++row) {
            for (int col = 0; col < m_tilemap.width(); ++col) {
                float x0 = col * TILE_SIZE;
                float y0 = row * TILE_SIZE;
                float x1 = x0 + TILE_SIZE;
                float y1 = y0 + TILE_SIZE;

                sf::Color color;
                switch (m_tilemap.at(col, row)) {
                    case EMPTY_TILE:  color = {200, 185, 160}; break;
                    case EXIT_TILE:   color = {100, 220, 100}; break;
                    case WATER_TILE:  color = { 60, 120, 220}; break;
                    default:          color = { 60,  60,  70}; break; // WALL
                }

                m_tileVerts[idx++] = {{x0, y0}, color};
                m_tileVerts[idx++] = {{x1, y0}, color};
                m_tileVerts[idx++] = {{x0, y1}, color};
                m_tileVerts[idx++] = {{x1, y0}, color};
                m_tileVerts[idx++] = {{x1, y1}, color};
                m_tileVerts[idx++] = {{x0, y1}, color};
            }
        }
    }

    void spawnPlayer() {
        int col, row;
        m_tilemap.randomEmptyCell(col, row);
        Vector2D center = m_tilemap.toWorld(col, row);
        m_player.body.position = center;
        m_player.shape.setPosition({center.x, center.y});
        m_collisions.add(&m_player);
    }

    void handleEvents() {
        while (auto ev = m_window.pollEvent()) {
            if (ev->is<sf::Event::Closed>())
                m_window.close();
            if (auto* k = ev->getIf<sf::Event::KeyPressed>())
                if (k->code == sf::Keyboard::Key::Escape)
                    m_window.close();
        }
    }

    void update(float dt) {
        m_player.handleInput(dt, m_tilemap);
        m_camera.setCenter({m_player.body.position.x, m_player.body.position.y});
    }

    void render() {
        m_window.clear({30, 30, 30});
        m_window.setView(m_camera);
        m_window.draw(m_tileVerts);
        m_window.draw(m_player.shape);
        m_window.setView(m_window.getDefaultView());
        m_window.display();
    }

public:
    Game()
        : m_window(sf::VideoMode({WINDOW_W, WINDOW_H}), "Dungeon Escape")
    {
        m_window.setFramerateLimit(60);
        m_camera.setSize({static_cast<float>(WINDOW_W),
                          static_cast<float>(WINDOW_H)});
        m_dungeon.generate(static_cast<unsigned>(std::time(nullptr)));
        m_tilemap.loadFromDungeon(m_dungeon);
        buildTileMesh();
        spawnPlayer();
    }

    void run() {
        sf::Clock clock;
        while (m_window.isOpen()) {
            const float dt = std::min(clock.restart().asSeconds(), 0.05f);
            handleEvents();
            update(dt);
            render();
        }
    }
};

#endif // GAME_HPP
