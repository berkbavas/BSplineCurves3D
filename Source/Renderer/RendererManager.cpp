#include "RendererManager.h"

#include "Core/CurveContainer.h"
#include "Renderer/SplineRenderer.h"

BSplineRenderer::RendererManager::RendererManager()
{
    mCamera = std::make_shared<FreeCamera>();
    mCamera->SetPosition(0, 0, 10);
    mCamera->MakeDirty();
    mLight = std::make_shared<DirectionalLight>();
    mLight->SetDirection(QVector3D(0, 0, 1).normalized());

    mSplineRenderer = new SplineRenderer;
    mCurveSelectionRenderer = new CurveSelectionRenderer;
}

void BSplineRenderer::RendererManager::Initialize()
{
    initializeOpenGLFunctions();

    glEnable(GL_MULTISAMPLE);

    mSplineRenderer->SetCamera(mCamera);
    mSplineRenderer->SetLight(mLight);
    mSplineRenderer->SetCurveContainer(mCurveContainer);
    mSplineRenderer->Initialize();

    mCurveSelectionRenderer->SetCamera(mCamera);
    mCurveSelectionRenderer->SetCurveContainer(mCurveContainer);
    mCurveSelectionRenderer->Initialize();

    mModelShader = new Shader("Model Shader");
    mModelShader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Model.vert");
    mModelShader->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/Model.frag");
    mModelShader->Initialize();

    mSkyBoxShader = new Shader("SkyBox Shader");
    mSkyBoxShader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/SkyBox.vert");
    mSkyBoxShader->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/SkyBox.frag");
    mSkyBoxShader->Initialize();

    mSkyBox = std::make_shared<SkyBox>("Resources/SkyBox", ".png");

    mSphereModel = new Model(std::make_shared<Sphere>(1, 32, 32));
}

void BSplineRenderer::RendererManager::Resize(int Width, int Height)
{
    mCurveSelectionRenderer->Resize(Width, Height);
}

void BSplineRenderer::RendererManager::Render()
{
    mLight->SetDirection(mCamera->GetViewDirection());

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, mCamera->GetWidth(), mCamera->GetHeight());
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mSkyBoxShader->Bind();
    mSkyBoxShader->SetUniformValue("uRotation", mCamera->GetRotationMatrix());
    mSkyBoxShader->SetUniformValue("uProjection", mCamera->GetProjectionMatrix());
    mSkyBoxShader->SetUniformValue("uTexture", 0);
    mSkyBoxShader->SetUniformValue("uBrightness", mSkyBox->GetBrightness());
    mSkyBox->Render();
    mSkyBoxShader->Release();

    glEnable(GL_DEPTH_TEST);

    mModelShader->Bind();
    mModelShader->SetUniformValue("uVP", mCamera->GetProjectionMatrix() * mCamera->GetViewMatrix());
    mModelShader->SetUniformValue("uLight.Color", mLight->GetColor());
    mModelShader->SetUniformValue("uLight.Direction", mLight->GetDirection());
    mModelShader->SetUniformValue("uLight.Ambient", mLight->GetAmbient());
    mModelShader->SetUniformValue("uLight.Diffuse", mLight->GetDiffuse());

    for (const auto& pModel : mModels)
    {
        mModelShader->SetUniformValue("uModelMatrix", pModel->GetTransformation());
        mModelShader->SetUniformValue("uNormalMatrix", pModel->GetTransformation().normalMatrix());
        mModelShader->SetUniformValue("uModel.Color", pModel->GetColor());
        mModelShader->SetUniformValue("uModel.Ambient", pModel->GetAmbient());
        mModelShader->SetUniformValue("uModel.Diffuse", pModel->GetDiffuse());
        pModel->GetMesh()->Render();
    }

    mModelShader->Release();

    if (mSelectedCurve)
    {
        RenderKnots(mSelectedCurve);
    }

    mSplineRenderer->Render();
    mCurveSelectionRenderer->Render();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

BSplineRenderer::CurveQueryInfo BSplineRenderer::RendererManager::Query(const QPoint& QueryPoint)
{
    return mCurveSelectionRenderer->Query(QueryPoint);
}

void BSplineRenderer::RendererManager::AddModel(ModelPtr pModel)
{
    mModels << pModel;
}

void BSplineRenderer::RendererManager::RemoveModel(ModelPtr pModel)
{
    mModels.removeAll(pModel);
}

void BSplineRenderer::RendererManager::SetNumberOfSegments(int NumberOfSegments)
{
    mSplineRenderer->SetNumberOfSegments(NumberOfSegments);
    mCurveSelectionRenderer->SetNumberOfSegments(NumberOfSegments);
}

void BSplineRenderer::RendererManager::SetNumberOfSectors(int NumberOfSectors)
{
    mSplineRenderer->SetNumberOfSectors(NumberOfSectors);
    mCurveSelectionRenderer->SetNumberOfSectors(NumberOfSectors);
}

int BSplineRenderer::RendererManager::GetNumberOfSegments() const
{
    return mSplineRenderer->GetNumberOfSegments();
}

int BSplineRenderer::RendererManager::GetNumberOfSectors() const
{
    return mSplineRenderer->GetNumberOfSectors();
}

bool* BSplineRenderer::RendererManager::GetWireframe()
{
    return &mSplineRenderer->GetWireframe_NonConst();
}

BSplineRenderer::GlowSettings& BSplineRenderer::RendererManager::GetGlowSettings()
{
    return mSplineRenderer->GetGlowSettings();
}

void BSplineRenderer::RendererManager::RenderKnots(SplinePtr pCurve)
{
    const auto& Knots = pCurve->GetKnots();
    const auto Radius = pCurve->GetRadius();

    mModelShader->Bind();
    mModelShader->SetUniformValue("uModel.Ambient", mSphereModel->GetAmbient());
    mModelShader->SetUniformValue("uModel.Diffuse", mSphereModel->GetDiffuse());

    for (const auto& pKnot : Knots)
    {
        if (mSelectedKnot == pKnot)
        {
            mSphereModel->SetColor(QVector3D(0, 1, 0));
            mSphereModel->SetScale(2.5 * Radius, 2.5 * Radius, 2.5 * Radius);
        }
        else if (mKnotAround == pKnot)
        {
            mSphereModel->SetColor(QVector3D(1, 1, 0));
            mSphereModel->SetScale(2.5 * Radius, 2.5 * Radius, 2.5 * Radius);
        }
        else
        {
            mSphereModel->SetColor(QVector3D(1, 1, 0));
            mSphereModel->SetScale(2 * Radius, 2 * Radius, 2 * Radius);
        }

        mSphereModel->SetPosition(pKnot->GetPosition());
        mModelShader->SetUniformValue("uModelMatrix", mSphereModel->GetTransformation());
        mModelShader->SetUniformValue("uNormalMatrix", mSphereModel->GetTransformation().normalMatrix());
        mModelShader->SetUniformValue("uModel.Color", mSphereModel->GetColor());

        mSphereModel->GetMesh()->Render();
    }

    mModelShader->Release();
}
