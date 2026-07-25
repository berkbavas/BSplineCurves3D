#pragma once

#include "Node/Mesh/Mesh.h"

namespace BSplineRenderer
{
    class Plane : public Mesh
    {
      public:
        Plane(float Size);
        ~Plane();

        void Render() override;

      private:
        GLuint mVAO{ 0 };
        GLuint mVBO{ 0 };

        QVector<Vertex> mVertices;
    };

    using PlanePtr = std::shared_ptr<Plane>;
}