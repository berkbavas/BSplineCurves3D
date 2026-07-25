#pragma once

#include "Node/Camera/FreeCamera.h"
#include "Node/Light/DirectionalLight.h"
#include "Node/Mesh/Sphere.h"
#include "Node/Model/Model.h"
#include "Renderer/Base/Shader.h"

#include <QElapsedTimer>
#include <QOpenGLFunctions_4_5_Core>

namespace BSplineRenderer
{
    class CurveContainer;

    // Glow effect settings structure
    struct GlowSettings
    {
        float Intensity{ 1.0f }; // Glow brightness intensity (0.0 - 3.0)
        float Pulse{ 0.0f };     // Pulse animation speed (0.0 = disabled)
        float Shimmer{ 0.0f };   // Shimmer / sparkle effect (0.0 - 1.0)
        float CoreBoost{ 0.0f }; // Center brightness boost (0.0 - 2.0)
    };

    class SplineRenderer : protected QOpenGLFunctions_4_5_Core
    {
      public:
        SplineRenderer() = default;

        void Initialize();
        void Render();
        void SetCurveContainer(CurveContainer* pCurveContainer);
        void SetCamera(FreeCameraPtr pCamera);
        void SetLight(DirectionalLightPtr pLight);

        // Glow settings access
        GlowSettings& GetGlowSettings();
        const GlowSettings& GetGlowSettings() const;

      private:
        CurveContainer* mCurveContainer;
        FreeCameraPtr mCamera;
        DirectionalLightPtr mLight;

        Shader* mSplineShader;

        // Glow effect
        GlowSettings mGlowSettings;
        QElapsedTimer mTimer;

        DEFINE_MEMBER(bool, Wireframe, false);
        DEFINE_MEMBER(int, NumberOfSegments, 32);
        DEFINE_MEMBER(int, NumberOfSectors, 32);
    };
}