#pragma once

#include "EventHandler/EventHandler.h"
#include "Util/Macros.h"

#include <QMouseEvent>
#include <QObject>
#include <QOpenGLExtraFunctions>

namespace BSplineRenderer
{
    class Window;
    class ImGuiWindow;
    class RendererManager;
    class CurveContainer;

    class Controller : public QObject, protected QOpenGLExtraFunctions
    {
        Q_OBJECT
      public:
        explicit Controller(QObject* pParent = nullptr);
        ~Controller();

        void Run();

      private:
        // Core Events
        void Initialize();
        void Resize(int Width, int Height);
        void Render(float Ifps);

        // Input Events
        void OnKeyPressed(QKeyEvent* pEvent);
        void OnKeyReleased(QKeyEvent* pEvent);
        void OnMousePressed(QMouseEvent* pEvent);
        void OnMouseReleased(QMouseEvent* pEvent);
        void OnMouseMoved(QMouseEvent* pEvent);
        void OnWheelMoved(QWheelEvent* pEvent);

        void ApplyCameraPreset(int Preset);

        float mDevicePixelRatio{ 1.0f };
        float mWidth{ 1.0f };
        float mHeight{ 1.0f };

        Window* mWindow;
        ImGuiWindow* mImGuiWindow;

        EventHandler* mEventHandler;
        RendererManager* mRendererManager;
        CurveContainer* mCurveContainer;
        FreeCameraPtr mCamera;
    };
}
