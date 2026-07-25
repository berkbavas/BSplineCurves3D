#pragma once

#include "Core/CurveContainer.h"
#include "Node/Camera/FreeCamera.h"
#include "Renderer/Base/CurveSelectionFramebuffer.h"
#include "Renderer/Base/Shader.h"

#include <QOpenGLExtraFunctions>

namespace BSplineRenderer
{
    class CurveSelectionRenderer : protected QOpenGLExtraFunctions
    {
      public:
        CurveSelectionRenderer() = default;

        void Initialize();
        void Render();
        void Resize(int Width, int Height);

        CurveQueryInfo Query(const QPoint& QueryPoint);

        void SetCurveContainer(CurveContainer* pCurveContainer);
        void SetCamera(FreeCameraPtr pCamera);

      private:
        CurveContainer* mCurveContainer;
        Shader* mShader;
        FreeCameraPtr mCamera;
        CurveSelectionFramebufferPtr mFramebuffer{ nullptr };

        DEFINE_MEMBER(int, NumberOfSegments, 32);
        DEFINE_MEMBER(int, NumberOfSectors, 32);
    };
}