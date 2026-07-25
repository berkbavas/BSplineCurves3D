#pragma once

#include "Util/Macros.h"

#include <QOpenGLExtraFunctions>
#include <QVector>

namespace BSplineRenderer
{
    class Interval : protected QOpenGLExtraFunctions
    {
      public:
        explicit Interval(float Start, float End, int Size);

        void Bind();
        void Render();
        void Release();
        void Destroy();
        float GetDelta() const;

      private:
        QVector<float> mPoints;
        float mStart;
        float mEnd;
        int mSize;

        GLuint mVAO{ 0 };
        GLuint mVBO{ 0 };
    };

}