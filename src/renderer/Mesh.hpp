#pragma once
#include "game/Object.hpp"
#include <memory>


class Mesh {
public:
    virtual std::unique_ptr<Mesh> clone() const = 0;

private:
    friend class Renderer;
    virtual void fill_bufs(Vector2D* vertBuf,  size_t& iVert, 
                           uint16_t* indexBuf, size_t& iIndex) = 0;
};


class Triangle : public Mesh {
public:
    Triangle(Vector2D a, Vector2D b, Vector2D c);
    std::unique_ptr<Mesh> clone() const override;

    void fill_bufs(Vector2D* vertBuf,  size_t& iVert, 
                   uint16_t* indexBuf, size_t& iIndex) override;

private:
    Vector2D a;
    Vector2D b;
    Vector2D c;
};


class Quad : public Mesh {
public:
    Quad(Vector2D a, Vector2D b, Vector2D c, Vector2D d);
    std::unique_ptr<Mesh> clone() const override;

    void fill_bufs(Vector2D* vertBuf,  size_t& iVert, 
                   uint16_t* indexBuf, size_t& iIndex) override;

private:
    Vector2D a;
    Vector2D b;
    Vector2D c;
    Vector2D d;
};
