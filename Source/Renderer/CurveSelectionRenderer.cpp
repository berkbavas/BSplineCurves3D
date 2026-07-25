#include "CurveSelectionRenderer.h"

void BSplineRenderer::CurveSelectionRenderer::Initialize()
{
    initializeOpenGLFunctions();

    mShader = new Shader("Curve Selection Shader");
    mShader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Spline.vert");
    mShader->AddPath(QOpenGLShader::TessellationControl, ":/Resources/Shaders/Spline.tcs");
    mShader->AddPath(QOpenGLShader::TessellationEvaluation, ":/Resources/Shaders/Spline.tes");
    mShader->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/CurveSelection.frag");
    mShader->Initialize();
}

void BSplineRenderer::CurveSelectionRenderer::Render()
{
    mFramebuffer->Clear();
    mFramebuffer->Bind();

    mShader->Bind();
    mShader->SetUniformValue("uNumberOfSegments", mNumberOfSegments);
    mShader->SetUniformValue("uNumberOfSectors", mNumberOfSectors);
    mShader->SetUniformValue("uVP", mCamera->GetViewProjectionMatrix());

    const auto& Curves = mCurveContainer->GetCurves();

    for (int Index = 0; Index < Curves.size(); ++Index)
    {
        const auto& pCurve = Curves[Index];
        mShader->SetUniformValue("uCurveIndex", Index);
        mShader->SetUniformValue("uRadius", pCurve->GetRadius());
        pCurve->Render();
    }

    mShader->Release();
}

void BSplineRenderer::CurveSelectionRenderer::Resize(int Width, int Height)
{
    mFramebuffer = std::make_unique<CurveSelectionFramebuffer>(Width, Height);
}

BSplineRenderer::CurveQueryInfo BSplineRenderer::CurveSelectionRenderer::Query(const QPoint& QueryPoint)
{
    return mFramebuffer->Query(QueryPoint);
}

void BSplineRenderer::CurveSelectionRenderer::SetCurveContainer(CurveContainer* pCurveContainer)
{
    mCurveContainer = pCurveContainer;
}

void BSplineRenderer::CurveSelectionRenderer::SetCamera(FreeCameraPtr pCamera)
{
    mCamera = pCamera;
}
