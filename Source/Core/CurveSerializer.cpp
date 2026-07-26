#include "CurveSerializer.h"

bool BSplineRenderer::CurveSerializer::SaveToFile(const QString &FilePath, CurveContainer *pContainer)
{
    QJsonObject Root;
    QJsonArray CurvesArray;

    for (const auto &pSpline : pContainer->GetCurves())
    {
        QJsonObject CurveObj;
        CurveObj["radius"] = pSpline->GetRadius();
        CurveObj["ambient"] = pSpline->GetAmbient();
        CurveObj["diffuse"] = pSpline->GetDiffuse();
        CurveObj["specular"] = pSpline->GetSpecular();
        CurveObj["shininess"] = pSpline->GetShininess();

        QJsonArray ColorArray;
        ColorArray.append(pSpline->GetColor().x());
        ColorArray.append(pSpline->GetColor().y());
        ColorArray.append(pSpline->GetColor().z());
        CurveObj["color"] = ColorArray;

        QJsonArray KnotsArray;
        for (const auto &pKnot : pSpline->GetKnots())
        {
            QJsonObject KnotObj;
            KnotObj["x"] = pKnot->GetPosition().x();
            KnotObj["y"] = pKnot->GetPosition().y();
            KnotObj["z"] = pKnot->GetPosition().z();
            KnotsArray.append(KnotObj);
        }
        CurveObj["knots"] = KnotsArray;

        CurvesArray.append(CurveObj);
    }

    Root["version"] = "1.0";
    Root["curves"] = CurvesArray;

    QJsonDocument Doc(Root);
    QFile File(FilePath);

    if (!File.open(QIODevice::WriteOnly))
    {
        return false;
    }

    File.write(Doc.toJson(QJsonDocument::Indented));
    File.close();
    return true;
}

bool BSplineRenderer::CurveSerializer::LoadFromFile(const QString &FilePath, CurveContainer *pContainer)
{
    QFile File(FilePath);

    if (!File.open(QIODevice::ReadOnly))
    {
        return false;
    }

    QByteArray Data = File.readAll();
    File.close();

    QJsonDocument Doc = QJsonDocument::fromJson(Data);

    if (Doc.isNull())
    {
        return false;
    }

    QJsonObject Root = Doc.object();
    QJsonArray CurvesArray = Root["curves"].toArray();

    for (const auto &CurveVal : CurvesArray)
    {
        QJsonObject CurveObj = CurveVal.toObject();

        auto pSpline = std::make_shared<Spline>();

        pSpline->SetRadius(CurveObj["radius"].toDouble(0.25f));
        pSpline->SetAmbient(CurveObj["ambient"].toDouble(0.25f));
        pSpline->SetDiffuse(CurveObj["diffuse"].toDouble(0.5f));
        pSpline->SetSpecular(CurveObj["specular"].toDouble(0.25f));
        pSpline->SetShininess(CurveObj["shininess"].toDouble(4.0f));

        QJsonArray ColorArray = CurveObj["color"].toArray();
        if (ColorArray.size() >= 3)
        {
            pSpline->SetColor(QVector3D(ColorArray[0].toDouble(), ColorArray[1].toDouble(), ColorArray[2].toDouble()));
        }

        QJsonArray KnotsArray = CurveObj["knots"].toArray();
        for (const auto &KnotVal : KnotsArray)
        {
            QJsonObject KnotObj = KnotVal.toObject();
            float x = KnotObj["x"].toDouble();
            float y = KnotObj["y"].toDouble();
            float z = KnotObj["z"].toDouble();
            pSpline->AddKnot(x, y, z);
        }

        pContainer->AddCurve(pSpline);
    }

    return true;
}

QString BSplineRenderer::CurveSerializer::SplineToJson(SplinePtr pSpline)
{
    QJsonObject CurveObj;
    CurveObj["radius"] = pSpline->GetRadius();
    CurveObj["ambient"] = pSpline->GetAmbient();
    CurveObj["diffuse"] = pSpline->GetDiffuse();
    CurveObj["specular"] = pSpline->GetSpecular();
    CurveObj["shininess"] = pSpline->GetShininess();

    QJsonArray ColorArray;
    ColorArray.append(pSpline->GetColor().x());
    ColorArray.append(pSpline->GetColor().y());
    ColorArray.append(pSpline->GetColor().z());
    CurveObj["color"] = ColorArray;

    QJsonArray KnotsArray;
    for (const auto &Knot : pSpline->GetKnots())
    {
        QJsonObject KnotObj;
        KnotObj["x"] = Knot->GetPosition().x();
        KnotObj["y"] = Knot->GetPosition().y();
        KnotObj["z"] = Knot->GetPosition().z();
        KnotsArray.append(KnotObj);
    }
    CurveObj["knots"] = KnotsArray;

    QJsonDocument Doc(CurveObj);
    return QString(Doc.toJson(QJsonDocument::Compact));
}