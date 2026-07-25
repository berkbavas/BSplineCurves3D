#pragma once

#include "Curve/Knot.h"
#include "Util/Macros.h"

#include <Dense>

#include <QOpenGLExtraFunctions>
#include <QVector>

namespace BSplineRenderer
{
    class Spline : QOpenGLExtraFunctions
    {
      public:
        Spline() = default;

        void AddKnot(KnotPtr pKnot);
        KnotPtr AddKnot(float X, float Y, float Z);
        KnotPtr AddKnot(const QVector3D& Position);

        void RemoveLastKnot();
        void RemoveKnot(KnotPtr pKnot);
        void ClearKnots();

        int GetKnotCount() const;
        float GetTotalLength() const;
        QVector3D GetCentroid() const;
        QPair<QVector3D, QVector3D> GetBoundingBox() const;

        const QVector<KnotPtr>& GetKnots() const;

        KnotPtr GetClosestKnotToRay(const QVector3D& RayOrigin, const QVector3D& RayDirection, float MaxDistance) const;

        void Render();
        void Update();
        void MakeDirty();
        void UpdateIfDirty();
        bool IsDirty() const;

      private:
        void DestroyOpenGLStuff();
        void ContructOpenGLStuff();
        void InitializeOpenGLStuffIfNot();

        Eigen::MatrixXf CreateCoefficientMatrix();
        void UpdateSplineControlPoints();

        QVector<KnotPtr> mKnots;
        QVector<QVector3D> mSplineControlPoints;
        QVector<QVector3D> mBezierControlPoints;
        GLuint mVAO{ 0 };
        GLuint mVBO{ 0 };

        bool mDirty{ false };

        bool mInitialized{ false };

        DEFINE_MEMBER(QVector3D, Color, QVector3D(1.0f, 1.0f, 1.0f));
        DEFINE_MEMBER(float, Ambient, 0.25f);
        DEFINE_MEMBER(float, Diffuse, 0.50f);
        DEFINE_MEMBER(float, Specular, 0.25f);
        DEFINE_MEMBER(float, Shininess, 4.0f);
        DEFINE_MEMBER(float, Radius, 0.25f);

        static constexpr int NUM_OF_PATCH_POINTS = 4;
    };

    using SplinePtr = std::shared_ptr<Spline>;
}