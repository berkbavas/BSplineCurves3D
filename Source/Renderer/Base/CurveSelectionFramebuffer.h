#pragma once

#include <memory>

#include <QOpenGLFunctions_4_5_Core>
#include <QPoint>
#include <QVector4D>

namespace BSplineRenderer
{
    struct CurveQueryInfo
    {
        int Index;
        int Unused0;
        int Unused1;
        int Result; // 0: Fail, 1: Success
    };

    class CurveSelectionFramebuffer : protected QOpenGLFunctions_4_5_Core
    {
      public:
        CurveSelectionFramebuffer(int Width, int Height);
        ~CurveSelectionFramebuffer();

        void Clear();
        void Bind();
        CurveQueryInfo Query(const QPoint& QueryPoint);

        GLuint GetHandle() const { return mFramebuffer; }
        GLuint GetTexture() const { return mTexture; }

      private:
        GLuint mFramebuffer{ 0 };
        GLuint mTexture{ 0 };

        int mWidth{ 1 };
        int mHeight{ 1 };
    };

    using CurveSelectionFramebufferPtr = std::unique_ptr<CurveSelectionFramebuffer>;
}