#pragma once

#include "Core/CurveContainer.h"
#include "Curve/Spline.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>

namespace BSplineRenderer
{
    class CurveSerializer
    {
      public:
        // Save all curves to JSON file
        static bool SaveToFile(const QString& FilePath, CurveContainer* pContainer);

        // Load curves from JSON file
        static bool LoadFromFile(const QString& FilePath, CurveContainer* pContainer);

        // Convert a single curve to JSON string
        static QString SplineToJson(SplinePtr pSpline);
    };
}
