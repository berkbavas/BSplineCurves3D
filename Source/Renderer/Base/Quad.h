#pragma once

#include <QOpenGLExtraFunctions>

namespace BSplineRenderer
{
    class Quad : protected QOpenGLExtraFunctions
    {
      public:
        Quad();
        ~Quad();

        void Render();

      private:
        GLuint mVAO;
        GLuint mVBO;
    };
}
