#pragma once

#include "Node/Mesh/Mesh.h"
#include "Node/Mesh/Texture.h"
#include "Util/Macros.h"

#include <QObject>
#include <QOpenGLExtraFunctions>
#include <QString>

namespace BSplineRenderer
{
    class SkyBox : public Mesh
    {
      public:
        SkyBox(const QString& Folder, const QString& Extension);
        ~SkyBox();

        void Render() override;

      private:
        GLuint mTexture;
        GLuint mVAO{ 0 };
        GLuint mVBO{ 0 };

        DEFINE_MEMBER(float, Brightness, 0.5f);
    };

    using SkyBoxPtr = std::shared_ptr<SkyBox>;
}