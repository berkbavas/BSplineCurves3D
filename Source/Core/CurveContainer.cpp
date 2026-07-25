#include "CurveContainer.h"

void BSplineRenderer::CurveContainer::AddCurve(SplinePtr pSpline)
{
    mCurves << pSpline;
}

void BSplineRenderer::CurveContainer::RemoveCurve(SplinePtr pSpline)
{
    mCurves.removeAll(pSpline);
}

BSplineRenderer::SplinePtr BSplineRenderer::CurveContainer::GetCurve(int Index)
{
    return mCurves[Index];
}

const QVector<BSplineRenderer::SplinePtr>& BSplineRenderer::CurveContainer::GetCurves() const
{
    return mCurves;
}
