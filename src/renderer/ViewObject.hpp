#pragma once
#include "./Mesh.hpp"
#include <memory>


class ViewObject : public Object {
public:
    ViewObject(const Mesh& mesh)
      : mesh(mesh.clone()) {}

    Mesh* get_mesh() const { return mesh.get(); }

private:
    std::unique_ptr<Mesh> mesh;
};
