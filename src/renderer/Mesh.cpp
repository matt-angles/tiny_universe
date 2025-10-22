#include "Mesh.hpp"


Triangle::Triangle(Vector2D a, Vector2D b, Vector2D c)
  : a(a), b(b), c(c) {}
std::unique_ptr<Mesh> Triangle::clone() const
{
    return std::make_unique<Triangle>(*this);
}

void Triangle::fill_bufs(Vector2D* vertBuf,  size_t& iVert, 
                         uint16_t* indexBuf, size_t& iIndex)
{
    
    indexBuf[iIndex++] = iVert; vertBuf[iVert++] = a;
    indexBuf[iIndex++] = iVert; vertBuf[iVert++] = b;
    indexBuf[iIndex++] = iVert; vertBuf[iVert++] = c;
}
