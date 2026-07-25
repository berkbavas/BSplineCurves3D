#include "FreeCamera.h"

#include <cmath>

#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QtMath>

const QMatrix4x4& BSplineRenderer::FreeCamera::GetProjectionMatrix()
{
    mProjectionMatrix.setToIdentity();
    mProjectionMatrix.perspective(mVerticalFov, GetAspectRatio(), GetZNear(), GetZFar());
    return mProjectionMatrix;
}

const QMatrix4x4& BSplineRenderer::FreeCamera::GetViewProjectionMatrix()
{
    mViewProjectionMatrix = GetProjectionMatrix() * GetViewMatrix();
    return mViewProjectionMatrix;
}

const QMatrix4x4& BSplineRenderer::FreeCamera::GetViewMatrix()
{
    mViewMatrix.setToIdentity();
    mViewMatrix.rotate(GetRotation().conjugated());
    mViewMatrix.translate(-GetPosition());
    return mViewMatrix;
}

const QMatrix4x4& BSplineRenderer::FreeCamera::GetRotationMatrix()
{
    constexpr QVector4D ZERO_TRANSLATION(0, 0, 0, 1);
    mRotationMatrix = mViewMatrix;
    mRotationMatrix.setColumn(3, ZERO_TRANSLATION);
    return mRotationMatrix;
}

const QVector3D& BSplineRenderer::FreeCamera::GetViewDirection()
{
    constexpr QVector3D NEGATIVE_Z(0, 0, -1);
    mViewDirection = GetRotation() * NEGATIVE_Z;
    return mViewDirection;
}

void BSplineRenderer::FreeCamera::Resize(int Width, int Height)
{
    mWidth = Width;
    mHeight = Height;
}

float BSplineRenderer::FreeCamera::GetHorizontalFov() const
{
    const float HalfVerticalFovRadian = 0.5f * qDegreesToRadians(mVerticalFov);
    const float HalfHorizontalFovRadian = std::atan(std::tan(HalfVerticalFovRadian) * GetAspectRatio());
    return 2.0f * qRadiansToDegrees(HalfHorizontalFovRadian);
}

float BSplineRenderer::FreeCamera::GetAspectRatio() const
{
    return static_cast<float>(mWidth) / static_cast<float>(mHeight);
}

void BSplineRenderer::FreeCamera::Update(float Ifps)
{
    // Rotation
    if (mUpdateRotation)
    {
        RotateGlobal(QVector3D(0, 1, 0), mAngularSpeedMultiplier * mAngularSpeed * mMouse.DX * Ifps);
        RotateLocal(QVector3D(1, 0, 0), mAngularSpeedMultiplier * mAngularSpeed * mMouse.DY * Ifps);
        mMouse.DX = 0.0f;
        mMouse.DY = 0.0f;
        mUpdateRotation = false;
    }

    // Translation
    if (mUpdatePosition)
    {
        if (mPressedKeys[Qt::Key_Control])
        {
            mLinearSpeed = 100.0f;
        }
        else if (mPressedKeys[Qt::Key_Shift])
        {
            mLinearSpeed = 10.0f;
        }
        else
        {
            mLinearSpeed = 1.0f;
        }

        const auto Keys = mPressedKeys.keys();

        for (const auto Key : Keys)
        {
            if (mPressedKeys.value(Key, false))
            {
                const auto Binding = KEY_BINDINGS.value(Key, QVector3D(0, 0, 0));
                const auto Direction = GetRotation().rotatedVector(Binding);
                const auto Delta = mLinearSpeed * mLinearSpeedMultiplier * Ifps * Direction;
                Translate(Delta);
            }
        }
    }

    if (mPressedKeys.empty())
    {
        mUpdatePosition = false;
    }
}

void BSplineRenderer::FreeCamera::Reset()
{
    const auto Keys = mPressedKeys.keys();

    for (const auto Key : Keys)
    {
        mPressedKeys.insert(Key, false);
    }

    mUpdatePosition = false;
    mUpdateRotation = false;
    mMouse.Reset();
}

void BSplineRenderer::FreeCamera::KeyPressed(QKeyEvent* pEvent)
{
    mPressedKeys.insert((Qt::Key) pEvent->key(), true);
    mUpdatePosition = true;
}

void BSplineRenderer::FreeCamera::KeyReleased(QKeyEvent* pEvent)
{
    mPressedKeys.insert((Qt::Key) pEvent->key(), false);
}

void BSplineRenderer::FreeCamera::MousePressed(QMouseEvent* pEvent)
{
    mMouse.X = pEvent->pos().x();
    mMouse.Y = pEvent->pos().y();
    mMouse.Button = pEvent->button();
}

void BSplineRenderer::FreeCamera::MouseReleased(QMouseEvent* pEvent)
{
    if (mMouse.Button == pEvent->button())
    {
        mMouse.Button = Qt::NoButton;
    }
}

void BSplineRenderer::FreeCamera::MouseMoved(QMouseEvent* pEvent)
{
    if (mMouse.Button == mActionReceiveButton)
    {
        mMouse.DX += mMouse.X - pEvent->pos().x();
        mMouse.DY += mMouse.Y - pEvent->pos().y();

        mMouse.X = pEvent->pos().x();
        mMouse.Y = pEvent->pos().y();
        mUpdateRotation = true;
    }
}

QVector3D BSplineRenderer::FreeCamera::GetDirectionFromScreenCoordinates(int X, int Y)
{
    const float HalfVerticalFovRadian = 0.5f * qDegreesToRadians(mVerticalFov);
    const float HalfHorizontalFovRadian = std::atan(std::tan(HalfVerticalFovRadian) * GetAspectRatio());

    const float HorizontalRotationAngleRadian = std::atan(std::tan(HalfHorizontalFovRadian) * (0.5f * mWidth - X) / (0.5f * mWidth));
    const float HorizontalRotationAngle = qRadiansToDegrees(HorizontalRotationAngleRadian);

    const float VerticalRotationAngleRadian = std::atan(((0.5f * mHeight - Y) / (0.5f * mWidth - X)) * std::sin(HorizontalRotationAngleRadian));
    const float VerticalRotationAngle = qRadiansToDegrees(VerticalRotationAngleRadian);

    QQuaternion Left = GetRotation() * QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), HorizontalRotationAngle);
    QQuaternion LeftThenUp = Left * QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), VerticalRotationAngle);

    return LeftThenUp * QVector3D(0, 0, -1);
}

const QMap<Qt::Key, QVector3D> BSplineRenderer::FreeCamera::KEY_BINDINGS = //
    { { Qt::Key_W, QVector3D(0, 0, -1) },                                  //
      { Qt::Key_S, QVector3D(0, 0, 1) },                                   //
      { Qt::Key_A, QVector3D(-1, 0, 0) },                                  //
      { Qt::Key_D, QVector3D(1, 0, 0) },                                   //
      { Qt::Key_E, QVector3D(0, 1, 0) },                                   //
      { Qt::Key_Q, QVector3D(0, -1, 0) } };
