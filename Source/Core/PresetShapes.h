#pragma once

#include "Curve/Spline.h"

#include <QVector3D>

namespace BSplineRenderer
{
    class PresetShapes
    {
      public:
        static SplinePtr CreateCircle(float Radius = 5.0f, int NumPoints = 12, const QVector3D& Center = QVector3D(0, 0, 0));
        static SplinePtr CreateSpiral(float Radius = 3.0f, float Height = 10.0f, int Turns = 3, int PointsPerTurn = 8, const QVector3D& Center = QVector3D(0, 0, 0));
        static SplinePtr CreateHeart(float Scale = 3.0f, int NumPoints = 24, const QVector3D& Center = QVector3D(0, 0, 0));
        static SplinePtr CreateStar(int NumPoints = 5, float OuterRadius = 5.0f, float InnerRadius = 2.5f, const QVector3D& Center = QVector3D(0, 0, 0));
        static SplinePtr CreateWave(float Amplitude = 2.0f, float Frequency = 2.0f, float Length = 20.0f, int NumPoints = 32, const QVector3D& Start = QVector3D(-10, 0, 0));
        static SplinePtr CreateLissajous(float A = 3.0f, float B = 2.0f, float Alpha = 5.0f, float Beta = 5.0f, float Delta = M_PI / 2.0f, int NumPoints = 64, const QVector3D& Center = QVector3D(0, 0, 0));
        static SplinePtr CreateHelix(float Radius = 3.0f, float Pitch = 2.0f, int Turns = 4, int PointsPerTurn = 12, const QVector3D& Center = QVector3D(0, 0, 0));
        static SplinePtr CreateTrefoilKnot(float Scale = 3.0f, int NumPoints = 96, const QVector3D& Center = QVector3D(0, 0, 0));
        static SplinePtr CreateButterfly(float Scale = 2.0f, int NumPoints = 128, const QVector3D& Center = QVector3D(0, 0, 0));
    };
}
