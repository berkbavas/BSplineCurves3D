#pragma once

#include "Curve/Spline.h"
#include "Node/Camera/FreeCamera.h"
#include "Node/Light/DirectionalLight.h"
#include "Node/Mesh/Plane.h"
#include "Node/Mesh/Sphere.h"
#include "Node/Model/Model.h"
#include "Node/SkyBox/SkyBox.h"
#include "Renderer/Base/Shader.h"
#include "Renderer/CurveSelectionRenderer.h"
#include "Renderer/SplineRenderer.h"

#include <QMap>
#include <QOpenGLFramebufferObject>
#include <QOpenGLFunctions_4_5_Core>
#include <QVector>

namespace BSplineRenderer
{

    class CurveContainer;
    class SplineRenderer;

    class RendererManager : protected QOpenGLFunctions_4_5_Core
    {
      public:
        RendererManager();

        void Initialize();
        void Resize(int Width, int Height);
        void Render();
        CurveQueryInfo Query(const QPoint& QueryPoint);
        void AddModel(ModelPtr pModel);
        void RemoveModel(ModelPtr pModel);
        FreeCameraPtr GetCamera() const { return mCamera; }
        void SetCurveContainer(CurveContainer* pCurveContainer) { mCurveContainer = pCurveContainer; }
        void SetNumberOfSegments(int NumberOfSegments);
        void SetNumberOfSectors(int NumberOfSectors);
        int GetNumberOfSegments() const;
        int GetNumberOfSectors() const;
        bool* GetWireframe();
        GlowSettings& GetGlowSettings();
        void SetSelectedCurve(SplinePtr pSpline) { mSelectedCurve = pSpline; }
        void SetSelectedKnot(KnotPtr pKnot) { mSelectedKnot = pKnot; }
        void SetKnotAround(KnotPtr pKnot) { mKnotAround = pKnot; }

      private:
        void RenderKnots(SplinePtr pCurve);

        Shader* mModelShader;
        Shader* mSkyBoxShader;
        CurveContainer* mCurveContainer;
        FreeCameraPtr mCamera;
        DirectionalLightPtr mLight;

        QVector<ModelPtr> mModels;

        SkyBoxPtr mSkyBox;
        Model* mSphereModel;

        SplineRenderer* mSplineRenderer;
        CurveSelectionRenderer* mCurveSelectionRenderer;

        SplinePtr mSelectedCurve{ nullptr };
        KnotPtr mSelectedKnot{ nullptr };
        KnotPtr mKnotAround{ nullptr };
    };
}