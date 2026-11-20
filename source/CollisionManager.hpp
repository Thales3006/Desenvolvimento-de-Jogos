#ifndef GAME_COLLISION_MANAGER_HPP
#define GAME_COLLISION_MANAGER_HPP

#include <vector>
#include <utility>
#include <algorithm>
#include "Entity.hpp"
#include "Global.hpp"

class CollisionManager {
    std::vector<Entity*> m_entities;
    EntityId m_nextId{INITIAL_ENTITY_ID};

public:
    void add(Entity* e) {
        if (e->id == 0)
            e->id = m_nextId++;
        m_entities.push_back(e);
    }

    void remove(EntityId id) {
        m_entities.erase(
            std::remove_if(m_entities.begin(), m_entities.end(),
                [id](Entity* e){ return e->id == id; }),
            m_entities.end()
        );
    }

    [[nodiscard]]
    std::vector<std::pair<Entity*, Entity*>> collisions() const {
        std::vector<std::pair<Entity*, Entity*>> result;
        for (std::size_t i = 0; i < m_entities.size(); ++i) {
            if (!m_entities[i]->active) continue;
            for (std::size_t j = i + 1; j < m_entities.size(); ++j) {
                if (!m_entities[j]->active) continue;
                if (m_entities[i]->bounds().intersects(m_entities[j]->bounds()))
                    result.emplace_back(m_entities[i], m_entities[j]);
            }
        }
        return result;
    }

    [[nodiscard]]
    Entity* find(EntityId id) const {
        for (Entity* e : m_entities)
            if (e->id == id) return e;
        return nullptr;
    }

    [[nodiscard]]
    const std::vector<Entity*>& entities() const noexcept {
        return m_entities;
    }
};

#endif // GAME_COLLISION_MANAGER_HPP
