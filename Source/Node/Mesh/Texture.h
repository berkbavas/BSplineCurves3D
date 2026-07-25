#pragma once

#include <QOpenGLFunctions_4_5_Core>
#include <QString>

namespace BSplineRenderer
{
    struct Texture
    {
        GLuint Id;     // OpenGL handle
        GLuint Target; // GL_TEXTURE_2D, GL_TEXTURE_CUBE_MAP etc.
        GLuint Unit;   // Uniform location
        QString Name;  // Texture name in the shader
    };
}