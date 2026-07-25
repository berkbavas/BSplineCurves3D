#include "PresetShapes.h"

#include <cmath>

BSplineRenderer::SplinePtr BSplineRenderer::PresetShapes::CreateCircle(float Radius, int NumPoints, const QVector3D& Center)
{
    auto pSpline = std::make_shared<Spline>();

    for (int i = 0; i < NumPoints; ++i)
    {
        float Angle = 2.0f * M_PI * i / NumPoints;
        float X = Center.x() + Radius * std::cos(Angle);
        float Y = Center.y() + Radius * std::sin(Angle);
        float Z = Center.z();
        pSpline->AddKnot(X, Y, Z);
    }

    float X = Center.x() + Radius;
    float Y = Center.y();
    float Z = Center.z();
    pSpline->AddKnot(X, Y, Z);

    pSpline->SetColor(QVector3D(0.2f, 0.8f, 0.4f));
    return pSpline;
}

BSplineRenderer::SplinePtr BSplineRenderer::PresetShapes::CreateSpiral(float Radius, float Height, int Turns, int PointsPerTurn, const QVector3D& Center)
{
    auto pSpline = std::make_shared<Spline>();
    int TotalPoints = Turns * PointsPerTurn;

    for (int i = 0; i <= TotalPoints; ++i)
    {
        float T = static_cast<float>(i) / TotalPoints;
        float Angle = 2.0f * M_PI * Turns * T;
        float X = Center.x() + Radius * std::cos(Angle);
        float Y = Center.y() + Height * T - Height / 2.0f;
        float Z = Center.z() + Radius * std::sin(Angle);
        pSpline->AddKnot(X, Y, Z);
    }

    pSpline->SetColor(QVector3D(0.8f, 0.4f, 0.2f));
    return pSpline;
}

BSplineRenderer::SplinePtr BSplineRenderer::PresetShapes::CreateHeart(float Scale, int NumPoints, const QVector3D& Center)
{
    auto pSpline = std::make_shared<Spline>();

    for (int i = 0; i <= NumPoints; ++i)
    {
        float T = 2.0f * M_PI * i / NumPoints;
        float X = Scale * 16.0f * std::pow(std::sin(T), 3) / 16.0f;
        float Y = Scale * (13.0f * std::cos(T) - 5.0f * std::cos(2 * T) - 2.0f * std::cos(3 * T) - std::cos(4 * T)) / 16.0f;
        float Z = 0.0f;

        pSpline->AddKnot(Center.x() + X, Center.y() + Y, Center.z() + Z);
    }

    pSpline->SetColor(QVector3D(0.9f, 0.2f, 0.3f));
    return pSpline;
}

BSplineRenderer::SplinePtr BSplineRenderer::PresetShapes::CreateStar(int NumPoints, float OuterRadius, float InnerRadius, const QVector3D& Center)
{
    auto pSpline = std::make_shared<Spline>();

    for (int i = 0; i <= NumPoints * 2; ++i)
    {
        float Angle = M_PI * i / NumPoints - M_PI / 2.0f;
        float Radius = (i % 2 == 0) ? OuterRadius : InnerRadius;
        float X = Center.x() + Radius * std::cos(Angle);
        float Y = Center.y() + Radius * std::sin(Angle);
        float Z = Center.z();
        pSpline->AddKnot(X, Y, Z);
    }

    pSpline->SetColor(QVector3D(1.0f, 0.85f, 0.0f));
    return pSpline;
}

BSplineRenderer::SplinePtr BSplineRenderer::PresetShapes::CreateWave(float Amplitude, float Frequency, float Length, int NumPoints, const QVector3D& Start)
{
    auto pSpline = std::make_shared<Spline>();

    for (int i = 0; i <= NumPoints; ++i)
    {
        float T = static_cast<float>(i) / NumPoints;
        float X = Start.x() + Length * T;
        float Y = Start.y() + Amplitude * std::sin(2.0f * M_PI * Frequency * T);
        float Z = Start.z();
        pSpline->AddKnot(X, Y, Z);
    }

    pSpline->SetColor(QVector3D(0.2f, 0.6f, 0.9f));
    return pSpline;
}

BSplineRenderer::SplinePtr BSplineRenderer::PresetShapes::CreateLissajous(float A, float B, float Alpha, float Beta, float Delta, int NumPoints, const QVector3D& Center)
{
    auto pSpline = std::make_shared<Spline>();

    for (int i = 0; i <= NumPoints; ++i)
    {
        float T = 2.0f * M_PI * i / NumPoints;
        float X = Center.x() + Alpha * std::sin(A * T + Delta);
        float Y = Center.y() + Beta * std::sin(B * T);
        float Z = Center.z();
        pSpline->AddKnot(X, Y, Z);
    }

    pSpline->SetColor(QVector3D(0.7f, 0.3f, 0.9f));
    return pSpline;
}

BSplineRenderer::SplinePtr BSplineRenderer::PresetShapes::CreateHelix(float Radius, float Pitch, int Turns, int PointsPerTurn, const QVector3D& Center)
{
    auto pSpline = std::make_shared<Spline>();
    int TotalPoints = Turns * PointsPerTurn;

    for (int i = 0; i <= TotalPoints; ++i)
    {
        float T = static_cast<float>(i) / PointsPerTurn;
        float Angle = 2.0f * M_PI * T;
        float X = Center.x() + Radius * std::cos(Angle);
        float Y = Center.y() + Pitch * T;
        float Z = Center.z() + Radius * std::sin(Angle);
        pSpline->AddKnot(X, Y, Z);
    }

    pSpline->SetColor(QVector3D(0.4f, 0.8f, 0.8f));
    return pSpline;
}

BSplineRenderer::SplinePtr BSplineRenderer::PresetShapes::CreateTrefoilKnot(float Scale, int NumPoints, const QVector3D& Center)
{
    auto pSpline = std::make_shared<Spline>();

    for (int i = 0; i <= NumPoints; ++i)
    {
        float T = 2.0f * M_PI * i / NumPoints;
        float X = Center.x() + Scale * (std::sin(T) + 2.0f * std::sin(2.0f * T));
        float Y = Center.y() + Scale * (std::cos(T) - 2.0f * std::cos(2.0f * T));
        float Z = Center.z() + Scale * (-std::sin(3.0f * T));
        pSpline->AddKnot(X, Y, Z);
    }

    pSpline->SetColor(QVector3D(0.9f, 0.5f, 0.1f));
    return pSpline;
}

BSplineRenderer::SplinePtr BSplineRenderer::PresetShapes::CreateButterfly(float Scale, int NumPoints, const QVector3D& Center)
{
    auto pSpline = std::make_shared<Spline>();

    for (int i = 0; i <= NumPoints; ++i)
    {
        float T = 12.0f * M_PI * i / NumPoints;
        float R = std::exp(std::cos(T)) - 2.0f * std::cos(4.0f * T) - std::pow(std::sin(T / 12.0f), 5);
        float X = Center.x() + Scale * R * std::sin(T);
        float Y = Center.y() + Scale * R * std::cos(T);
        float Z = Center.z();
        pSpline->AddKnot(X, Y, Z);
    }

    pSpline->SetColor(QVector3D(0.95f, 0.6f, 0.8f));
    return pSpline;
}
