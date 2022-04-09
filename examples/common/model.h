#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "entity.h"

struct Transform {
    Transform(float x, float y) :
        position(glm::vec3(x, y, 0.0f)),
        scale(glm::vec3(1.0f, 1.0f, 1.0f)),
        angle(0.0f) {}

    glm::vec3 position;
    glm::vec3 scale;
    float angle;
};

class Model : public Entity {
public:
    void Create(float x, float y) {
        Entity::Create();
        s_registry.emplace<Transform>(handle_, x, y);
    }
};