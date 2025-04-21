#ifndef ARCTICVOX_GAMEOBJECT_HPP
#define ARCTICVOX_GAMEOBJECT_HPP

#include <memory>

#include <glm/vec3.hpp>

#include "arcticvox/components/model.hpp"
#include "arcticvox/components/transform.hpp"

namespace arcticvox::components {
class gameobject {
  public:
    static gameobject make_gameobject() {
        static std::size_t current_id = 0;
        return gameobject(current_id++);
    }

    gameobject(const gameobject& other) = delete;
    gameobject(gameobject&& other) = default;

    gameobject& operator=(const gameobject& other) = delete;
    gameobject& operator=(gameobject&& other) = default;

    std::size_t get_id() const {
        return id_;
    }

    std::shared_ptr<model> model {};
    glm::vec3 colour {};
    transform transform {};

  private:
    gameobject(const std::size_t id) : id_(id) { }

    std::size_t id_;
};

}

#endif
