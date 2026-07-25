#include "Spline.h"

#include "Util/Logger.h"

void BSplineRenderer::Spline::AddKnot(KnotPtr pKnot)
{
    mKnots << pKnot;
    MakeDirty();
}

BSplineRenderer::KnotPtr BSplineRenderer::Spline::AddKnot(float X, float Y, float Z)
{
    return AddKnot(QVector3D(X, Y, Z));
}

BSplineRenderer::KnotPtr BSplineRenderer::Spline::AddKnot(const QVector3D& Position)
{
    KnotPtr pKnot = std::make_shared<Knot>(Position);
    mKnots << pKnot;
    MakeDirty();
    return pKnot;
}

void BSplineRenderer::Spline::MakeDirty()
{
    mDirty = true;
}

void BSplineRenderer::Spline::UpdateIfDirty()
{
    if (IsDirty())
    {
        Update();
    }
}

bool BSplineRenderer::Spline::IsDirty() const
{
    return mDirty;
}

void BSplineRenderer::Spline::Render()
{
    UpdateIfDirty();
    glBindVertexArray(mVAO);
    glDrawArrays(GL_PATCHES, 0, mBezierControlPoints.size());
}

void BSplineRenderer::Spline::Update()
{
    mBezierControlPoints.clear();

    if (mKnots.size() == 1)
    {
        mBezierControlPoints << mKnots[0]->GetPosition();
        mBezierControlPoints << mKnots[0]->GetPosition();
        mBezierControlPoints << mKnots[0]->GetPosition();
        mBezierControlPoints << mKnots[0]->GetPosition();
    }
    else if (mKnots.size() == 2)
    {
        mBezierControlPoints << mKnots[0]->GetPosition();
        mBezierControlPoints << mKnots[0]->GetPosition();
        mBezierControlPoints << mKnots[1]->GetPosition();
        mBezierControlPoints << mKnots[1]->GetPosition();
    }
    else if (mKnots.size() == 3)
    {
        for (int i = 0; i < 2; i++)
        {
            mBezierControlPoints << mKnots.at(i)->GetPosition();
            mBezierControlPoints << (2.0f / 3.0f) * mKnots.at(i)->GetPosition() + (1.0f / 3.0f) * mKnots.at(i + 1)->GetPosition();
            mBezierControlPoints << (1.0f / 3.0f) * mKnots.at(i)->GetPosition() + (2.0f / 3.0f) * mKnots.at(i + 1)->GetPosition();
            mBezierControlPoints << mKnots.at(i + 1)->GetPosition();
        }
    }
    else if (mKnots.size() >= 4)
    {
        UpdateSplineControlPoints();

        for (int i = 1; i < mKnots.size(); ++i)
        {
            mBezierControlPoints << mKnots.at(i - 1)->GetPosition();
            mBezierControlPoints << (2.0f / 3.0f) * mSplineControlPoints[i - 1] + (1.0f / 3.0f) * mSplineControlPoints[i];
            mBezierControlPoints << (1.0f / 3.0f) * mSplineControlPoints[i - 1] + (2.0f / 3.0f) * mSplineControlPoints[i];
            mBezierControlPoints << mKnots.at(i)->GetPosition();
        }
    }

    InitializeOpenGLStuffIfNot();
    DestroyOpenGLStuff();
    ContructOpenGLStuff();
    mDirty = false;
}

void BSplineRenderer::Spline::DestroyOpenGLStuff()
{
    if (mVAO != 0)
    {
        glDeleteVertexArrays(1, &mVAO);
        mVAO = 0;
    }

    if (mVBO != 0)
    {
        glDeleteBuffers(1, &mVBO);
        mVBO = 0;
    }
}

