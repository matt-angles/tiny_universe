#pragma once
#include <glm/vec2.hpp>


using Vector2D = glm::ivec2;
using Vector2  = glm::vec2;

class Object {
public:
    Object()
      : pos(0, 0), rot(0.0, 0.0), scale(1.0, 1.0) {}
    Object(int x, int y)
      : pos(x, y), rot(0.0, 0.0), scale(1.0, 1.0) {}
    Object(Vector2D pos, Vector2 rot, Vector2 scale)
      : pos(pos), rot(rot), scale(scale) {}

    Vector2D pos;
    Vector2  rot;
    Vector2  scale;
};
