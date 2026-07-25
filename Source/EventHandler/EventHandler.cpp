#include "EventHandler.h"

#include "Core/CurveContainer.h"
#include "Renderer/RendererManager.h"
#include "Util/Logger.h"

BSplineRenderer::EventHandler::EventHandler(QObject* pParent)
    : QObject(pParent)
{}

void BSplineRenderer::EventHandler::SetCamera(FreeCameraPtr pCamera)
{
    mCamera = pCamera;
}

void BSplineRenderer::EventHandler::SetCurveContainer(CurveContainer* pCurveContainer)
{
    mCurveContainer = pCurveContainer;
}

void BSplineRenderer::EventHandler::SetRendererManager(RendererManager* pRendererManager)
{
    mRendererManager = pRendererManager;
}

void BSplineRenderer::EventHandler::SetDevicePixelRatio(float DevicePixelRatio)
{
    mDevicePixelRatio = DevicePixelRatio;
}

void BSplineRenderer::EventHandler::OnKeyPressed(QKeyEvent* pEvent)
{
    mCamera->KeyPressed(pEvent);
}

void BSplineRenderer::EventHandler::OnKeyReleased(QKeyEvent* pEvent)
{
    mCamera->KeyReleased(pEvent);
}

void BSplineRenderer::EventHandler::OnMousePressed(QMouseEvent* pEvent)
{
    mCamera->MousePressed(pEvent);
    mMouse.Button = pEvent->button();
    mMouse.X = pEvent->x() * mDevicePixelRatio;
    mMouse.Y = pEvent->y() * mDevicePixelRatio;

    if (pEvent->button() == Qt::LeftButton)
    {
        if (mKnotAround)
        {
            TrySelectKnot(mMouse.X, mMouse.Y);
        }
        else
        {
            TrySelectCurve(mMouse.X, mMouse.Y);

            if (mSelectedCurve)
            {
                TrySelectKnot(mMouse.X, mMouse.Y);
            }
        }
    }
    else if (pEvent->button() == Qt::RightButton)
    {
        Eigen::Vector3f Normal = GetCameraViewDirection();
        Eigen::Vector3f Origin = GetCameraPosition();
        Eigen::ParametrizedLine<float, 3> Ray = GetRayFromScreenCoordinates(mMouse.X, mMouse.Y);

        if (mSelectedCurve && mSelectedCurve->GetKnotCount() > 0)
        {
            const auto X = mSelectedCurve->GetKnots().last()->GetPosition().x();
            const auto Y = mSelectedCurve->GetKnots().last()->GetPosition().y();
            const auto Z = mSelectedCurve->GetKnots().last()->GetPosition().z();
            Eigen::Vector3f LastKnotPosition = Eigen::Vector3f(X, Y, Z);

            Eigen::Hyperplane<float, 3> Plane = Eigen::Hyperplane<float, 3>(Normal, -Normal.dot(LastKnotPosition));
            const float t = Ray.intersection(Plane);
            Eigen::Vector3f Intersection = Ray.pointAt(t);

            if (std::isnan(t) == false && std::isinf(t) == false && t > 0)
            {
                const auto pKnot = mSelectedCurve->AddKnot(Intersection.x(), Intersection.y(), Intersection.z());
                SetSelectedKnot(pKnot);
            }
            else
            {
                LOG_DEBUG("EventHandler::OnMousePressed: There is no intersection between ray along the view direction and translation plane.");
            }
        }
        else
        {
            Eigen::Hyperplane<float, 3> Plane = Eigen::Hyperplane<float, 3>(Normal, -Normal.dot(Origin + 20 * Normal));
            const auto Parameter = Ray.intersection(Plane);
            Eigen::Vector3f Intersection = Ray.pointAt(Parameter);

            if (std::isnan(Parameter) == false && std::isinf(Parameter) == false)
            {
                SplinePtr pSpline = std::make_shared<Spline>();
                const auto pKnot = pSpline->AddKnot(Intersection.x(), Intersection.y(), Intersection.z());
                mCurveContainer->AddCurve(pSpline);
                SetSelectedCurve(pSpline);
                SetSelectedKnot(pKnot);
            }
            else
            {
                LOG_DEBUG("EventHandler::OnMousePressed: There is no intersection between ray along the view direction and translation plane.");
            }
        }
    }
}

void BSplineRenderer::EventHandler::OnMouseReleased(QMouseEvent* pEvent)
{
    mCamera->MouseReleased(pEvent);

    mMouse.Button = Qt::NoButton;
}

