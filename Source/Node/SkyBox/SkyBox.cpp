#include "SkyBox.h"

#include "Util/Logger.h"

#include <QImage>

BSplineRenderer::SkyBox::SkyBox(const QString& Folder, const QString& Extension)
{
    initializeOpenGLFunctions();

    QVector<QVector3D> Vertices;
    Vertices.reserve(36);

    // Front face
    Vertices.append(QVector3D(-1.0f, -1.0f, 1.0f));
    Vertices.append(QVector3D(1.0f, -1.0f, 1.0f));
    Vertices.append(QVector3D(1.0f, 1.0f, 1.0f));
    Vertices.append(QVector3D(1.0f, 1.0f, 1.0f));
    Vertices.append(QVector3D(-1.0f, 1.0f, 1.0f));
    Vertices.append(QVector3D(-1.0f, -1.0f, 1.0f));

    // Back face
    Vertices.append(QVector3D(-1.0f, -1.0f, -1.0f));
    Vertices.append(QVector3D(-1.0f, 1.0f, -1.0f));
    Vertices.append(QVector3D(1.0f, 1.0f, -1.0f));
    Vertices.append(QVector3D(1.0f, 1.0f, -1.0f));
    Vertices.append(QVector3D(1.0f, -1.0f, -1.0f));
    Vertices.append(QVector3D(-1.0f, -1.0f, -1.0f));

    // Left face
    Vertices.append(QVector3D(-1.0f, -1.0f, -1.0f));
    Vertices.append(QVector3D(-1.0f, -1.0f, 1.0f));
    Vertices.append(QVector3D(-1.0f, 1.0f, 1.0f));
    Vertices.append(QVector3D(-1.0f, 1.0f, 1.0f));
    Vertices.append(QVector3D(-1.0f, 1.0f, -1.0f));
    Vertices.append(QVector3D(-1.0f, -1.0f, -1.0f));

    // Right face
    Vertices.append(QVector3D(1.0f, -1.0f, -1.0f));
    Vertices.append(QVector3D(1.0f, 1.0f, -1.0f));
    Vertices.append(QVector3D(1.0f, 1.0f, 1.0f));
    Vertices.append(QVector3D(1.0f, 1.0f, 1.0f));
    Vertices.append(QVector3D(1.0f, -1.0f, 1.0f));
    Vertices.append(QVector3D(1.0f, -1.0f, -1.0f));

    // Top face
    Vertices.append(QVector3D(-1.0f, 1.0f, -1.0f));
    Vertices.append(QVector3D(-1.0f, 1.0f, 1.0f));
    Vertices.append(QVector3D(1.0f, 1.0f, 1.0f));
    Vertices.append(QVector3D(1.0f, 1.0f, 1.0f));
    Vertices.append(QVector3D(1.0f, 1.0f, -1.0f));
    Vertices.append(QVector3D(-1.0f, 1.0f, -1.0f));

    // Bottom face
    Vertices.append(QVector3D(-1.0f, -1.0f, -1.0f));
    Vertices.append(QVector3D(1.0f, -1.0f, -1.0f));
    Vertices.append(QVector3D(1.0f, -1.0f, 1.0f));
    Vertices.append(QVector3D(1.0f, -1.0f, 1.0f));
    Vertices.append(QVector3D(-1.0f, -1.0f, 1.0f));
    Vertices.append(QVector3D(-1.0f, -1.0f, -1.0f));

    mTexture;
    glGenTextures(1, &mTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, mTexture);

    QStringList Paths;
    Paths << Folder + "/" + "px" + Extension;
    Paths << Folder + "/" + "nx" + Extension;
    Paths << Folder + "/" + "py" + Extension;
    Paths << Folder + "/" + "ny" + Extension;
    Paths << Folder + "/" + "pz" + Extension;
    Paths << Folder + "/" + "nz" + Extension;

    QVector<int> Targets;

    Targets << GL_TEXTURE_CUBE_MAP_POSITIVE_X;
    Targets << GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
    Targets << GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
    Targets << GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
    Targets << GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
    Targets << GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;

    for (unsigned int Index = 0; Index < 6; Index++)
    {
        LOG_DEBUG("SkyBox::SkyBox: Loading texture from '{}'", Paths[Index].toStdString());

        const auto Image = QImage(Paths[Index]).mirrored().convertToFormat(QImage::Format_RGBA8888);

        if (Image.isNull())
        {
            BR_EXIT_FAILURE("SkyBox::SkyBox: Image is null.");
        }

        glTexImage2D(Targets[Index], 0, GL_RGBA, Image.width(), Image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, Image.bits());
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    LOG_DEBUG("SkyBox::Initialize: Constructing OpenGL stuff for SkyBox");

    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(QVector3D), Vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);

    if (mVAO == 0 || mVBO == 0)
    {
        BR_EXIT_FAILURE("SkyBox::Initialize: OpenGL handle(s) could not be created!");
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    LOG_DEBUG("SkyBox::Initialize: OpenGL stuff has been constructed.");
}

BSplineRenderer::SkyBox::~SkyBox()
{
    if (mVAO)
    {
        glDeleteVertexArrays(1, &mVAO);
        mVAO = 0;
    }

    if (mVBO)
    {
        glDeleteBuffers(1, &mVBO);
        mVBO = 0;
    }
}

void BSplineRenderer::SkyBox::Render()
{
    glDisable(GL_DEPTH_TEST);
    glBindVertexArray(mVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, mTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glEnable(GL_DEPTH_TEST);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
