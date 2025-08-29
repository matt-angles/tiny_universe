#pragma once
#include "game/Object.hpp"


class Shape {};     // abstract
class Material {};  // abstract

class ViewObject : public Object {
private:
    Shape shape;
    Material material;
};
