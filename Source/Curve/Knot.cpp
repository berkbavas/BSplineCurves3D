#include "Knot.h"

BSplineRenderer::Knot::Knot(float X, float Y, float Z)
    : mPosition(X, Y, Z)
{}

BSplineRenderer::Knot::Knot(const QVector3D& Position)
    : mPosition(Position)
{}

void BSplineRenderer::Knot::SetPosition(const QVector3D& Position)
{
    mPosition = Position;
}

void BSplineRenderer::Knot::SetPosition(float X, float Y, float Z)
{
    mPosition.setX(X);
    mPosition.setY(Y);
    mPosition.setZ(Z);
}

const QVector3D& BSplineRenderer::Knot::GetPosition() const
{
    return mPosition;
}
