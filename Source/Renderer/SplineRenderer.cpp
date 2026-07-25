#include "SplineRenderer.h"

#include "Core/CurveContainer.h"

void BSplineRenderer::SplineRenderer::Initialize()
{
    initializeOpenGLFunctions();

    mSplineShader = new Shader("Spline Shader");
    mSplineShader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Spline.vert");
    mSplineShader->AddPath(QOpenGLShader::TessellationControl, ":/Resources/Shaders/Spline.tcs");
    mSplineShader->AddPath(QOpenGLShader::TessellationEvaluation, ":/Resources/Shaders/Spline.tes");
    mSplineShader->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/Spline.frag");
    mSplineShader->Initialize();

    // Start timer for glow animations
    mTimer.start();
}

void BSplineRenderer::SplineRenderer::Render()
{
    if (mWireframe)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    mSplineShader->Bind();
    mSplineShader->SetUniformValue("uNumberOfSegments", mNumberOfSegments);
    mSplineShader->SetUniformValue("uNumberOfSectors", mNumberOfSectors);
    mSplineShader->SetUniformValue("uVP", mCamera->GetProjectionMatrix() * mCamera->GetViewMatrix());

    mSplineShader->SetUniformValue("uLight.Color", mLight->GetColor());
    mSplineShader->SetUniformValue("uLight.Direction", mLight->GetDirection());
    mSplineShader->SetUniformValue("uLight.Ambient", mLight->GetAmbient());
    mSplineShader->SetUniformValue("uLight.Diffuse", mLight->GetDiffuse());

    // Glow effect uniforms
    const auto ElapsedTime = mTimer.elapsed() / 1000.0f;
    mSplineShader->SetUniformValue("uTime", ElapsedTime);
    mSplineShader->SetUniformValue("uGlow.Intensity", mGlowSettings.Intensity);
    mSplineShader->SetUniformValue("uGlow.Pulse", mGlowSettings.Pulse);
    mSplineShader->SetUniformValue("uGlow.Shimmer", mGlowSettings.Shimmer);
    mSplineShader->SetUniformValue("uGlow.CoreBoost", mGlowSettings.CoreBoost);

    const auto& Curves = mCurveContainer->GetCurves();

    for (int Index = 0; Index < Curves.size(); ++Index)
    {
        const auto& Curve = Curves[Index];
        mSplineShader->SetUniformValue("uCurve.Color", Curve->GetColor());
        mSplineShader->SetUniformValue("uCurve.Ambient", Curve->GetAmbient());
        mSplineShader->SetUniformValue("uCurve.Diffuse", Curve->GetDiffuse());
        mSplineShader->SetUniformValue("uRadius", Curve->GetRadius());
        Curve->Render();
    }

    mSplineShader->Release();

    if (mWireframe)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

void BSplineRenderer::SplineRenderer::SetCurveContainer(CurveContainer* pCurveContainer)
{
    mCurveContainer = pCurveContainer;
}

void BSplineRenderer::SplineRenderer::SetCamera(FreeCameraPtr pCamera)
{
    mCamera = pCamera;
}

void BSplineRenderer::SplineRenderer::SetLight(DirectionalLightPtr pLight)
{
    mLight = pLight;
}

BSplineRenderer::GlowSettings& BSplineRenderer::SplineRenderer::GetGlowSettings()
{
    return mGlowSettings;
}

const BSplineRenderer::GlowSettings& BSplineRenderer::SplineRenderer::GetGlowSettings() const
{
    return mGlowSettings;
}
