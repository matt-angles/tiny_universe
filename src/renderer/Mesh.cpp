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


Quad::Quad(Vector2D a, Vector2D b, Vector2D c, Vector2D d)
  : a(a), b(b), c(c), d(d) {}
std::unique_ptr<Mesh> Quad::clone() const
{
    return std::make_unique<Quad>(*this);
}

void Quad::fill_bufs(Vector2D* vertBuf,  size_t& iVert, 
                     uint16_t* indexBuf, size_t& iIndex)
{    
    vertBuf[iVert]   = a; vertBuf[iVert+1] = b; 
    vertBuf[iVert+2] = c; vertBuf[iVert+3] = d; 

    indexBuf[iIndex++] = iVert;
    indexBuf[iIndex++] = iVert+1;
    indexBuf[iIndex++] = iVert+2;

    indexBuf[iIndex++] = iVert+1;
    indexBuf[iIndex++] = iVert+2;
    indexBuf[iIndex++] = iVert+3;

    iVert+=4;
}
