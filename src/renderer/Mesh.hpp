#pragma once
#include "game/Object.hpp"
#include <memory>


class Mesh {
public:
    virtual std::unique_ptr<Mesh> clone() const = 0;

private:
    friend class Renderer;
    virtual uint32_t fill_bufs(Vector2D*& buf) = 0;
};


class Triangle : public Mesh {
public:
    Triangle(Vector2D a, Vector2D b, Vector2D c);
    std::unique_ptr<Mesh> clone() const override;

    uint32_t fill_bufs(Vector2D*& buf) override;

private:
    Vector2D a;
    Vector2D b;
    Vector2D c;
};
