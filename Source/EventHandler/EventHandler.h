#pragma once

#include "Curve/Spline.h"
#include "Node/Camera/FreeCamera.h"
#include "Core/Mouse.h"

#include <Dense>
#include <QInputEvent>
#include <QObject>

namespace BSplineRenderer
{
    class CurveContainer;
    class RendererManager;

    class EventHandler : public QObject
    {
        Q_OBJECT
      public:
        explicit EventHandler(QObject* pParent);

        void OnKeyPressed(QKeyEvent* pEvent);
        void OnKeyReleased(QKeyEvent* pEvent);
        void OnMousePressed(QMouseEvent* pEvent);
        void OnMouseReleased(QMouseEvent* pEvent);
        void OnMouseMoved(QMouseEvent* pEvent);
        void OnWheelMoved(QWheelEvent* pEvent);

        void SetCamera(FreeCameraPtr pCamera);
        void SetCurveContainer(CurveContainer* pCurveContainer);
        void SetRendererManager(RendererManager* pRendererManager);

        Eigen::Vector3f GetViewDirection() const;

        void SetSelectedCurve(SplinePtr pSpline);
        void SetSelectedKnot(KnotPtr pKnot);
        void SetKnotAround(KnotPtr pKnot);

        void SetDevicePixelRatio(float DevicePixelRatio);

      signals:
        void SelectedKnotChanged(KnotPtr pKnot);
        void SelectedCurveChanged(SplinePtr pSpline);
        void KnotAroundChanged(KnotPtr pKnot);

      private:
        void TrySelectKnot(float X, float Y);
        void TrySelectCurve(float X, float Y);
        void UpdateKnotTranslationPlane();
        Eigen::ParametrizedLine<float, 3> GetRayFromScreenCoordinates(float X, float Y);
        Eigen::Vector3f GetCameraViewDirection() const;
        Eigen::Vector3f GetCameraPosition() const;
        Eigen::Vector3f GetDirectionFromScreenCoordinates(float X, float Y) const;

        FreeCameraPtr mCamera;
        Mouse mMouse;
        SplinePtr mSelectedCurve{ nullptr };
        KnotPtr mSelectedKnot{ nullptr };
        KnotPtr mKnotAround{ nullptr };

        CurveContainer* mCurveContainer;
        RendererManager* mRendererManager;

        float mDevicePixelRatio{ 1.0f };

        Eigen::Hyperplane<float, 3> mKnotTranslationPlane;
    };

}
