#include "Node.h"

void BSplineRenderer::Node::SetRotation(const QQuaternion& NewRotation)
{
    if (mRotation == NewRotation)
    {
        return;
    }

    mRotation = NewRotation;
    Node::UpdateTransformation();
    MakeDirty();
}

void BSplineRenderer::Node::SetPosition(const QVector3D& NewPosition)
{
    if (mPosition == NewPosition)
    {
        return;
    }

    mPosition = NewPosition;
    Node::UpdateTransformation();
    MakeDirty();
}

void BSplineRenderer::Node::SetScale(const QVector3D& NewScale)
{
    if (mScale == NewScale)
    {
        return;
    }

    mScale = NewScale;
    Node::UpdateTransformation();
    MakeDirty();
}

void BSplineRenderer::Node::SetTransformation(const QMatrix4x4& NewTransformation)
{
    if (mTransformation == NewTransformation)
    {
        return;
    }

    mTransformation = NewTransformation;
    mPosition = mTransformation.column(3).toVector3D();
    mRotation = QQuaternion::fromRotationMatrix(mTransformation.normalMatrix());
    MakeDirty();
}

void BSplineRenderer::Node::SetPosition(float X, float Y, float Z)
{
    SetPosition(QVector3D(X, Y, Z));
}

void BSplineRenderer::Node::SetScale(float X, float Y, float Z)
{
    SetScale(QVector3D(X, Y, Z));
}

void BSplineRenderer::Node::UpdateTransformation()
{
    mTransformation.setToIdentity();
    mTransformation.scale(mScale);
    mTransformation.rotate(mRotation);
    mTransformation.setColumn(3, QVector4D(mPosition, 1.0f));

    mNormalMatrix = mTransformation.normalMatrix();
}

void BSplineRenderer::Node::RotateGlobal(const QVector3D& Axis, float Angle)
{
    SetRotation(QQuaternion::fromAxisAndAngle(Axis, Angle) * GetRotation());
}

void BSplineRenderer::Node::RotateLocal(const QVector3D& Axis, float Angle)
{
    SetRotation(GetRotation() * QQuaternion::fromAxisAndAngle(Axis, Angle));
}

void BSplineRenderer::Node::Translate(const QVector3D& Delta)
{
    SetPosition(mPosition + Delta);
}
