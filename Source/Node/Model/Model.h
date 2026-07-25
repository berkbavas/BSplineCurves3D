#pragma once

#include "Node/Mesh/Mesh.h"
#include "Node/Node.h"

namespace BSplineRenderer
{
    class Model : public Node
    {
      public:
        Model(MeshPtr pMesh);

        MeshPtr GetMesh() const;

      private:
        MeshPtr mMesh;

        DEFINE_MEMBER(QVector3D, Color, QVector3D(1.0f, 1.0f, 1.0f));
        DEFINE_MEMBER(float, Ambient, 0.10f);
        DEFINE_MEMBER(float, Diffuse, 0.50f);
        DEFINE_MEMBER(float, Specular, 0.25f);
        DEFINE_MEMBER(float, Shininess, 4.0f);
    };

    using ModelPtr = std::shared_ptr<Model>;
}