void BSplineRenderer::Spline::ContructOpenGLStuff()
{
    if (mVAO != 0 || mVBO != 0)
    {
        DestroyOpenGLStuff();
    }

    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, mBezierControlPoints.size() * sizeof(QVector3D), mBezierControlPoints.constData(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(QVector3D), (void*) 0);
    glEnableVertexAttribArray(0);

    glPatchParameteri(GL_PATCH_VERTICES, NUM_OF_PATCH_POINTS);

    if (mVAO == 0 || mVBO == 0)
    {
        BR_EXIT_FAILURE("Spline::ContructOpenGLStuff: OpenGL handle(s) could not be created! this = {:#010x}", reinterpret_cast<intptr_t>(this));
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    LOG_DEBUG("Spline::ContructOpenGLStuff: OpenGL stuff for Spline has been constructed. "
              "this = {:#010x}, mVAO = {}, mVBO = {}, # of Knots: {}",
              reinterpret_cast<intptr_t>(this),
              mVAO,
              mVBO,
              mKnots.size());
}

void BSplineRenderer::Spline::InitializeOpenGLStuffIfNot()
{
    if (mInitialized == false)
    {
        initializeOpenGLFunctions();
        mInitialized = true;
    }
}

Eigen::MatrixXf BSplineRenderer::Spline::CreateCoefficientMatrix()
{
    const auto N = mKnots.size() - 2;
    Eigen::MatrixXf Coef(N, N);

    for (int i = 0; i < N; ++i)
    {
        for (int j = 0; j < N; ++j)
        {
            Coef(i, j) = 0;
        }
    }

    // First row
    Coef(0, 0) = 4;
    Coef(0, 1) = 1;

    for (int i = 1; i < N - 1; ++i)
    {
        Coef(i, i - 1) = 1;
        Coef(i, i) = 4;
        Coef(i, i + 1) = 1;
    }

    // Last row
    Coef(N - 1, N - 2) = 1;
    Coef(N - 1, N - 1) = 4;

    return Coef;
}

void BSplineRenderer::Spline::UpdateSplineControlPoints()
{
    const auto N = mKnots.size();

    Eigen::MatrixXf KnotPoints(N, 3);

    for (int i = 0; i < N; ++i)
    {
        KnotPoints(i, 0) = mKnots.at(i)->GetPosition().x();
        KnotPoints(i, 1) = mKnots.at(i)->GetPosition().y();
        KnotPoints(i, 2) = mKnots.at(i)->GetPosition().z();
    }

    // Constants on the right side
    Eigen::MatrixXf Constants(N - 2, 3);

    for (int j = 0; j < 3; ++j)
    {
        Constants(0, j) = 6 * KnotPoints(1, j) - KnotPoints(0, j);
        Constants(N - 3, j) = 6 * KnotPoints(N - 2, j) - KnotPoints(N - 1, j);
    }

    for (int i = 1; i < N - 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            Constants(i, j) = 6 * KnotPoints(i + 1, j);
        }
    }

    // Compute BSpline control points
    Eigen::MatrixXf Coef = CreateCoefficientMatrix();
    Eigen::MatrixXf ControlPoints = Coef.inverse() * Constants;

    // Result
    mSplineControlPoints.clear();
    mSplineControlPoints.resize(N);

    mSplineControlPoints[0] = QVector3D(KnotPoints(0, 0), KnotPoints(0, 1), KnotPoints(0, 2));
    mSplineControlPoints[N - 1] = QVector3D(KnotPoints(N - 1, 0), KnotPoints(N - 1, 1), KnotPoints(N - 1, 2));

    for (int i = 0; i < N - 2; ++i)
    {
        mSplineControlPoints[i + 1] = QVector3D(ControlPoints(i, 0), ControlPoints(i, 1), ControlPoints(i, 2));
    }
}

BSplineRenderer::KnotPtr BSplineRenderer::Spline::GetClosestKnotToRay(const QVector3D& RayOrigin, const QVector3D& RayDirection, float MaxDistance) const
{
    float MinDistance = std::numeric_limits<float>::infinity();
    KnotPtr pClosestKnot = nullptr;

    for (auto& pKnot : mKnots)
    {
        const auto Difference = pKnot->GetPosition() - RayOrigin;

        const auto Dot = QVector3D::dotProduct(Difference, RayDirection);

        if (Dot >= 0.0f)
        {
            const auto Distance = (Difference - RayDirection * Dot).length();
            if (Distance < MinDistance)
            {
                MinDistance = Distance;
                pClosestKnot = pKnot;
            }
        }
    }

    if (MinDistance >= MaxDistance)
    {
        pClosestKnot = nullptr;
    }

    return pClosestKnot;
}

void BSplineRenderer::Spline::RemoveLastKnot()
{
    if (!mKnots.isEmpty())
    {
        mKnots.removeLast();
        MakeDirty();
    }
}

void BSplineRenderer::Spline::RemoveKnot(KnotPtr pKnot)
{
    mKnots.removeAll(pKnot);
    MakeDirty();
}

void BSplineRenderer::Spline::ClearKnots()
{
    mKnots.clear();
    MakeDirty();
}

int BSplineRenderer::Spline::GetKnotCount() const
{
    return mKnots.size();
}

float BSplineRenderer::Spline::GetTotalLength() const
{
    float Length = 0.0f;
    for (int i = 1; i < mKnots.size(); ++i)
    {
        Length += (mKnots[i]->GetPosition() - mKnots[i - 1]->GetPosition()).length();
    }
    return Length;
}

QVector3D BSplineRenderer::Spline::GetCentroid() const
{
    if (mKnots.isEmpty())
        return QVector3D(0, 0, 0);

    QVector3D Sum(0, 0, 0);
    for (const auto& pKnot : mKnots)
    {
        Sum += pKnot->GetPosition();
    }
    return Sum / mKnots.size();
}

QPair<QVector3D, QVector3D> BSplineRenderer::Spline::GetBoundingBox() const
{
    if (mKnots.isEmpty())
    {
        return { QVector3D(0, 0, 0), QVector3D(0, 0, 0) };
    }

    QVector3D MinPos = mKnots[0]->GetPosition();
    QVector3D MaxPos = mKnots[0]->GetPosition();

    for (const auto& pKnot : mKnots)
    {
        const QVector3D& Pos = pKnot->GetPosition();
        MinPos.setX(std::min(MinPos.x(), Pos.x()));
        MinPos.setY(std::min(MinPos.y(), Pos.y()));
        MinPos.setZ(std::min(MinPos.z(), Pos.z()));
        MaxPos.setX(std::max(MaxPos.x(), Pos.x()));
        MaxPos.setY(std::max(MaxPos.y(), Pos.y()));
        MaxPos.setZ(std::max(MaxPos.z(), Pos.z()));
    }

    return { MinPos, MaxPos };
}

const QVector<BSplineRenderer::KnotPtr>& BSplineRenderer::Spline::GetKnots() const
{
    return mKnots;
}
