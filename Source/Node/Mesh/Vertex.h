#pragma once

#include <QVector2D>
#include <QVector3D>

namespace BSplineRenderer
{
    struct Vertex
    {
        QVector3D Position;
        QVector3D Normal;
        QVector2D TextureCoords;
    };
}
