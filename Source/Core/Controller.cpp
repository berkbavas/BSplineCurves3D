#include "Controller.h"

#include "Core/AnimationManager.h"
#include "Core/CurveContainer.h"
#include "Core/UndoRedoManager.h"
#include "Core/Window.h"
#include "Gui/ImGuiWindow.h"
#include "Renderer/RendererManager.h"
#include "Util/Logger.h"

#include <imgui.h>

#include <QThread>
#include <QtImGui.h>

BSplineRenderer::Controller::Controller(QObject* pParent)
    : QObject(pParent)
{
    mWindow = new Window;
    mEventHandler = new EventHandler(this);
    mImGuiWindow = new ImGuiWindow(this);
    mRendererManager = new RendererManager;
    mCurveContainer = new CurveContainer;

    mCamera = mRendererManager->GetCamera();
    mEventHandler->SetCamera(mCamera);
    mEventHandler->SetCurveContainer(mCurveContainer);
    mEventHandler->SetRendererManager(mRendererManager);

    mRendererManager->SetCurveContainer(mCurveContainer);
    mImGuiWindow->SetRendererManager(mRendererManager);
    mImGuiWindow->SetCurveContainer(mCurveContainer);

    connect(mWindow, &Window::Initialize, this, &Controller::Initialize);
    connect(mWindow, &Window::Render, this, &Controller::Render);
    connect(mWindow, &Window::Resize, this, &Controller::Resize);
    connect(mWindow, &Window::MousePressed, this, &Controller::OnMousePressed);
    connect(mWindow, &Window::MouseReleased, this, &Controller::OnMouseReleased);
    connect(mWindow, &Window::MouseMoved, this, &Controller::OnMouseMoved);
    connect(mWindow, &Window::WheelMoved, this, &Controller::OnWheelMoved);
    connect(mWindow, &Window::KeyPressed, this, &Controller::OnKeyPressed);
    connect(mWindow, &Window::KeyReleased, this, &Controller::OnKeyReleased);

    connect(mEventHandler, &EventHandler::SelectedCurveChanged, this, [this](SplinePtr pCurve) {
        mRendererManager->SetSelectedCurve(pCurve);
        mImGuiWindow->SetSelectedCurve(pCurve);
    });

    connect(mEventHandler, &EventHandler::SelectedKnotChanged, this, [this](KnotPtr pKnot) {
        mRendererManager->SetSelectedKnot(pKnot);
        mImGuiWindow->SetSelectedKnot(pKnot);
    });

    connect(mEventHandler, &EventHandler::KnotAroundChanged, this, [this](KnotPtr pKnot) { mRendererManager->SetKnotAround(pKnot); });

    connect(mImGuiWindow, &ImGuiWindow::RequestCameraReset, this, [this]() { mCamera->Reset(); });
    connect(mImGuiWindow, &ImGuiWindow::RequestCameraPreset, this, [this](int Preset) { ApplyCameraPreset(Preset); });
    connect(mImGuiWindow, &ImGuiWindow::CurveAdded, this, [this](SplinePtr pSpline) {
        mEventHandler->SetSelectedCurve(pSpline);
        LOG_INFO("Preset shape added with {} knots", pSpline->GetKnotCount());
    });
}

BSplineRenderer::Controller::~Controller()
{
    qDebug() << "Controller::~Controller: Application closing...";
    qDebug() << "Controller::~Controller: Current Thread:" << QThread::currentThread();
}

void BSplineRenderer::Controller::Run()
{
    qDebug() << "Controller::Controller: Application starting...";
    qInstallMessageHandler(Logger::QtMessageOutputCallback);
    mWindow->showMinimized();
}

void BSplineRenderer::Controller::Initialize()
{
    initializeOpenGLFunctions();

    mRendererManager->Initialize();

    QtImGui::initialize(mWindow);

    mWindow->showMaximized();
}

void BSplineRenderer::Controller::Render(float Ifps)
{
    mCamera->Resize(mWidth, mHeight);
    mCamera->Update(Ifps);
    mEventHandler->SetDevicePixelRatio(mDevicePixelRatio);

    AnimationManager::Instance().Update(Ifps, mCurveContainer);

    mRendererManager->Render();

    QtImGui::newFrame();

    mImGuiWindow->Draw();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, mWidth, mHeight);

    ImGui::Render();
    QtImGui::render();
}

void BSplineRenderer::Controller::OnKeyPressed(QKeyEvent* pEvent)
{
    mEventHandler->OnKeyPressed(pEvent);
}

void BSplineRenderer::Controller::OnKeyReleased(QKeyEvent* pEvent)
{
    mEventHandler->OnKeyReleased(pEvent);
}

void BSplineRenderer::Controller::Resize(int Width, int Height)
{
    mDevicePixelRatio = mWindow->devicePixelRatio();
    mWidth = Width * mDevicePixelRatio;
    mHeight = Height * mDevicePixelRatio;

    mWindow->makeCurrent();
    mRendererManager->Resize(mWidth, mHeight);
    mWindow->doneCurrent();
}

void BSplineRenderer::Controller::OnMousePressed(QMouseEvent* pEvent)
{
    if (ImGui::GetIO().WantCaptureMouse)
    {
        return;
    }

    mEventHandler->OnMousePressed(pEvent);
}

void BSplineRenderer::Controller::OnMouseReleased(QMouseEvent* pEvent)
{
    mEventHandler->OnMouseReleased(pEvent);
}

void BSplineRenderer::Controller::OnMouseMoved(QMouseEvent* pEvent)
{
    if (ImGui::GetIO().WantCaptureMouse)
    {
        return;
    }

    mEventHandler->OnMouseMoved(pEvent);
}

void BSplineRenderer::Controller::OnWheelMoved(QWheelEvent* pEvent)
{
    if (ImGui::GetIO().WantCaptureMouse)
    {
        return;
    }

    mEventHandler->OnWheelMoved(pEvent);
}

void BSplineRenderer::Controller::ApplyCameraPreset(int Preset)
{
    // Camera preset positions
    switch (Preset)
    {
    case 0: // Front
        mCamera->SetPosition(QVector3D(0, 0, 30));
        mCamera->SetRotation(QQuaternion::fromEulerAngles(0, 0, 0));
        break;
    case 1: // Back
        mCamera->SetPosition(QVector3D(0, 0, -30));
        mCamera->SetRotation(QQuaternion::fromEulerAngles(0, 180, 0));
        break;
    case 2: // Left
        mCamera->SetPosition(QVector3D(-30, 0, 0));
        mCamera->SetRotation(QQuaternion::fromEulerAngles(0, 90, 0));
        break;
    case 3: // Right
        mCamera->SetPosition(QVector3D(30, 0, 0));
        mCamera->SetRotation(QQuaternion::fromEulerAngles(0, -90, 0));
        break;
    case 4: // Top
        mCamera->SetPosition(QVector3D(0, 30, 0));
        mCamera->SetRotation(QQuaternion::fromEulerAngles(90, 0, 0));
        break;
    case 5: // Bottom
        mCamera->SetPosition(QVector3D(0, -30, 0));
        mCamera->SetRotation(QQuaternion::fromEulerAngles(-90, 0, 0));
        break;
    case 6: // Isometric
        mCamera->SetPosition(QVector3D(20, 20, 20));
        mCamera->SetRotation(QQuaternion::fromEulerAngles(35.264f, -45, 0));
        break;
    }
}