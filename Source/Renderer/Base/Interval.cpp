#include "Interval.h"

BSplineRenderer::Interval::Interval(float Start, float End, int Size)
    : mStart(Start)
    , mEnd(End)
    , mSize(Size)
{
    initializeOpenGLFunctions();

    mPoints = QVector<float>(mSize, 0.0);

    for (int i = 0; i < mSize; ++i)
    {
        mPoints[i] = mStart + (mEnd - mStart) * i / (mSize - 1);
    }

    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, mPoints.size() * sizeof(float), mPoints.constData(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, 0, (void *) 0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void BSplineRenderer::Interval::Bind()
{
    glBindVertexArray(mVAO);
}

void BSplineRenderer::Interval::Render()
{
    glDrawArrays(GL_POINTS, 0, mPoints.size());
}

void BSplineRenderer::Interval::Release()
{
    glBindVertexArray(0);
}

void BSplineRenderer::Interval::Destroy()
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

float BSplineRenderer::Interval::GetDelta() const
{
    return (mEnd - mStart) / mSize;
}
