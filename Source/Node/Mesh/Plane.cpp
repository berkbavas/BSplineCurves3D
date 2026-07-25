#include "Plane.h"

#include "Util/Logger.h"

#include <QVector3D>

BSplineRenderer::Plane::Plane(float Size)
{
    LOG_DEBUG("Plane::Plane: Constructing OpenGL stuff for Plane.");

    // Define the vertices of the plane
    mVertices = {
        { QVector3D(-Size / 2.0f, 0.0f, -Size / 2.0f), QVector3D(0.0f, 1.0f, 0.0f), QVector2D(0.0f, 0.0f) }, //
        { QVector3D(Size / 2.0f, 0.0f, -Size / 2.0f), QVector3D(0.0f, 1.0f, 0.0f), QVector2D(1.0f, 0.0f) },  //
        { QVector3D(Size / 2.0f, 0.0f, Size / 2.0f), QVector3D(0.0f, 1.0f, 0.0f), QVector2D(1.0f, 1.0f) },   //
        { QVector3D(-Size / 2.0f, 0.0f, Size / 2.0f), QVector3D(0.0f, 1.0f, 0.0f), QVector2D(0.0f, 1.0f) }   //
    };

    initializeOpenGLFunctions();

    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), mVertices.constData(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) (offsetof(Vertex, Normal)));
    glEnableVertexAttribArray(1);

    if (mVAO == 0 || mVBO == 0)
    {
        BR_EXIT_FAILURE("Plane::Plane: OpenGL handle(s) could not be created!");
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    LOG_DEBUG("Plane::Plane: OpenGL stuff for Plane has been constructed.");
}

BSplineRenderer::Plane::~Plane()
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

void BSplineRenderer::Plane::Render()
{
    glBindVertexArray(mVAO);
    glDrawArrays(GL_TRIANGLES, 0, mVertices.size());
    glBindVertexArray(0);
}
