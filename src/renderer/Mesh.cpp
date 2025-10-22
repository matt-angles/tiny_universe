#include "Mesh.hpp"


Triangle::Triangle(Vector2D a, Vector2D b, Vector2D c)
  : a(a), b(b), c(c) {}
std::unique_ptr<Mesh> Triangle::clone() const
{
    return std::make_unique<Triangle>(*this);
}

uint32_t Triangle::fill_bufs(Vector2D*& buf)
{
    *buf++ = a; *buf++ = b; *buf++ = c;
    return 3;
}
