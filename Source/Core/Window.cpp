#include "Window.h"

#include "Util/Logger.h"

#include <QDateTime>
#include <QDebug>
#include <QKeyEvent>

BSplineRenderer::Window::Window(QWindow* pParent)
    : QOpenGLWindow(QOpenGLWindow::UpdateBehavior::NoPartialUpdate, pParent)

{
    QSurfaceFormat Format = QSurfaceFormat::defaultFormat();
    Format.setSamples(8);
    setFormat(Format);

    connect(this, &QOpenGLWindow::frameSwapped, [=]() { update(); });
}

void BSplineRenderer::Window::initializeGL()
{
    initializeOpenGLFunctions();

    mCurrentTime = QDateTime::currentMSecsSinceEpoch();
    mPreviousTime = mCurrentTime;

    emit Initialize();
}

void BSplineRenderer::Window::resizeGL(int Width, int Height)
{
    emit Resize(Width, Height);
}

void BSplineRenderer::Window::paintGL()
{
    mCurrentTime = QDateTime::currentMSecsSinceEpoch();
    const float ifps = (mCurrentTime - mPreviousTime) * 0.001f;
    mPreviousTime = mCurrentTime;

    emit Render(ifps);
}

void BSplineRenderer::Window::keyPressEvent(QKeyEvent* pEvent)
{
    emit KeyPressed(pEvent);
}

void BSplineRenderer::Window::keyReleaseEvent(QKeyEvent* pEvent)
{
    emit KeyReleased(pEvent);
}

void BSplineRenderer::Window::mousePressEvent(QMouseEvent* pEvent)
{
    emit MousePressed(pEvent);
}

void BSplineRenderer::Window::mouseReleaseEvent(QMouseEvent* pEvent)
{
    emit MouseReleased(pEvent);
}

void BSplineRenderer::Window::mouseMoveEvent(QMouseEvent* pEvent)
{
    emit MouseMoved(pEvent);
}

void BSplineRenderer::Window::wheelEvent(QWheelEvent* pEvent)
{
    emit WheelMoved(pEvent);
}
