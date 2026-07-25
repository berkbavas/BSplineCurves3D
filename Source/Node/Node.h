#pragma once

#include "Util/Macros.h"

#include <memory>

#include <QMatrix4x4>
#include <QQuaternion>
#include <QVector3D>
#include <QtMath>

namespace BSplineRenderer
{
    class Node
    {
      protected:
        Node() = default;
        virtual ~Node() = default;

      public:
        const QMatrix4x4& GetTransformation() const { return mTransformation; }
        const QQuaternion& GetRotation() const { return mRotation; }

        const QVector3D& GetPosition() const { return mPosition; }
        const float GetPosition(int Coord) const { return mPosition[Coord]; }
        float* GetPosition(int Coord) { return &mPosition[Coord]; }

        const QVector3D& GetScale() const { return mScale; }
        const float GetScale(int Coord) const { return mScale[Coord]; }
        float* GetScale(int Coord) { return &mScale[Coord]; }

        const QMatrix3x3 GetNormalMatrix() const { return mNormalMatrix; }

        void SetTransformation(const QMatrix4x4& NewTransformation);
        void SetRotation(const QQuaternion& NewRotation);
        void SetPosition(const QVector3D& NewPosition);
        void SetPosition(float X, float Y, float Z);
        void SetScale(const QVector3D& NewScale);
        void SetScale(float X, float Y, float Z);

        virtual void UpdateTransformation();

        void RotateGlobal(const QVector3D& Axis, float Angle);
        void RotateLocal(const QVector3D& Axis, float Angle);
        void Translate(const QVector3D& Delta);

        void MakeDirty() { mDirty = true; }
        void MakeClean() { mDirty = false; }
        bool GetDirty() const { return mDirty; }

      private:
        QMatrix4x4 mTransformation;
        QMatrix3x3 mNormalMatrix;
        QQuaternion mRotation;
        QVector3D mPosition{ QVector3D(0, 0, 0) };
        QVector3D mScale{ QVector3D(1, 1, 1) };

        bool mDirty{ false };
    };

    using NodePtr = std::shared_ptr<Node>;
}
