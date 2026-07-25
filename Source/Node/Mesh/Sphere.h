#pragma once

#include "Node/Mesh/Mesh.h"

#include <memory>

#include <QOpenGLExtraFunctions>
#include <QVector2D>
#include <QVector3D>
#include <QVector>

namespace BSplineRenderer
{
    class Sphere : public Mesh
    {
      public:
        explicit Sphere(float Radius, int StackCount, int SectorCount);
        ~Sphere();

        void Render() override;

      private:
        struct Vertex
        {
            QVector3D Position;
            QVector3D Normal;
            QVector2D TextureCoords;
        };

        struct TriangleFace
        {
            unsigned int Index0;
            unsigned int Index1;
            unsigned int Index2;
        };

        void CreateGeometry(float Radius, int StackCount, int SectorCount);

        GLuint mVAO{ 0 };
        GLuint mVBO{ 0 };
        GLuint mEBO{ 0 };

        QVector<Vertex> mVertices;
        QVector<TriangleFace> mFaces;

        int mNumberOfFaces{ 0 };
    };

    using SpherePtr = std::unique_ptr<Sphere>;
}