void BSplineRenderer::EventHandler::OnMouseMoved(QMouseEvent* pEvent)
{
    mCamera->MouseMoved(pEvent);

    mMouse.X = pEvent->x() * mDevicePixelRatio;
    mMouse.Y = pEvent->y() * mDevicePixelRatio;

    if (mMouse.Button == Qt::LeftButton)
    {
        if (mSelectedKnot)
        {
            Eigen::ParametrizedLine<float, 3> Line = GetRayFromScreenCoordinates(mMouse.X, mMouse.Y);
            const auto Parameter = Line.intersection(mKnotTranslationPlane);
            Eigen::Vector3f Intersection = Line.pointAt(Parameter);

            if (std::isnan(Parameter) == false && std::isinf(Parameter) == false)
            {
                mSelectedKnot->SetPosition(Intersection.x(), Intersection.y(), Intersection.z());

                if (mSelectedCurve)
                {
                    mSelectedCurve->MakeDirty();
                }
            }
        }
    }
    else if (mMouse.Button == Qt::NoButton)
    {
        if (mSelectedCurve)
        {
            const auto Direction = mCamera->GetDirectionFromScreenCoordinates(mMouse.X, mMouse.Y);
            const auto Origin = mCamera->GetPosition();
            const auto pKnot = mSelectedCurve->GetClosestKnotToRay(Origin, Direction, 3.0f * mSelectedCurve->GetRadius());
            SetKnotAround(pKnot);
        }
    }
}

void BSplineRenderer::EventHandler::OnWheelMoved(QWheelEvent* pEvent) {}

Eigen::Vector3f BSplineRenderer::EventHandler::GetViewDirection() const
{
    const auto& Direction = mCamera->GetViewDirection();
    return Eigen::Vector3f(Direction.x(), Direction.y(), Direction.z()).normalized();
}

void BSplineRenderer::EventHandler::SetSelectedKnot(KnotPtr pKnot)
{
    if (mSelectedKnot == pKnot)
    {
        return;
    }

    mSelectedKnot = pKnot;
    emit SelectedKnotChanged(mSelectedKnot);
}

void BSplineRenderer::EventHandler::SetKnotAround(KnotPtr pKnot)
{
    if (mKnotAround == pKnot)
    {
        return;
    }

    mKnotAround = pKnot;
    emit KnotAroundChanged(mKnotAround);
}

void BSplineRenderer::EventHandler::SetSelectedCurve(SplinePtr pSpline)
{
    if (mSelectedCurve == pSpline)
    {
        return;
    }

    SetSelectedKnot(nullptr);
    mSelectedCurve = pSpline;
    emit SelectedCurveChanged(mSelectedCurve);
}

void BSplineRenderer::EventHandler::TrySelectKnot(float X, float Y)
{
    KnotPtr pSelectedKnot = nullptr;

    if (mSelectedCurve)
    {
        const auto Direction = mCamera->GetDirectionFromScreenCoordinates(X, Y);
        const auto Origin = mCamera->GetPosition();
        pSelectedKnot = mSelectedCurve->GetClosestKnotToRay(Origin, Direction, 3.0f * mSelectedCurve->GetRadius());
    }

    SetSelectedKnot(pSelectedKnot);
    UpdateKnotTranslationPlane();
}

void BSplineRenderer::EventHandler::TrySelectCurve(float X, float Y)
{
    CurveQueryInfo Info = mRendererManager->Query(QPoint(X, Y));

    if (Info.Result == 1)
    {
        SetSelectedCurve(mCurveContainer->GetCurve(Info.Index));
    }
    else
    {
        SetSelectedCurve(nullptr);
    }
}

void BSplineRenderer::EventHandler::UpdateKnotTranslationPlane()
{
    if (mSelectedKnot)
    {
        const float X = mSelectedKnot->GetPosition().x();
        const float Y = mSelectedKnot->GetPosition().y();
        const float Z = mSelectedKnot->GetPosition().z();
        Eigen::Vector3f KnotPosition = Eigen::Vector3f(X, Y, Z);
        Eigen::Vector3f Normal = GetCameraViewDirection();
        mKnotTranslationPlane = Eigen::Hyperplane<float, 3>(Normal, -Normal.dot(KnotPosition));
    }
}

Eigen::Vector3f BSplineRenderer::EventHandler::GetCameraViewDirection() const
{
    const auto& ViewDirection = mCamera->GetViewDirection();
    return Eigen::Vector3f(ViewDirection[0], ViewDirection[1], ViewDirection[2]);
}

Eigen::Vector3f BSplineRenderer::EventHandler::GetCameraPosition() const
{
    const auto& Position = mCamera->GetPosition();
    return Eigen::Vector3f(Position[0], Position[1], Position[2]);
}

Eigen::Vector3f BSplineRenderer::EventHandler::GetDirectionFromScreenCoordinates(float X, float Y) const
{
    const auto& Direction = mCamera->GetDirectionFromScreenCoordinates(X, Y);
    return Eigen::Vector3f(Direction[0], Direction[1], Direction[2]);
}

Eigen::ParametrizedLine<float, 3> BSplineRenderer::EventHandler::GetRayFromScreenCoordinates(float X, float Y)
{
    Eigen::Vector3f Direction = GetDirectionFromScreenCoordinates(X, Y);
    Eigen::Vector3f Origin = GetCameraPosition();

    return Eigen::ParametrizedLine<float, 3>(Origin, Direction);
}
