#include "AnimationManager.h"

BSplineRenderer::AnimationManager &BSplineRenderer::AnimationManager::Instance()
{
    static AnimationManager Instance;
    return Instance;
}

void BSplineRenderer::AnimationManager::Update(float DeltaTime, CurveContainer *pContainer)
{
    if (!mEnabled || mAnimationType == AnimationType::None)
    {
        return;
    }

    mTime += DeltaTime * mSpeed;

    for (const auto &Spline : pContainer->GetCurves())
    {
        ApplyAnimation(Spline);
    }
}

void BSplineRenderer::AnimationManager::ApplyAnimation(SplinePtr pSpline)
{
    switch (mAnimationType)
    {
    case AnimationType::Rotate:
        ApplyRotation(pSpline);
        break;
    case AnimationType::Pulse:
        ApplyPulse(pSpline);
        break;
    case AnimationType::Wave:
        ApplyWave(pSpline);
        break;
    case AnimationType::Bounce:
        ApplyBounce(pSpline);
        break;
    case AnimationType::Spiral:
        ApplySpiral(pSpline);
        break;
    default:
        break;
    }
}

void BSplineRenderer::AnimationManager::Reset()
{
    mTime = 0.0f;

    // Restore original positions
    for (auto &[pSpline, Positions] : mOriginalPositions)
    {
        const auto &Knots = pSpline->GetKnots();

        for (int i = 0; i < Knots.size() && i < Positions.size(); ++i)
        {
            Knots[i]->SetPosition(Positions[i]);
        }

        pSpline->MakeDirty();
    }
}

void BSplineRenderer::AnimationManager::SaveOriginalPositions(CurveContainer *pContainer)
{
    mOriginalPositions.clear();

    for (const auto &pSpline : pContainer->GetCurves())
    {
        QVector<QVector3D> Positions;

        for (const auto &Knot : pSpline->GetKnots())
        {
            Positions.append(Knot->GetPosition());
        }

        mOriginalPositions[pSpline] = Positions;
    }
}

const char *BSplineRenderer::AnimationManager::GetAnimationTypeName(AnimationType AnimationType) const
{
    switch (AnimationType)
    {
    case AnimationType::None:
        return "None";
    case AnimationType::Rotate:
        return "Rotate";
    case AnimationType::Pulse:
        return "Pulse";
    case AnimationType::Wave:
        return "Wave";
    case AnimationType::Bounce:
        return "Bounce";
    case AnimationType::Spiral:
        return "Spiral";
    default:
        return "Unknown";
    }
}

void BSplineRenderer::AnimationManager::ApplyRotation(SplinePtr pSpline)
{
    if (!mOriginalPositions.contains(pSpline))
    {
        return;
    }

    const auto &OriginalPositions = mOriginalPositions[pSpline];
    const auto &Knots = pSpline->GetKnots();

    QMatrix4x4 Rotation;
    Rotation.rotate(mTime * 30.0f, 0, 1, 0); // Rotate around Y axis

    for (int i = 0; i < Knots.size() && i < OriginalPositions.size(); ++i)
    {
        QVector3D newPos = Rotation.map(OriginalPositions[i]);
        Knots[i]->SetPosition(newPos);
    }

    pSpline->MakeDirty();
}

void BSplineRenderer::AnimationManager::ApplyPulse(SplinePtr pSpline)
{
    if (!mOriginalPositions.contains(pSpline))
    {
        return;
    }

    const auto &OriginalPositions = mOriginalPositions[pSpline];
    const auto &Knots = pSpline->GetKnots();

    const auto Scale = 1.0f + mAmplitude * 0.2f * std::sin(mTime * 2.0f);

    for (int i = 0; i < Knots.size() && i < OriginalPositions.size(); ++i)
    {
        const auto NewPos = OriginalPositions[i] * Scale;
        Knots[i]->SetPosition(NewPos);
    }

    pSpline->MakeDirty();
}

void BSplineRenderer::AnimationManager::ApplyWave(SplinePtr pSpline)
{
    if (!mOriginalPositions.contains(pSpline))
    {
        return;
    }

    const auto &OriginalPositions = mOriginalPositions[pSpline];
    const auto &Knots = pSpline->GetKnots();

    for (int i = 0; i < Knots.size() && i < OriginalPositions.size(); ++i)
    {
        float offset = mAmplitude * std::sin(mTime * 2.0f + i * 0.5f);
        QVector3D newPos = OriginalPositions[i] + QVector3D(0, offset, 0);
        Knots[i]->SetPosition(newPos);
    }

    pSpline->MakeDirty();
}

void BSplineRenderer::AnimationManager::ApplyBounce(SplinePtr pSpline)
{
    if (!mOriginalPositions.contains(pSpline))
    {
        return;
    }

    const auto &OriginalPositions = mOriginalPositions[pSpline];
    const auto &Knots = pSpline->GetKnots();

    const auto Bounce = mAmplitude * std::abs(std::sin(mTime * 3.0f));

    for (int i = 0; i < Knots.size() && i < OriginalPositions.size(); ++i)
    {
        QVector3D newPos = OriginalPositions[i] + QVector3D(0, Bounce, 0);
        Knots[i]->SetPosition(newPos);
    }

    pSpline->MakeDirty();
}

void BSplineRenderer::AnimationManager::ApplySpiral(SplinePtr pSpline)
{
    if (!mOriginalPositions.contains(pSpline))
    {
        return;
    }

    const auto &OriginalPositions = mOriginalPositions[pSpline];
    const auto &Knots = pSpline->GetKnots();

    for (int i = 0; i < Knots.size() && i < OriginalPositions.size(); ++i)
    {
        float Angle = mTime + i * 0.3f;
        float SpiralOffset = mAmplitude * 0.5f * std::sin(Angle);
        QMatrix4x4 Rotation;
        Rotation.rotate(mTime * 20.0f + i * 10.0f, 0, 1, 0);

        QVector3D RotatedPos = Rotation.map(OriginalPositions[i]);
        RotatedPos += QVector3D(0, SpiralOffset, 0);
        Knots[i]->SetPosition(RotatedPos);
    }

    pSpline->MakeDirty();
}
