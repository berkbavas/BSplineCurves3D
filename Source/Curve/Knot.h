#pragma once

#include <memory>

#include <QVector3D>

namespace BSplineRenderer
{
    class Knot
    {
      public:
        Knot(float X, float Y, float Z);
        Knot(const QVector3D& Position);

        void SetPosition(const QVector3D& Position);
        void SetPosition(float X, float Y, float Z);

        const QVector3D& GetPosition() const;

      private:
        QVector3D mPosition;
    };

    using KnotPtr = std::shared_ptr<Knot>;
}
