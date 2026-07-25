#pragma once

#include "Curve/Spline.h"

#include <QVector>

namespace BSplineRenderer
{
    class CurveContainer
    {
      public:
        CurveContainer() = default;

        void AddCurve(SplinePtr pSpline);
        void RemoveCurve(SplinePtr pSpline);
        SplinePtr GetCurve(int Index);

        const QVector<SplinePtr>& GetCurves() const;

      private:
        QVector<SplinePtr> mCurves;
    };
}