#pragma once

#include "Node/Mesh/Vertex.h"
#include "Util/Macros.h"

#include <memory>

#include <QOpenGLExtraFunctions>

namespace BSplineRenderer
{
    class Mesh : protected QOpenGLExtraFunctions
    {
      public:
        Mesh() = default;
        virtual ~Mesh() = default;

        virtual void Render() = 0;
    };

    using MeshPtr = std::shared_ptr<Mesh>;
    using MeshWeakPtr = std::weak_ptr<Mesh>;
}